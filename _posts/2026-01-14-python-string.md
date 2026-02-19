---
layout: post
title: "Python 입문 - 문자열"
description: "문자열 선언, 슬라이싱, 주요 메서드, immutable 특성"
date: 2026-01-14
category: "Python"
subcategory: "입문"
tags: python, string
comments: true
---

## 문자열 기초

- 문자열은 수정이 안 된다. C언어랑 똑같다
  - 이걸 'mutable하지 않다', 'immutable하다'고 한다. mutable은 list처럼 각 원소에 접근하고 수정할 수 있는 것을 말하는데, immutable한 string은 `str1 = 'abcdef'`일 때 `str1[3] = '2'` 이런 식의 수정이 안 된다
- 문자열 선언: `a = 'fdsaffdas'` 이런 식으로 선언한다
- `len(a)`하면 str 변수 a의 길이를 반환한다
- `\n` = 개행문자 (줄내림)
- `\t` = tab

## 문자열 연산

- 문자열 연산이 가능하다 (그대로 복붙한 게 반복되거나 이어붙여지거나(concat))
- 문자열 형변환이 가능하다 (`int("66")` → `66` 정수형 데이터로 변환된다)

## 슬라이싱

- `[이상:미만:간격]` 형식으로 슬라이싱한다

```python
a = 'abcdef'
a[::-1]  # 문자열 전체를 뒤에서부터 역순으로 출력한다
```

## 주요 메서드

- `upper()`, `lower()` → 대문자 변환, 소문자 변환
- `isupper()`, `islower()` → 대문자인지, 소문자인지 확인하여 반환
