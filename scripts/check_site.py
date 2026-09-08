"""Check generated routes, fragment links, RSS, and the public search index."""
import json
import sys
import xml.etree.ElementTree as ET
from collections import Counter
from html.parser import HTMLParser
from pathlib import Path
from urllib.parse import unquote, urlsplit


class Page(HTMLParser):
    def __init__(self, text):
        super().__init__()
        self.links, self.ids, self.titles = [], set(), 0
        self.feed(text)

    def handle_starttag(self, tag, attrs):
        attrs = dict(attrs)
        if 'id' in attrs:
            self.ids.add(attrs['id'])
        if tag == 'title':
            self.titles += 1
        if tag == 'a' and attrs.get('href'):
            self.links.append(attrs['href'])
        if tag in ('img', 'script') and attrs.get('src'):
            self.links.append(attrs['src'])
        if tag == 'link' and attrs.get('rel') == 'stylesheet':
            self.links.append(attrs['href'])


def check(root):
    root = root.resolve()
    pages = {path: Page(path.read_text()) for path in root.rglob('*.html')}
    errors = []
    for path, page in pages.items():
        if page.titles != 1:
            errors.append(f'{path.relative_to(root)}: expected one title')
        for link in page.links:
            parts = urlsplit(link)
            if parts.scheme or parts.netloc:
                continue
            dest = unquote(parts.path)
            target = root / dest.lstrip('/') if dest.startswith('/') else path.parent / dest if dest else path
            if target.is_dir():
                target /= 'index.html'
            target = target.resolve()
            if not target.exists():
                errors.append(f'{path.relative_to(root)}: missing {link}')
            elif parts.fragment and target in pages and unquote(parts.fragment) not in pages[target].ids:
                errors.append(f'{path.relative_to(root)}: missing fragment {link}')
    entries = json.loads((root / 'search.json').read_text())
    urls = [entry['url'] for entry in entries]
    if len(set(urls)) != len(urls):
        errors.append('Search index contains duplicate URLs')
    for entry in entries:
        path = root / entry['url'].lstrip('/')
        if path.is_dir():
            path /= 'index.html'
        if path not in pages:
            errors.append(f'Search result has no output page: {entry["url"]}')
        if not entry['title'] or not entry['datetime']:
            errors.append(f'Missing search metadata: {entry["url"]}')
        if any(tag.endswith(',') for tag in entry.get('tags') or []):
            errors.append(f'Comma in tag: {entry["url"]}')
    ET.parse(root / 'feed.xml')
    for route in ('notes', 'archive', 'stack/project'):
        if not (root / route / 'index.html').exists():
            errors.append(f'Missing route: /{route}/')
    if errors:
        raise SystemExit('\n'.join(errors))
    print(f'OK: {len(pages)} HTML pages, {len(entries)} search entries, internal links, anchors, and RSS')
    print('Record kinds:', dict(Counter(entry['kind'] for entry in entries)))


if __name__ == '__main__':
    check(Path(sys.argv[1]) if len(sys.argv) > 1 else Path('_site'))
