---
layout: post
title: "Python 입문 - 패킹과 언패킹"
description: "*args, **kwargs가 뭔지 모르겠다면? 패킹과 언패킹 완전 정리"
date: 2026-01-19
category: "Python"
subcategory: "입문"
tags: python, packing, unpacking
comments: true
---

## 들어가며

패킹과 언패킹은 개념 자체는 단순한데, `*`이랑 `**`가 붙기 시작하면 갑자기 헷갈린다. 특히 함수 정의에서 `*args`, `**kwargs`를 처음 봤을 때 "이게 뭐야?" 싶었다. 근데 이걸 이해하면 Python 함수를 훨씬 유연하게 쓸 수 있다.

## 핵심 개념

### 패킹과 언패킹 기초

- `v = [1, 2, 3, 4]`일 때 이를 packing한다고 말한다

- `v1, v2, v3, v4 = v`일 때 v1, v2, v3, v4는 1, 2, 3, 4이다. 이를 unpacking이라고 하며 파이썬에서 되게 많이 쓰이므로 잘 알아놓자

- packing(`*args`)은 여러 개의 단일 객체를 iterable한 객체로 변환하는 것이다
- unpacking은 iterable한 객체를 여러 개의 단일 객체로 변환하는 것이다

## 헷갈리기 쉬운 포인트

### *args — 위치 인자를 몽땅 받아준다

함수를 만들 때 인자가 몇 개 들어올지 모를 때 `*args`를 쓴다:

```python
def add_all(*args):
    return sum(args)  # args는 튜플이다

add_all(1, 2, 3)      # 6
add_all(1, 2, 3, 4, 5) # 15
```

`*args`의 `args`는 그냥 관례적인 이름이다. `*numbers`라고 해도 되지만, Python 커뮤니티에서는 `*args`를 쓰는 게 표준이다.

### **kwargs — 키워드 인자를 몽땅 받아준다

`**kwargs`는 `key=value` 형태의 인자들을 딕셔너리로 받는다:

```python
def print_info(**kwargs):
    for key, value in kwargs.items():
        print(f"{key}: {value}")

print_info(name="Kim", age=25, city="Seoul")
# name: Kim
# age: 25
# city: Seoul
```

### 언패킹 실수 — 개수가 안 맞으면 에러다

`a, b, c = [1, 2, 3, 4]` 하면 ValueError가 발생한다. 왼쪽 변수 수와 오른쪽 원소 수가 맞아야 한다.

나머지를 몽땅 받고 싶으면 `*`를 쓴다:

```python
a, *b, c = [1, 2, 3, 4, 5]
# a = 1, b = [2, 3, 4], c = 5
```

이 문법을 **확장 언패킹(extended unpacking)**이라고 하는데, 첫 번째랑 마지막 원소만 따로 쓰고 나머지는 리스트로 받고 싶을 때 유용하다.

### 함수 호출할 때도 언패킹이 된다

리스트나 딕셔너리를 함수 인자로 풀어서 넘길 수 있다:

```python
def greet(name, age):
    print(f"{name}은 {age}살")

info = ["Kim", 25]
greet(*info)  # Kim은 25살 — 리스트를 풀어서 위치 인자로 전달

info_dict = {"name": "Kim", "age": 25}
greet(**info_dict)  # Kim은 25살 — 딕셔너리를 풀어서 키워드 인자로 전달
```

## 정리하며

1. 패킹 = 여러 값을 하나로 묶기, 언패킹 = 묶인 걸 풀기
2. `*args`는 위치 인자를 튜플로, `**kwargs`는 키워드 인자를 딕셔너리로 받는다
3. `*`를 쓰면 나머지 원소를 리스트로 받을 수 있다
4. 함수 호출 시 `*list`, `**dict`로 인자를 풀어서 넘길 수 있다
