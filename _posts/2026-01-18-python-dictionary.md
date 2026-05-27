---
layout: post
title: "Python 입문 - 딕셔너리"
description: "get()이랑 []의 차이를 모르면 KeyError 지옥에 빠진다"
date: 2026-01-18
category: "Python"
subcategory: "입문"
tags: python, dictionary
comments: true
---

## 들어가며

딕셔너리는 key-value 쌍으로 데이터를 저장하는 자료형인데, Python에서 정말 많이 쓴다. JSON도 딕셔너리 구조고, API 응답 파싱할 때도 딕셔너리를 쓴다. 처음에 `dict['key']`로 접근하다가 KeyError를 만나서 고생했는데, `get()`을 알고 나서 세상이 편해졌다.

## 핵심 개념

### 딕셔너리 기초

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

### 주요 메서드

- `get()` → key를 입력하면 value값을 반환한다. key가 없어도 오류가 일어나지 않고 None을 반환한다
- `keys()` → 키값들만 출력 (tuple 형식, `list()`로 리스트 형태 변경 가능)
- `values()` → value값들만 출력 (tuple 형식)
- `items()` → keys, values 모두 쌍으로 출력
- `pop(key)` → 해당 key와 value를 제거하고 value를 반환한다
- `popitem()` → 무작위 key와 value를 제거하고 둘 다 반환한다
- `update()` → 수정할 수 있다
- `del['key']` 또는 `del[num]`으로 해당 원소를 지울 수 있다

## 헷갈리기 쉬운 포인트

### get() vs [] — 이 차이가 핵심이다

- `dict['key']`: key가 없으면 **KeyError** 터진다
- `dict.get('key')`: key가 없으면 **None** 반환 (에러 없음)
- `dict.get('key', '기본값')`: key가 없으면 지정한 기본값 반환

실무에서는 `get()`을 훨씬 많이 쓴다. 데이터가 항상 완전하다는 보장이 없으니까. API 응답 파싱할 때 `get()`을 안 쓰면 KeyError 지옥이다.

### key가 될 수 있는 것, 없는 것

딕셔너리의 key는 **immutable한 자료형만** 가능하다:
- 가능: str, int, float, tuple
- **불가능**: list, dict, set (mutable이라서)

이건 해시(hash) 때문인데, mutable한 객체는 값이 바뀔 수 있어서 해시값이 바뀌고 딕셔너리 내부 구조가 망가진다.

### dict comprehension — 알아두면 코드가 깔끔해진다

리스트 컴프리헨션처럼 딕셔너리도 한 줄로 만들 수 있다:

```python
# 1~5의 제곱을 딕셔너리로
squares = {x: x**2 for x in range(1, 6)}
# {1: 1, 2: 4, 3: 9, 4: 16, 5: 25}
```

for문으로 빈 딕셔너리에 하나씩 넣는 것보다 이게 훨씬 Pythonic하다.

## 정리하며

1. **`get()` 쓰자** — `[]`는 KeyError 위험이 있다
2. key는 immutable만 가능 — list는 key가 될 수 없다
3. dict comprehension으로 깔끔하게 딕셔너리를 만들 수 있다
