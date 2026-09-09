"""Check the published learning order and the reading path before each article."""
import json
import re
import sys
from html.parser import HTMLParser
from pathlib import Path


class Element:
    def __init__(self, tag='', attrs=()):
        self.tag, self.attrs, self.children = tag, dict(attrs), []

    def text(self):
        return ''.join(child if isinstance(child, str) else child.text() for child in self.children)

    def find(self, tag=None, css=None):
        result = []
        for child in self.children:
            if isinstance(child, Element):
                if (tag is None or child.tag == tag) and (css is None or css in child.attrs.get('class', '').split()):
                    result.append(child)
                result.extend(child.find(tag, css))
        return result


class Document(HTMLParser):
    VOID = {'area', 'base', 'br', 'col', 'embed', 'hr', 'img', 'input', 'link', 'meta', 'param', 'source', 'track', 'wbr'}

    def __init__(self, text):
        super().__init__()
        self.root = Element()
        self.stack = [self.root]
        self.feed(text)

    def handle_starttag(self, tag, attrs):
        node = Element(tag, attrs)
        self.stack[-1].children.append(node)
        if tag not in self.VOID:
            self.stack.append(node)

    def handle_startendtag(self, tag, attrs):
        self.handle_starttag(tag, attrs)
        if tag not in self.VOID:
            self.handle_endtag(tag)

    def handle_endtag(self, tag):
        for i in range(len(self.stack) - 1, 0, -1):
            if self.stack[i].tag == tag:
                del self.stack[i:]
                break

    def handle_data(self, data):
        self.stack[-1].children.append(data)


def metadata(path):
    front = path.read_text().split('---', 2)[1]
    fields = {}
    for line in front.splitlines():
        if ':' in line and not line.startswith(' '):
            key, value = line.split(':', 1)
            fields[key] = value.strip().strip('"').strip("'")
    return fields


def check(site):
    source = Path(__file__).resolve().parents[1]
    index = {entry['url']: entry for entry in json.loads((site / 'search.json').read_text())}
    total = 0
    for series, count, route in [('data-structure', 10, 'data-structure'), ('algorithms', 14, 'algorithm')]:
        posts = [metadata(p) for p in (source / '_posts').glob('*.md')]
        posts = sorted((p for p in posts if p.get('series') == series), key=lambda p: int(p['series_order']))
        assert [int(p['series_order']) for p in posts] == list(range(1, count + 1)), series
        urls = [p['permalink'] for p in posts]
        listing = Document((site / 'stack' / route / 'index.html').read_text()).root
        rows = listing.find(css='record')
        links = [row.find(tag='h3')[0].find(tag='a')[0] for row in rows]
        assert [link.attrs['href'] for link in links] == urls, f'{series}: Stack must follow learning order, not date/title'
        assert all(re.match(rf'{i}\.\s', link.text().strip()) for i, link in enumerate(links, 1)), series

        for i, post in enumerate(posts):
            url = post['permalink']
            text = (site / url.lstrip('/')).read_text()
            document = Document(text).root
            title = document.find(tag='h1', css='c-article__title')[0].text().strip()
            assert title.startswith(f'{i + 1}. '), url
            assert index[url]['seriesOrder'] == i + 1, f'{url}: search numbering'
            bridges = document.find(tag='nav', css='series-bridge')
            assert len(bridges) == 1, url
            bridge = bridges[0]
            assert len(bridge.find(tag='dt')) == 3 and all(p.text().strip() for p in bridge.find(tag='p')), url
            prev = [a.attrs['href'] for a in bridge.find(tag='a') if a.attrs.get('rel') == 'prev']
            next_ = [a.attrs['href'] for a in bridge.find(tag='a') if a.attrs.get('rel') == 'next']
            assert prev == ([urls[i - 1]] if i else []), f'{url}: previous article'
            assert next_ == ([urls[i + 1]] if i + 1 < count else []), f'{url}: next article'
            assert text.index('class="series-bridge"') < text.index('class="c-article__main"'), url
            assert len(document.find(css='series-nav__list')[0].find(tag='li')) == count, url
            assert not document.find(css='pagenav'), f'{url}: avoid a second chronological previous/next path'
            assert all(post.get(k) for k in ('series_intro', 'series_from', 'series_to')), url
            total += 1
    print(f'OK: {total} articles; numbered titles/search; ordered Stack; previous/current/next context before content')


if __name__ == '__main__':
    check(Path(sys.argv[1]) if len(sys.argv) > 1 else Path('_site'))
