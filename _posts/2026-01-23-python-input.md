---
layout: post
title: "Python 입문 - 입력 (input)"
description: "input 함수, 형변환, map을 활용한 다중 입력"
date: 2026-01-23
category: "Python"
subcategory: "입문"
tags: python, input
comments: true
---

## 입력 (input)

- `n = input()` → 기본 입력 형식은 string 형식이다
- `int(input())`을 통해서 입력받은 숫자를 int 형식으로 변환한다
- `input("숫자를 입력해주세요")`처럼 입력받기 전에 출력할 문자열을 지정할 수 있다
- `map(int, input().split())` ← 이거 진짜 많이 쓴다

```python
x, y, z = map(int, input().split())
# 3 4 45 입력하면 x = 3, y = 4, z = 45로 된다
```
