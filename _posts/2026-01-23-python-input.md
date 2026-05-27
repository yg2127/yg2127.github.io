---
layout: post
title: "Python 입문 - 입력 (input)"
description: "input()은 항상 문자열이라고? 형변환과 map 활용 다중 입력 팁"
date: 2026-01-23
category: "Python"
subcategory: "입문"
tags: python, input
comments: true
---

## 들어가며

사용자로부터 값을 입력받으려면 `input()` 함수를 쓴다. 간단해 보이는데, **input()은 항상 문자열(str)을 반환한다**는 걸 모르면 숫자 계산에서 이상한 결과가 나온다. "1 + 2 = 12"가 나오는 경험을 한 번쯤은 하게 된다.

---

## 핵심 개념

- `n = input()` → 기본 입력 형식은 **string**이다
- `int(input())`을 통해서 입력받은 숫자를 int 형식으로 변환한다
- `input("숫자를 입력해주세요")`처럼 입력 전에 안내 메시지를 표시할 수 있다
- `map(int, input().split())` — 이거 진짜 많이 쓴다

```python
x, y, z = map(int, input().split())
# 3 4 45 입력하면 x = 3, y = 4, z = 45로 된다
```

---

## 헷갈리기 쉬운 포인트

### input()은 무조건 str — 형변환 필수

```python
a = input()  # "5" 입력
b = input()  # "3" 입력
print(a + b)  # "53" 출력 — 문자열 연결이 됨!
```

숫자 계산을 원하면 `int(input())` 또는 `float(input())`으로 변환해야 한다.

### map(int, input().split()) 분해해서 이해하기

이 한 줄이 처음에 뭔 소린지 몰랐는데, 분해하면 간단하다:
1. `input()` → 사용자가 "3 4 45" 입력 → `"3 4 45"` (문자열)
2. `.split()` → 공백 기준으로 분리 → `["3", "4", "45"]` (문자열 리스트)
3. `map(int, ...)` → 각 원소에 `int()` 적용 → `3, 4, 45` (정수들)

코딩 테스트에서 거의 필수로 쓰이니까 외워두자.

---

## 정리하며

1. `input()`은 **항상 str 반환** — 숫자가 필요하면 형변환하자
2. 여러 값 한 줄 입력은 `map(int, input().split())` — 이거 외우자
3. 안내 메시지는 `input("메시지")` 형태로 넣을 수 있다
