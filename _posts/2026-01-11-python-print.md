---
layout: post
title: "Python 입문 - print 출력"
description: "print 함수의 sep, end, file 옵션과 format, f-string 문법"
date: 2026-01-11
category: "Python"
subcategory: "입문"
tags: python, print, format
comments: true
---

## print 출력

- `sep` 옵션으로 출력값 사이의 구분자를 지정할 수 있다
- `end` 옵션에 `end=''`을 주면 원래 print 내 문자열 맨 마지막에 있는 `\n` 개행문자를 제거한다
- `file` 옵션으로 파일에 출력할 수 있다
- `format` 함수를 사용해 포맷팅 출력이 가능하다

## 포맷팅 문법

- **string 형식**: 자리수 확보, 자리수 제한 등
- **digit 형식**: `%04d`하면 4자리만 출력된다
- **float 형식**: 자리수 확보, 제한
- **f-string 문법**: 가장 편리한 포맷팅 방법이다. 꼭 쓰자

```python
name = "Python"
version = 3.12
print(f"{name} {version}")  # Python 3.12
```
