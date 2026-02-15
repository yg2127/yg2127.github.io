---
layout: post
title: "Python 입문 - 제어문"
description: "if 조건문, for 반복문, while 반복문 정리"
date: 2026-01-14
category: "Python"
tags: python, if, for, while
comments: true
---

## if 조건문

```python
if 조건1:
    실행문
elif 조건2:
    실행문
else:
    실행문
```

### 논리 연산자

- `and`, `or`, `not`

### in / not in

리스트, 튜플 등에 원소가 포함되어 있는지 확인할 때 매우 유용하다.

```python
a = [1, 2, 3, 4, 5]
b = 7

if b not in a:
    print(f'{b} is not in a ({a})')
else:
    print(f'{b} is in a ({a})')
```

## for 반복문

### range 활용

```python
for i in range(1, 11):
    print(i)  # 1부터 10까지 출력
```

### 리스트 순회

`in` 뒤에 리스트가 올 수 있고, 원소의 타입도 자유롭다.

```python
a = ['kim', 'lee', 'park', 'na']

for x in a:
    print(f'first name is {x}')
```

### break / continue

- `break`: 반복문 즉시 종료
- `continue`: 현재 반복을 건너뛰고 다음 반복으로

## while 반복문

조건이 참인 동안 블록 안의 코드를 반복 실행한다.

```python
count = 0
while count < 5:
    print(count)
    count += 1
```
