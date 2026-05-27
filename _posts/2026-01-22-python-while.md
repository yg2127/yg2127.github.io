---
layout: post
title: "Python 입문 - 반복문 (while)"
description: "무한루프에 빠진 적 있다면 — while문의 원리와 탈출 전략"
date: 2026-01-22
category: "Python"
subcategory: "입문"
tags: python, while, loop
comments: true
---

## 들어가며

while문은 "조건이 참인 동안 반복"하는 건데, for문과 달리 반복 횟수를 미리 모를 때 쓴다. 사용자 입력을 받거나, 조건이 충족될 때까지 계속 시도하는 로직에 적합하다. 다만 조건을 잘못 설정하면 무한루프에 빠지는 위험이 있다.

---

## 핵심 개념

조건문에 부합하는 동안 while 블록 안의 코드들을 실행한다.

```python
count = 0
while count < 5:
    print(f'count: {count}')
    count += 1
```

`count += 1`을 빼먹으면 count가 영원히 0이라서 무한루프에 빠진다.

---

## 헷갈리기 쉬운 포인트

### 무한루프 실수 — count 업데이트 빼먹기

while문에서 가장 흔한 실수가 **루프 변수를 업데이트하지 않는 것**이다. `count += 1` 같은 줄을 빼먹으면 조건이 영원히 참이라서 프로그램이 멈추지 않는다. Ctrl+C로 강제 종료해야 한다.

### while True — 의도적 무한루프

의도적으로 무한루프를 만들 때는 `while True:`를 쓰고 `break`로 탈출한다. 사용자 입력을 반복적으로 받을 때 자주 쓰는 패턴이다.

```python
while True:
    answer = input("계속할까요? (y/n): ")
    if answer == 'n':
        break
```

### for vs while — 언제 뭘 쓰나?

- 반복 횟수를 알 때 → **for** (리스트 순회, range 등)
- 조건 기반 반복 → **while** (사용자 입력 대기, 조건 충족까지 반복)
- 둘 다 되는 경우에는 보통 for가 더 깔끔하다

---

## 정리하며

1. while은 **조건이 참인 동안** 반복 — 변수 업데이트를 빼먹지 말자
2. 의도적 무한루프는 `while True` + `break` 패턴
3. 반복 횟수를 알면 for, 조건 기반이면 while
