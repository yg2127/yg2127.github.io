(() => {
  const main = document.querySelector('.article-reading .c-article__main');
  const toc = document.getElementById('toc');
  const list = document.getElementById('toc-list');
  const disclosure = document.getElementById('toc-disclosure');
  const desktop = matchMedia('(min-width: 1280px)');
  const reducedMotion = matchMedia('(prefers-reduced-motion: reduce)');
  const headers = main ? [...main.querySelectorAll('h2, h3')] : [];
  const tocLinks = [];
  const progress = document.getElementById('progress-bar');
  const back = document.getElementById('back-to-top');
  const announcement = document.getElementById('copy-status');
  let activeIndex = -1;
  let scheduled = false;
  async function copy(text, success) {
    try {
      await navigator.clipboard.writeText(text);
      announcement.textContent = success;
      return true;
    } catch (_) {
      announcement.textContent = '복사하지 못했습니다. 링크는 주소창에서, 코드는 직접 선택해 복사할 수 있습니다.';
      return false;
    }
  }
  headers.forEach((header, index) => {
    if (!header.id) header.id = `section-${index + 1}`;
    const title = header.textContent.trim();
    const href = '#' + encodeURIComponent(header.id);
    const permalink = document.createElement('a');
    permalink.className = 'heading-permalink';
    permalink.href = href;
    permalink.textContent = '#';
    permalink.setAttribute('aria-label', `${title} 링크 복사`);
    permalink.title = '이 문단 링크 복사';
    permalink.addEventListener('click', () => copy(new URL(href, location.href).href, '문단 링크를 복사했습니다.'));
    header.append(permalink);
    if (headers.length < 2 || !list) return;
    const item = document.createElement('li');
    item.className = 'toc-' + header.tagName.toLowerCase();
    const link = document.createElement('a');
    link.href = href;
    link.textContent = title;
    link.addEventListener('click', () => { if (!desktop.matches) disclosure.open = false; });
    item.append(link);
    list.append(item);
    tocLinks.push(link);
  });
  if (toc && tocLinks.length) {
    toc.hidden = false;
    disclosure.open = desktop.matches;
    desktop.addEventListener('change', event => { disclosure.open = event.matches; });
  }
  function updateReading() {
    scheduled = false;
    const scrollTop = window.scrollY;
    back.hidden = scrollTop < 300;
    if (main) {
      const start = main.getBoundingClientRect().top + scrollTop;
      const range = Math.max(1, main.offsetHeight - window.innerHeight);
      progress.style.width = `${Math.min(100, Math.max(0, (scrollTop - start) / range * 100))}%`;
    }
    if (!tocLinks.length) return;
    let current = 0;
    headers.forEach((header, index) => { if (header.getBoundingClientRect().top <= 130) current = index; });
    if (activeIndex === current) return;
    tocLinks.forEach((link, index) => {
      if (index === current) link.setAttribute('aria-current', 'location');
      else link.removeAttribute('aria-current');
    });
    activeIndex = current;
    if (desktop.matches && disclosure.open) {
      const nav = list.parentElement;
      const rect = tocLinks[current].getBoundingClientRect();
      const bounds = nav.getBoundingClientRect();
      if (rect.top < bounds.top || rect.bottom > bounds.bottom) nav.scrollTop += rect.top - bounds.top - nav.clientHeight / 3;
    }
  }
  function scheduleUpdate() {
    if (scheduled) return;
    scheduled = true;
    requestAnimationFrame(updateReading);
  }
  window.addEventListener('scroll', scheduleUpdate, { passive: true });
  window.addEventListener('resize', scheduleUpdate);
  window.addEventListener('load', scheduleUpdate);
  updateReading();
  back.addEventListener('click', () => window.scrollTo({ top: 0, behavior: reducedMotion.matches ? 'instant' : 'smooth' }));
  document.querySelectorAll('div.highlighter-rouge').forEach(block => {
    const code = block.querySelector('code');
    if (!code) return;
    const button = document.createElement('button');
    button.type = 'button';
    button.className = 'copy-btn';
    button.textContent = '복사';
    button.setAttribute('aria-label', '코드 복사');
    button.addEventListener('click', async () => {
      const copied = await copy(code.textContent, '코드를 복사했습니다.');
      button.textContent = copied ? '복사됨' : '복사 실패';
      setTimeout(() => { button.textContent = '복사'; }, 2000);
    });
    block.append(button);
  });
})();
