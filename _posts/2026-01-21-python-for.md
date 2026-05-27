---
layout: post
title: "Python 입문 - 반복문 (for)"
description: "range(1, 11)이 왜 10까지인지, enumerate는 언제 쓰는 건지 — for문 완전 정복"
date: 2026-01-21
category: "Python"
subcategory: "입문"
tags: python, for, loop
comments: true
---

## 들어가며

같은 코드를 여러 번 실행해야 할 때 for문을 쓴다. 단순해 보이는데, `range()`의 범위 규칙이나 중첩 반복문에서 인덱스 추적하는 게 은근 헷갈린다. 특히 `enumerate()`의 존재를 모르면 불필요하게 복잡한 코드를 작성하게 된다.

---

## 핵심 개념

### for문 기본

`for i in range(1, 11)` — 이게 기본 형태다. `in` 다음에는 range 함수뿐만 아니라 리스트, 문자열 등 **iterable한 객체**면 뭐든 올 수 있다.

```python
a = ['kim', 'lee', 'park', 'na']

for x in a:
    print(f'first name is {x}')
```

리스트 안의 변수 형식이 정수든 텍스트든 상관없다. for문은 리스트의 원소를 하나씩 꺼내서 x에 넣어주는 거다.

### break와 continue

- `break` — 반복문을 즉시 탈출한다
- `continue` — 현재 반복만 건너뛰고 다음 반복으로 넘어간다

---

## 헷갈리기 쉬운 포인트

### range(1, 11)이 왜 1부터 10까지인지

`range(start, stop)`에서 stop은 **포함되지 않는다**. 슬라이싱의 `[이상:미만]`과 같은 규칙이다. `range(5)`는 0, 1, 2, 3, 4를 생성한다. 처음에 "왜 직관적으로 안 만들었지?" 싶었는데, 이렇게 하면 `range(len(리스트))`로 인덱스를 깔끔하게 순회할 수 있어서 그런 거다.

### enumerate() — 인덱스가 필요할 때

인덱스와 값을 동시에 쓰고 싶으면 `enumerate()`를 쓰자. `for i in range(len(a))` 이렇게 하면 되긴 하는데, Pythonic하지 않다.

```python
# 이렇게 하지 말고
for i in range(len(a)):
    print(i, a[i])

# 이렇게 하자
for i, v in enumerate(a):
    print(i, v)
```

### 중첩 반복문에서 변수 이름

2중 for문을 쓸 때 `i`, `j`를 쓰는 건 관례인데, 3중이 되면 `k`까지 가고 그 이상은 가독성이 떨어진다. 의미 있는 이름을 쓰는 게 좋다. 예: `for row in matrix:` `for col in row:`

---

## 정리하며

1. `range(start, stop)`에서 **stop은 미포함** — 슬라이싱과 같은 규칙
2. 인덱스가 필요하면 `enumerate()` 쓰자 — `range(len())`보다 깔끔하다
3. `break`는 탈출, `continue`는 건너뛰기
