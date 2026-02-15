---
layout: post
title: "Python 입문 - 출력과 변수"
description: "print 함수 옵션, 변수 선언, 네이밍 컨벤션, 숫자 자료형"
date: 2026-01-11
category: "Python"
tags: python, print, variable
comments: true
---

## print 함수

- `sep` 옵션: 출력값 사이의 구분자를 지정
- `end` 옵션: `end=''`으로 하면 기본 개행문자(`\n`)를 제거
- `file` 옵션: 파일에 출력 가능
- `format` 함수: 포맷팅 출력

## 문자열 포맷팅

- **string 형식**: 자리수 확보, 자리수 제한
- **digit 형식**: `%04d` → 4자리로 출력
- **float 형식**: 자리수 확보 및 제한
- **f-string 문법**: 가장 편리한 포맷팅 방법

```python
name = "Python"
version = 3.12
print(f"{name} {version}")  # Python 3.12
```

## 변수 선언

- 파이썬은 `id()`를 통해 메모리를 절약한다
- 같은 값을 가진 변수는 동일한 메모리 주소를 참조할 수 있음

## 네이밍 컨벤션

| 스타일 | 용도 | 예시 |
|--------|------|------|
| camelCase | 메소드 | `myFunction` |
| PascalCase | 클래스 | `MyClass` |
| snake_case | 변수 | `my_variable` |

## 숫자 자료형

- `int('123')` → `123` (문자열을 정수로 형변환)
- 주요 연산자: `+`, `-`, `*`, `/`, `//`(몫), `%`(나머지), `**`(제곱), `abs()`(절댓값)
