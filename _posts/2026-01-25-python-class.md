---
layout: post
title: "Python 입문 - 클래스"
description: "클래스와 인스턴스, __init__, 공통 속성과 개별 속성, 메서드"
date: 2026-01-25
category: "Python"
subcategory: "입문"
tags: python, class, oop
comments: true
---

## 클래스와 인스턴스

- 클래스 = 붕어빵을 찍어내는 기계. 객체의 공통 속성, 개별 속성을 정의하고 method를 정의한다
- 인스턴스 = 붕어빵. 코드에서 사용할 변수(객체)를 클래스를 통해 선언한 것이다
- 객체는 모든 변수를 의미하고, 인스턴스는 클래스를 통해 생성된 객체를 의미한다

```python
class Dog(object):

    species = 'dog'  # 모든 인스턴스의 공통 속성

    def __init__(self, name, age):  # 개별 인스턴스의 속성 정의
        self.name = name
        self.age = age

    def bark(self):
        print(f'{self.name} is barking!')
```

## __init__ 메서드

- `__init__` 메서드를 통해 인스턴스의 개별 속성을 정의한다. 입력값으로 `self`가 반드시 들어가야 한다

```python
a = Dog('micky', 2)  # Dog 클래스의 인스턴스 a를 생성
```

## 인스턴스 활용

- `instance.__dict__`를 통해서 해당 인스턴스의 개별 속성을 dict 형식으로 반환받을 수 있다
- 생성된 인스턴스가 클래스 method를 실행하려면 아래와 같이 작성한다

```python
a.bark()       # 'micky is barking!' 출력
Dog.bark(a)    # 클래스.메서드(인스턴스) 형식으로도 똑같이 실행된다
```

- 메서드를 선언할 때 입력값에 `(self)`를 안 넣어주면 그 메서드는 개별 인스턴스를 인풋으로 받지 않는다. 따라서 `Dog.bark()` 이런 식으로 호출해야 한다
