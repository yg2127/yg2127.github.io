---
layout: post
title: "Python 입문 - 내장 함수"
description: "map, filter, enumerate — 이것만 알면 반복문이 절반으로 줄어든다"
date: 2026-01-29
category: "Python"
subcategory: "입문"
tags: python, builtin
comments: true
---

## 들어가며

Python에는 import 없이 바로 쓸 수 있는 **내장 함수**들이 있다. 전부 외울 필요는 없고, 자주 쓰는 것들만 알아두면 코드가 훨씬 간결해진다. 특히 `map`, `filter`, `enumerate`는 반복문을 줄여주는 핵심 함수다.

---

## 핵심 개념

자주 사용되는 내장 함수들:

- **abs**: 절댓값 반환
- **all**: iterable의 모든 원소가 참일 때 True
- **any**: iterable의 원소 중 하나라도 참이면 True
- **chr, ord**: 아스키코드 ↔ 문자 변환
- **enumerate**: 인덱스와 원소를 함께 반환
- **filter**: 조건에 맞는 값만 반환 (tuple 형식)
- **len**: 요소의 길이 반환
- **max, min**: 최대값, 최소값 반환
- **map**: 모든 원소에 함수를 적용한 결과 반환
  - `x, y, z = map(int, input().split(' '))` ← 이거 엄청 많이 쓴다
- **range**: 반복 가능한 숫자 시퀀스 생성
- **type**: 객체의 자료형 반환

---

## 헷갈리기 쉬운 포인트

### map vs list comprehension

```python
# map 사용
result = list(map(lambda x: x**2, [1, 2, 3]))

# list comprehension
result = [x**2 for x in [1, 2, 3]]
```

둘 다 같은 결과인데, Python에서는 **list comprehension이 더 Pythonic**하다고 여겨진다. 단순 변환이면 list comprehension, 이미 있는 함수를 적용할 때는 map이 깔끔하다 (예: `map(int, strings)`).

### filter의 반환값

`filter()`는 리스트가 아니라 **iterator**를 반환한다. `list()`로 감싸줘야 리스트가 된다. `map()`도 마찬가지. 이걸 모르면 "왜 출력이 이상하지?" 하게 된다.

### all()과 any()의 빈 리스트

- `all([])` → `True` (비어있으면 "모두 참" 조건이 자동 충족)
- `any([])` → `False` (비어있으면 "하나라도 참"인 게 없음)

---

## 정리하며

1. `map()`은 모든 원소에 함수 적용, `filter()`는 조건 필터링
2. 단순 변환은 list comprehension이 더 Pythonic하다
3. `map`, `filter`는 iterator 반환 — `list()`로 감싸야 리스트가 된다
