---
layout: post
title: "Python 입문 - 반복문 (for)"
description: "for문, range 함수, break/continue 사용법"
date: 2026-01-21
category: "Python"
subcategory: "입문"
tags: python, for, loop
comments: true
---

## 반복문 (for)

- `for i in range(1, 11)` 이거 잘 사용하도록 하자
- `in` 다음에 range 함수 말고 리스트가 와도 된다. 리스트 내부의 변수 형식도 정수뿐만 아니라 텍스트까지 와도 된다

```python
a = ['kim', 'lee', 'park', 'na']

for x in a:
    print(f'first name is {x}')
```

- `break`문, `continue`문을 사용할 수 있다
