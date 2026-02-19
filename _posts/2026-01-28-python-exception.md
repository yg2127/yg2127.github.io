---
layout: post
title: "Python 입문 - 예외처리"
description: "주요 에러 종류, try-except-else, raise"
date: 2026-01-28
category: "Python"
subcategory: "입문"
tags: python, exception, try
comments: true
---

## 주요 에러 종류

- **SyntaxError**: 코드 문법 오류
- **NameError**: 없는 변수를 접근할 때
- **ZeroDivisionError**: 수식에 수학적 오류가 있을 때 (0으로 나누기)
- **IndexError**: 리스트에서 없는 인덱스 접근
- **KeyError**: 딕셔너리에서 없는 키 접근
- **AttributeError**: 모듈, 클래스에 있는 잘못된 속성 사용
- **ValueError**: 리스트, 튜플에 없는 원소를 제거하려고 할 때
- **FileNotFoundError**: 없는 파일을 불러오려고 할 때
- **TypeError**: 자료형이 맞지 않는 연산을 할 때

## try-except-else

- **try**: 코드를 실행하는 블록. 해당 블록에서 에러가 나면 except 블록으로 넘어간다
- **except**: try 블록에서 오류가 났을 때 실행되는 블록. Error 내용에 따라 여러 블록을 설정할 수 있다
- **else**: try가 정상적으로 실행됐을 때 이어서 실행되는 블록이다

```python
name = ['Kim', 'Lee', 'Park']

try:
    z = 'Cho'
    x = name.index(z)
    print(f'{z} found it! {x+1} in name')
except:
    print(f'not found it! - Occured ValueError!')
else:
    print('Ok! else,')  # try가 정상적으로 실행될 때 실행된다
```

## raise

```python
try:
    a = input()
    if a == 'Kim':
        print('OK! Pass!')
    else:
        raise ValueError  # 에러가 날 부분이 없어도 특정 조건에서 에러가 나게 설정할 수 있다
except ValueError:
    print("Occured Exception!")
else:
    print('Ok else!')
```
