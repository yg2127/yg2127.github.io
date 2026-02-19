---
layout: post
title: "Python 입문 - 함수"
description: "함수 선언, 다중반환, *args, **kwargs, lambda"
date: 2026-01-24
category: "Python"
subcategory: "입문"
tags: python, function, lambda
comments: true
---

## 함수 선언

```python
def f1(w):
    print(f'hello {w}')
    return f'hello {w}'
```

- 함수를 선언하고 사용하고 반환값 return을 다룬다

## 다중 반환

```python
def f2(x):
    y1 = x * 10
    y2 = x * 20
    y3 = x * 30
    return y1, y2, y3

x, y, z = f2(20)
list1 = list(f2(20))
```

- 다중반환 함수는 여러 값들을 tuple 형식으로 반환하므로 형변환과 unpacking을 사용해야 한다

## *args (가변 인자)

```python
def args_f(*a):
    for i, v in enumerate(a):
        print(f'result : {i} {v}')
    print('------')
```

- `*args`를 사용해서 함수 입력값에 튜플을 입력받는데, 튜플 길이가 몇이든 상관없이 모두 받을 수 있다

## **kwargs (키워드 인자)

```python
def kwargs_f(**x):
    for v in x.keys():
        print(f'{v} {x[v]}')
    print('-------')
```

- `**kwargs`를 사용해서 함수 입력값에 딕셔너리를 입력받는데, 길이가 몇이든 상관없이 모두 받을 수 있다

## lambda

- lambda를 사용해서 곧바로 메모리에 올리고 반환값을 받을 수 있다
