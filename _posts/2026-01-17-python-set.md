---
layout: post
title: "Python 입문 - 집합(Set)"
description: "집합 선언, 집합 연산, 주요 메서드"
date: 2026-01-17
category: "Python"
subcategory: "입문"
tags: python, set
comments: true
---

## 집합(Set)

- 집합은 순서가 없고 중복이 되지 않는다

```python
b = set([1, 2, 3, 4, 4, 4, 4])
e = {1, 2, 3, ('f', 'i', 'x'), False, '~dict', 'set'}
```

## 집합 연산

- `a & b` = a, b의 교집합
- `a | b` = a, b의 합집합
- `a - b` = a, b의 차집합

## 주요 메서드

- `len()` → 길이 출력
- `isdisjoint()` → 중복 원소가 있는지(독립인지) bool 형식 반환
- `issubset()` → 부분집합인지 bool 형식 반환
- `add()` → 원소 추가
- `remove()` → 원소 제거
- `discard()` → 원소 제거
- `clear()` → 모든 원소 제거
