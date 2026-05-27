---
layout: post
title: "Python 입문 - 변수 선언"
description: "a = b 했을 때 복사일까 참조일까? 변수의 진짜 동작 원리"
date: 2026-01-12
category: "Python"
subcategory: "입문"
tags: python, variable
comments: true
---

## 들어가며

변수 선언은 프로그래밍의 가장 기본인데, Python의 변수는 다른 언어랑 동작 방식이 좀 다르다. C에서는 변수가 "값을 담는 상자"였다면, Python에서는 "값에 붙이는 이름표"에 가깝다. 이 차이를 모르면 나중에 리스트 복사할 때 큰일난다.

## 핵심 개념

### 변수와 메모리

- 파이썬은 `id()`를 통해서 메모리를 절약한다
- 같은 값을 가진 변수는 동일한 메모리 주소를 참조할 수 있다

### 네이밍 컨벤션

- **camelCase** → method에서 사용
- **PascalCase** → class에서 사용
- **snake_case** → 변수에서 사용

## 헷갈리기 쉬운 포인트

### 할당은 복사가 아니라 참조다

이게 진짜 중요한 건데, `a = [1, 2, 3]` 한 다음 `b = a` 하면 b는 a의 **복사본이 아니라 같은 리스트를 가리키는 다른 이름표**다. 그래서 `b.append(4)` 하면 a도 `[1, 2, 3, 4]`가 된다. `id(a) == id(b)`로 확인해보면 같은 주소를 참조하고 있다는 걸 알 수 있다.

진짜 복사하고 싶으면 `b = a[:]` 또는 `b = a.copy()`를 써야 한다.

### mutable vs immutable

Python의 자료형은 크게 두 종류로 나뉜다:

- **immutable** (수정 불가): int, float, str, tuple — 값을 바꾸면 새로운 객체가 생긴다
- **mutable** (수정 가능): list, dict, set — 값을 바꿔도 같은 객체다

immutable한 int는 `a = 10` 후 `a = 20` 하면, 10이 20으로 바뀌는 게 아니라 a가 20이라는 새로운 객체를 가리키게 되는 거다. 이 차이를 알아야 나중에 함수 인자 전달할 때 헷갈리지 않는다.

### 네이밍 컨벤션, 실수로 틀리기 쉬운 것

Python은 **snake_case가 표준**이다. Java에서 넘어오면 camelCase를 쓰고 싶겠지만, Python 커뮤니티에서는 변수와 함수 이름에 snake_case를 쓴다. PEP 8 스타일 가이드를 따르자.

## 정리하며

1. Python 변수는 "이름표"다 — 값을 담는 상자가 아니다
2. `=`는 복사가 아니라 참조 — 리스트 다룰 때 특히 주의
3. mutable/immutable 구분은 반드시 알아두자
4. 네이밍은 snake_case가 Python의 표준
