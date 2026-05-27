---
layout: post
title: "Python 입문 - 함수"
description: "return이랑 print 차이를 몰랐던 시절 — 함수의 진짜 역할 이해하기"
date: 2026-01-24
category: "Python"
subcategory: "입문"
tags: python, function, lambda
comments: true
---

## 들어가며

함수는 코드를 재사용하기 위한 블록인데, 처음에 **return과 print의 차이**를 몰라서 한참 헤맸다. "함수 안에서 print 하면 되는 거 아냐?" 싶었는데, return이 있어야 값을 다른 곳에서 쓸 수 있다는 걸 깨닫기까지 시간이 걸렸다.

---

## 핵심 개념

### 함수 선언

```python
def f1(w):
    print(f'hello {w}')
    return f'hello {w}'
```

함수를 선언하고, 호출하고, return으로 값을 반환한다.

### 다중 반환

```python
def f2(x):
    y1 = x * 10
    y2 = x * 20
    y3 = x * 30
    return y1, y2, y3

x, y, z = f2(20)
list1 = list(f2(20))
```

다중반환 함수는 여러 값을 **tuple 형식**으로 반환하므로, 언패킹이나 형변환을 해서 사용한다.

### *args (가변 인자)

```python
def args_f(*a):
    for i, v in enumerate(a):
        print(f'result : {i} {v}')
    print('------')
```

`*args`로 **튜플** 형태의 가변 인자를 받는다. 인자 개수가 몇 개든 상관없이 모두 받을 수 있다.

### **kwargs (키워드 인자)

```python
def kwargs_f(**x):
    for v in x.keys():
        print(f'{v} {x[v]}')
    print('-------')
```

`**kwargs`로 **딕셔너리** 형태의 가변 키워드 인자를 받는다. `kwargs_f(name='kim', age=25)` 이런 식으로 호출한다.

### lambda

lambda를 사용하면 한 줄짜리 간단한 함수를 그 자리에서 만들 수 있다. `lambda x: x * 2` 이런 식으로.

---

## 헷갈리기 쉬운 포인트

### return vs print — 근본적인 차이

```python
def add_print(a, b):
    print(a + b)      # 화면에 출력만 한다

def add_return(a, b):
    return a + b       # 값을 돌려준다

result = add_print(3, 4)   # 7이 출력되지만 result는 None!
result = add_return(3, 4)  # 출력 없지만 result는 7
```

`print()`는 화면에 보여주기만 하고, `return`은 값을 돌려준다. 함수의 결과를 다른 곳에서 쓰려면 반드시 return이 필요하다.

### 기본값 인자의 함정

```python
def append_to(element, target=[]):  # 이러면 안 된다!
    target.append(element)
    return target
```

기본값으로 빈 리스트를 주면, 호출할 때마다 같은 리스트에 쌓인다. 함수가 처음 정의될 때 기본값 객체가 한 번만 생성되기 때문이다. `target=None`으로 설정하고 함수 안에서 새 리스트를 만들어야 한다.

### *args와 **kwargs 순서

함수 인자를 정의할 때 순서가 있다: **일반 인자 → *args → 기본값 인자 → **kwargs**. 이 순서를 지키지 않으면 문법 에러가 난다.

---

## 정리하며

1. `return`은 값을 돌려주고, `print`는 화면에 출력만 한다 — 둘은 완전히 다르다
2. 다중 반환은 tuple로 나온다 — 언패킹으로 받자
3. `*args`는 튜플, `**kwargs`는 딕셔너리로 가변 인자를 받는다
4. 기본값에 mutable 객체(리스트 등)를 쓰면 안 된다
