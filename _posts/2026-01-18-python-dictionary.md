---
layout: post
title: "Python 입문 - 딕셔너리"
description: "key-value 쌍, 딕셔너리 선언과 주요 메서드"
date: 2026-01-18
category: "Python"
subcategory: "입문"
tags: python, dictionary
comments: true
---

## 딕셔너리

- 딕셔너리는 key, value 쌍으로 이뤄진다. 순서는 없으며 key가 중복이 되지 않는다

```python
a = {'name': 'Kim', 1: '01041846662', 'birth': '010101', 'how many': 10}

f = dict(
    Name='niceman',
    City='Seoul',
    Age=33,
    Grade='A',
    Status=True
)
```

## 주요 메서드

- `get()` → key를 입력하면 value값을 반환한다. key가 없어도 오류가 일어나지 않고 None을 반환한다
- `keys()` → 키값들만 출력 (tuple 형식, `list()`로 리스트 형태 변경 가능)
- `values()` → value값들만 출력 (tuple 형식)
- `items()` → keys, values 모두 쌍으로 출력
- `pop(key)` → 해당 key와 value를 제거하고 value를 반환한다
- `popitem()` → 무작위 key와 value를 제거하고 둘 다 반환한다
- `update()` → 수정할 수 있다
- `del['key']` 또는 `del[num]`으로 해당 원소를 지울 수 있다
