---
layout: post
title: "Python 입문 - 문자열"
description: "문자열 선언, 슬라이싱, 주요 메소드 정리"
date: 2026-01-12
category: "Python"
tags: python, string
comments: true
---

## 문자열의 특성

문자열은 **immutable**(변경 불가)하다. C언어와 동일하게 개별 문자에 접근은 가능하지만 직접 수정은 불가능하다.

```python
str1 = 'abcdef'
str1[3] = '2'  # TypeError! 수정 불가
```

> mutable: list처럼 각 원소에 접근하고 수정할 수 있는 것
> immutable: string처럼 개별 원소를 직접 수정할 수 없는 것

## 기본 문법

- 선언: `a = 'hello'`
- 길이: `len(a)`
- 이스케이프: `\n`(개행), `\t`(탭)

## 문자열 연산

- 더하기: `'abc' + 'def'` → `'abcdef'` (concat)
- 곱하기: `'abc' * 3` → `'abcabcabc'`
- 형변환: `int("66")` → `66`

## 슬라이싱

```python
a = 'abcdef'
a[1:4]    # 'bcd' (이상:미만)
a[::2]    # 'ace' (간격 2)
a[::-1]   # 'fedcba' (역순 출력!)
```

슬라이싱 문법: `[이상:미만:간격]`

## 주요 메소드

| 메소드 | 설명 |
|--------|------|
| `upper()` | 대문자로 변환 |
| `lower()` | 소문자로 변환 |
| `isupper()` | 대문자인지 확인 (bool) |
| `islower()` | 소문자인지 확인 (bool) |
