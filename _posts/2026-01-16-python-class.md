---
layout: post
title: "Python 입문 - 클래스"
description: "클래스와 인스턴스, __init__, 메소드 호출 방법 정리"
date: 2026-01-16
category: "Python"
tags: python, class, oop
comments: true
---

## 클래스와 인스턴스

- **클래스** = 붕어빵 틀. 객체의 공통 속성, 개별 속성, 메소드를 정의한다.
- **인스턴스** = 붕어빵. 클래스를 통해 생성된 객체.
- **객체 vs 인스턴스**: 객체는 모든 변수를 의미, 인스턴스는 클래스를 통해 생성된 객체를 의미.

## 클래스 선언

```python
class Dog(object):

    species = 'dog'  # 모든 인스턴스의 공통 속성

    def __init__(self, name, age):  # 개별 인스턴스의 속성 정의
        self.name = name
        self.age = age

    def bark(self):
        print(f'{self.name} is barking!')
```

- `__init__` 메소드로 인스턴스의 개별 속성을 정의한다. 입력값에 `self`가 반드시 들어가야 한다.

## 인스턴스 생성과 사용

```python
a = Dog('micky', 2)  # Dog 클래스의 인스턴스 생성

a.__dict__  # {'name': 'micky', 'age': 2} - 개별 속성을 dict로 반환
```

## 메소드 호출 방법

```python
a.bark()       # 'micky is barking!' - 인스턴스.메소드()
Dog.bark(a)    # 동일한 결과 - 클래스.메소드(인스턴스)
```

메소드 선언 시 `self`를 넣지 않으면 개별 인스턴스를 입력으로 받지 않는다. 이 경우 `Dog.bark()`처럼 클래스에서 직접 호출해야 한다.
