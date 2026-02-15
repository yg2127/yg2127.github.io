---
layout: post
title: "Python 입문 - 함수"
description: "함수 정의, 다중 반환, *args, **kwargs, lambda, input 정리"
date: 2026-01-15
category: "Python"
tags: python, function, lambda
comments: true
---

## 함수 선언과 반환

```python
def f1(w):
    print(f'hello {w}')
    return f'hello {w}'
```

### 다중 반환

다중 반환 함수는 값들을 **tuple** 형식으로 반환하므로 unpacking을 활용한다.

```python
def f2(x):
    y1 = x * 10
    y2 = x * 20
    y3 = x * 30
    return y1, y2, y3

x, y, z = f2(20)          # unpacking
list1 = list(f2(20))       # 리스트로 형변환
```

## *args (가변 인자)

튜플 형식으로 몇 개를 입력하든 모두 받을 수 있다.

```python
def args_f(*a):
    for i, v in enumerate(a):
        print(f'result : {i} {v}')
```

## **kwargs (키워드 가변 인자)

딕셔너리 형식으로 키-값 쌍을 몇 개든 받을 수 있다.

```python
def kwargs_f(**x):
    for v in x.keys():
        print(f'{v} {x[v]}')
```

## lambda

간단한 함수를 한 줄로 정의하여 바로 사용할 수 있다.

```python
square = lambda x: x ** 2
print(square(5))  # 25
```

## input 입력

- 기본 입력 형식은 **string**
- `int(input())`으로 정수 변환

### map을 활용한 다중 입력

```python
x, y, z = map(int, input().split())
# "3 4 45" 입력 → x=3, y=4, z=45
```

이 패턴은 코딩테스트에서 매우 자주 사용된다.
