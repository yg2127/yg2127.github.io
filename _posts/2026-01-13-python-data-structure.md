---
layout: post
title: "Python 입문 - 자료구조"
description: "list, tuple, dictionary, set의 특징과 주요 메소드 정리"
date: 2026-01-13
category: "Python"
tags: python, list, tuple, dictionary, set
comments: true
---

## List (리스트)

배열 안에 `int`, `float`, `bool`, `str`, 심지어 다른 리스트까지 들어갈 수 있다.

### 기본 조작

```python
a = [1, 2, 3]
a.append(4)       # [1, 2, 3, 4] - 원소 추가
a.sort()           # 오름차순 정렬
a.reverse()        # 내림차순 정렬
a.index(3)         # 원소 3의 인덱스 반환
a.insert(1, 'x')   # 인덱스 1에 'x' 삽입
a.remove(3)        # 원소 3 제거
a.pop()            # 마지막 원소 제거 후 반환
a.pop(0)           # 첫 번째 원소 제거 후 반환
a.extend(b)        # a + b와 동일
del a[3]           # 인덱스 3의 원소 삭제
```

### 슬라이싱으로 삭제

```python
c[1:3] = []  # 인덱스 1~2 원소를 제거
```

## Tuple (튜플)

**수정/삭제가 불가능한 리스트**. 소괄호로 선언한다.

```python
a = (1, 2, 3)
```

- 인덱싱, 슬라이싱, 더하기, 곱하기 연산은 리스트와 동일

### Packing & Unpacking

```python
v = [1, 2, 3, 4]       # packing
v1, v2, v3, v4 = v      # unpacking → v1=1, v2=2, v3=3, v4=4
```

파이썬에서 매우 자주 쓰이는 패턴이므로 잘 알아두자.

## Dictionary (딕셔너리)

**key-value 쌍**으로 구성. 순서 없음, key 중복 불가.

```python
a = {'name': 'Kim', 'birth': '010101', 'age': 25}

# dict() 생성자로도 선언 가능
f = dict(Name='niceman', City='Seoul', Age=33)
```

### 주요 메소드

| 메소드 | 설명 |
|--------|------|
| `get(key)` | value 반환 (key 없으면 `None` 반환, 에러 없음) |
| `keys()` | 모든 key 반환 |
| `values()` | 모든 value 반환 |
| `items()` | key-value 쌍을 모두 반환 |
| `pop(key)` | 해당 key-value 제거 후 value 반환 |
| `popitem()` | 무작위 key-value 제거 후 반환 |
| `update()` | 값 수정 |

## Set (집합)

**순서 없음, 중복 불가**.

```python
b = set([1, 2, 3, 4, 4, 4, 4])  # {1, 2, 3, 4}
e = {1, 2, 3, ('f', 'i', 'x'), False, 'set'}
```

### 집합 연산

| 연산 | 문법 | 설명 |
|------|------|------|
| 교집합 | `a & b` | 공통 원소 |
| 합집합 | `a \| b` | 모든 원소 |
| 차집합 | `a - b` | a에만 있는 원소 |

### 주요 메소드

| 메소드 | 설명 |
|--------|------|
| `isdisjoint()` | 독립(공통 원소 없음)인지 확인 |
| `issubset()` | 부분집합인지 확인 |
| `add()` | 원소 추가 |
| `remove()` | 원소 제거 |
| `discard()` | 원소 제거 |
| `clear()` | 모든 원소 제거 |
