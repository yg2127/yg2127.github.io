/* Search enhances the original chronological archive; all posts remain readable without JS. */
(() => {
  const root = document.querySelector('[data-search-index]');
  if (!root) return;
  const input = root.querySelector('input[type="search"]');
  const filters = root.querySelector('.record-filters');
  const sort = root.querySelector('#record-sort');
  const results = root.querySelector('#record-results');
  const status = root.querySelector('.search-status');
  const heading = root.querySelector('#records-title');
  const empty = root.querySelector('#search-empty');
  const original = [...results.childNodes].map(node => node.cloneNode(true));
  const labels = { all: '전체', article: '해설글', note: '짧은 노트', reflection: '회고' };
  let kind = 'all', indexPromise, generation = 0, timer;
  const normalize = value => String(value || '').normalize('NFC').toLocaleLowerCase();
  const element = (tag, className, text) => {
    const node = document.createElement(tag);
    if (className) node.className = className;
    if (text !== undefined) node.textContent = text;
    return node;
  };
  function loadIndex() {
    if (!indexPromise) indexPromise = fetch(root.dataset.searchIndex).then(response => {
      if (!response.ok) throw new Error('Search index unavailable');
      return response.json();
    }).then(entries => entries.map(entry => ({
      ...entry,
      searchable: normalize([entry.title, entry.description, entry.topic, entry.tags, entry.content].join(' ')),
      normalizedTitle: normalize(entry.title), normalizedTags: normalize(entry.tags)
    }))).catch(error => { indexPromise = undefined; throw error; });
    return indexPromise;
  }
  function renderRecord(entry, tokens) {
    const row = element('li', 'c-archives__item record');
    const body = element('div', 'record__body');
    const title = element('h3');
    const displayTitle = entry.seriesOrder ? `${entry.seriesOrder}. ${entry.title}` : entry.title;
    const link = element('a', '', displayTitle);
    link.href = entry.url;
    title.append(link); body.append(title);
    let excerpt = entry.description;
    if (tokens.length && !tokens.every(token => normalize(excerpt).includes(token))) {
      const position = normalize(entry.content).indexOf(tokens[0]);
      if (position >= 0) {
        const start = Math.max(0, position - 45);
        excerpt = (start ? '…' : '') + entry.content.slice(start, start + 150) + (entry.content.length > start + 150 ? '…' : '');
      }
    }
    if (excerpt) body.append(element('p', 'record__description', excerpt));
    if (entry.kind !== 'article' || entry.status) body.append(element('p', 'record__kind', [entry.kind === 'article' ? '' : labels[entry.kind], entry.status].filter(Boolean).join(' · ')));
    if (entry.updated) body.append(element('p', 'record__updated', `수정 ${entry.updated}${entry.updateNote ? ' · ' + entry.updateNote : ''}`));
    const date = element('p', 'record__date');
    const time = element('time', '', entry.displayDate || entry.date);
    time.dateTime = entry.datetime;
    date.append(time); row.append(body, date);
    return row;
  }
  function renderGroups(entries) {
    const fragment = document.createDocumentFragment();
    let previousYear, previousMonth, section, list;
    entries.forEach(entry => {
      const [year, month] = entry.date.split('.');
      if (year !== previousYear) {
        section = element('section', 'record-year');
        const title = element('h2', 'c-archives__year', year);
        title.id = `year-${year}`;
        section.setAttribute('aria-labelledby', title.id);
        section.append(title); fragment.append(section);
        previousYear = year; previousMonth = undefined;
      }
      if (month !== previousMonth) {
        section.append(element('h3', 'c-archives__month', `${month}월`));
        list = element('ul', 'c-archives__list'); section.append(list);
        previousMonth = month;
      }
      list.append(renderRecord(entry, []));
    });
    return fragment;
  }
  function updateUrl() {
    const url = new URL(location.href);
    for (const [key, value] of [['q', input.value.trim()], ['kind', kind === 'all' ? '' : kind], ['sort', sort.value === 'newest' ? '' : sort.value]]) {
      if (value) url.searchParams.set(key, value); else url.searchParams.delete(key);
    }
    history.replaceState(null, '', url);
  }
  async function render(syncUrl = true) {
    const current = ++generation;
    const query = input.value.trim();
    const tokens = normalize(query).split(/\s+/).filter(Boolean);
    filters.querySelectorAll('button').forEach(button => button.setAttribute('aria-pressed', String(button.dataset.kind === kind)));
    if (syncUrl) updateUrl();
    // Default home does not need a network request or a client-rendered replacement.
    if (!query && kind === 'all' && sort.value === 'newest') {
      results.replaceChildren(...original.map(node => node.cloneNode(true)));
      empty.hidden = true; status.textContent = ''; heading.textContent = '모든 기록';
      root.removeAttribute('aria-busy');
      return;
    }
    status.textContent = '기록을 찾고 있습니다…';
    root.setAttribute('aria-busy', 'true');
    try {
      const entries = await loadIndex();
      if (current !== generation) return;
      const matches = entries.filter(entry => (kind === 'all' || entry.kind === kind) && tokens.every(token => entry.searchable.includes(token)));
      if (sort.value === 'updated') {
        matches.sort((a, b) => (b.updated || b.date).localeCompare(a.updated || a.date) || b.datetime.localeCompare(a.datetime));
      } else if (tokens.length) {
        const score = entry => tokens.reduce((sum, token) => sum + (entry.normalizedTitle.includes(token) ? 10 : 0) + (entry.normalizedTags.includes(token) ? 3 : 0), 0);
        matches.sort((a, b) => score(b) - score(a));
      }
      heading.textContent = query ? '검색 결과' : labels[kind];
      status.textContent = query ? `“${query}” · ${labels[kind]} ${matches.length}개` : `${labels[kind]} ${matches.length}개${sort.value === 'updated' ? ' · 수정일이 없는 글은 작성일 기준' : ''}`;
      if (!query && sort.value === 'newest') results.replaceChildren(renderGroups(matches));
      else {
        const list = element('ul', 'c-archives__list');
        list.append(...matches.map(entry => renderRecord(entry, tokens)));
        results.replaceChildren(list);
      }
      empty.hidden = matches.length > 0;
      if (!matches.length) {
        empty.querySelector('.empty-state__title').textContent = query ? '검색 결과가 없습니다.' : `아직 ${labels[kind]}가 없습니다.`;
        empty.querySelector('.empty-state__description').textContent = query ? '다른 단어나 기록 종류로 찾아보세요.' : '새 기록이 쌓이면 이곳에 모입니다.';
      }
    } catch (_) {
      if (current !== generation) return;
      results.replaceChildren(); empty.hidden = false;
      status.textContent = '검색을 불러오지 못했습니다. 다시 시도해 주세요.';
      empty.querySelector('.empty-state__title').textContent = '현재 검색 결과를 표시할 수 없습니다.';
      empty.querySelector('.empty-state__description').textContent = '전체 기록으로 돌아가거나 검색을 다시 시도할 수 있습니다.';
    } finally { if (current === generation) root.removeAttribute('aria-busy'); }
  }
  function readUrl() {
    const params = new URLSearchParams(location.search);
    input.value = params.get('q') || '';
    kind = Object.hasOwn(labels, params.get('kind')) ? params.get('kind') : 'all';
    sort.value = params.get('sort') === 'updated' ? 'updated' : 'newest';
    render(false);
  }
  root.querySelector('.record-toolbar').hidden = false;
  input.addEventListener('input', () => { clearTimeout(timer); timer = setTimeout(render, 120); });
  root.querySelector('form').addEventListener('submit', event => { event.preventDefault(); clearTimeout(timer); render(); });
  filters.addEventListener('click', event => {
    const button = event.target.closest('[data-kind]');
    if (button) { clearTimeout(timer); kind = button.dataset.kind; render(); }
  });
  sort.addEventListener('change', () => { clearTimeout(timer); render(); });
  root.querySelector('#reset-search').addEventListener('click', () => { input.value = ''; kind = 'all'; sort.value = 'newest'; render(); input.focus(); });
  window.addEventListener('popstate', readUrl);
  readUrl();
})();
