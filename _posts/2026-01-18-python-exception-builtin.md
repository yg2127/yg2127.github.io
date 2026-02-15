---
layout: post
title: "Python 입문 - 예외처리와 내장함수"
description: "try-except 예외처리, 주요 내장함수, 외장함수 정리"
date: 2026-01-18
category: "Python"
tags: python, exception, builtin
comments: true
---

## 주요 에러 종류

| 에러 | 발생 상황 |
|------|-----------|
| `SyntaxError` | 코드 문법 오류 |
| `NameError` | 없는 변수에 접근 |
| `ZeroDivisionError` | 0으로 나누기 |
| `IndexError` | 없는 인덱스 접근 |
| `KeyError` | 없는 딕셔너리 키 접근 |
| `AttributeError` | 잘못된 속성 사용 |
| `ValueError` | 없는 원소를 제거 시도 |
| `FileNotFoundError` | 없는 파일 접근 |
| `TypeError` | 자료형이 맞지 않는 연산 |

## try-except-else

```python
name = ['Kim', 'Lee', 'Park']

try:
    z = 'Cho'
    x = name.index(z)
    print(f'{z} found it! {x+1} in name')
except:
    print('not found it! - Occured ValueError!')
else:
    print('Ok! else')  # try가 정상 실행됐을 때 실행
```

## raise로 강제 예외 발생

```python
try:
    a = input()
    if a == 'Kim':
        print('OK! Pass!')
    else:
        raise ValueError  # 강제로 예외 발생
except ValueError:
    print('Occured Exception!')
```

## 주요 내장함수

| 함수 | 설명 |
|------|------|
| `abs(x)` | 절댓값 반환 |
| `all(iterable)` | 모든 원소가 참이면 True |
| `any(iterable)` | 하나라도 참이면 True |
| `chr(n)` / `ord(c)` | 아스키코드 ↔ 문자 변환 |
| `enumerate(iterable)` | 인덱스와 원소를 함께 반환 |
| `filter(func, iterable)` | 조건에 맞는 값만 반환 |
| `len(x)` | 요소의 길이 반환 |
| `max()` / `min()` | 최대값 / 최소값 반환 |
| `map(func, iterable)` | 모든 원소에 함수 적용 후 반환 |
| `range(start, stop)` | 반복 가능한 객체 반환 |
| `type(x)` | 자료형 반환 |

## 주요 외장 모듈

| 모듈 | 기능 |
|------|------|
| `sys` | 시스템 접근 (`argv`, `exit`) |
| `pickle` | 파일 읽기/쓰기 (직렬화) |
| `os` | 시스템 명령 (`mkdir`, `rmdir`) |
| `time` | 시간 반환 (`time`, `localtime`, `ctime`) |
| `random` | 무작위 숫자 (`random`, `shuffle`, `choice`) |
| `webbrowser` | 웹 브라우저 실행 |
