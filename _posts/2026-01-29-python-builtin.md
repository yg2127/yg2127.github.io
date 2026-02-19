---
layout: post
title: "Python 입문 - 내장 함수"
description: "자주 사용되는 파이썬 내장 함수 정리"
date: 2026-01-29
category: "Python"
subcategory: "입문"
tags: python, builtin
comments: true
---

## 내장 함수

자주 사용되는 유용한 파이썬 내장 함수를 소개한다.

- **abs**: 절댓값 반환
- **all**: iterable 객체(list, tuple, dict, set)의 모든 원소가 참일 때 참 반환
- **any**: iterable 객체의 원소 중 하나라도 참일 때 참 반환
- **chr, ord**: 각각 아스키코드 → 문자, 문자 → 아스키코드 반환
- **enumerate**: iterable한 객체에 사용 시 인덱스와 원소를 함께 반환한다
- **filter**: iterable한 객체의 원소들 중 지정한 조건에 맞는 값만 반환한다 (tuple 형식)
- **len**: 요소의 길이 반환
- **max, min**: iterable한 객체의 최대, 최소 반환
- **map**: iterable한 객체의 모든 원소를 지정한 함수로 실행한 뒤에 반환한다
  - `x, y, z = map(int, input().split(' '))` ← 이거 엄청 많이 쓴다
- **range**: 반복 가능한 객체를 반환
- **type**: 객체의 자료형을 반환
