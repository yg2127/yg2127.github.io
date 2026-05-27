---
layout: post
title: "Python 입문 - 예외처리"
description: "에러가 났을 때 프로그램이 죽지 않게 하려면 — try/except의 올바른 사용법"
date: 2026-01-28
category: "Python"
subcategory: "입문"
tags: python, exception, try
comments: true
---

## 들어가며

코드를 짜다 보면 에러는 반드시 난다. 문제는 에러가 나면 프로그램이 바로 죽어버린다는 거다. 예외처리를 쓰면 에러가 나도 프로그램이 계속 돌아갈 수 있다. 처음에는 "그냥 에러 안 나게 짜면 되지 않나?"라고 생각했는데, 사용자 입력이나 파일 접근처럼 **예측 불가능한 상황**이 많다는 걸 깨달았다.

---

## 핵심 개념

### 주요 에러 종류

- **SyntaxError**: 코드 문법 오류
- **NameError**: 없는 변수를 접근할 때
- **ZeroDivisionError**: 0으로 나누기
- **IndexError**: 리스트에서 없는 인덱스 접근
- **KeyError**: 딕셔너리에서 없는 키 접근
- **AttributeError**: 잘못된 속성 사용
- **ValueError**: 없는 원소를 제거하려고 할 때
- **FileNotFoundError**: 없는 파일을 불러올 때
- **TypeError**: 자료형이 맞지 않는 연산

### try-except-else

- **try**: 실행할 코드 블록. 에러 발생 시 except로 넘어간다
- **except**: 에러가 났을 때 실행되는 블록. Error 종류별로 여러 개 설정 가능
- **else**: try가 정상 실행됐을 때 이어서 실행되는 블록

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

### raise — 의도적 에러 발생

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

---

## 헷갈리기 쉬운 포인트

### bare except의 위험

```python
except:  # 이렇게 하면 모든 에러를 잡아버린다 — 위험!
```

모든 에러를 잡아버리면 **진짜 버그도 숨겨버린다.** 디버깅이 불가능해진다. 항상 구체적인 에러 타입을 명시하자:

```python
except ValueError:     # ValueError만 잡기
except (ValueError, KeyError):  # 여러 에러 잡기
```

### try/except를 언제 쓰는 게 적절한지

모든 곳에 try/except를 넣으면 코드가 지저분해진다. **예측 불가능한 외부 요인**이 있을 때만 쓰자:
- 사용자 입력 (잘못된 값 입력 가능)
- 파일 접근 (파일이 없을 수 있음)
- 네트워크 요청 (연결 실패 가능)
- 외부 데이터 파싱 (형식이 다를 수 있음)

단순 로직 오류는 try/except로 감추지 말고 코드 자체를 고치자.

### finally — 무조건 실행

try/except에 `finally` 블록을 추가하면 에러 여부와 관계없이 무조건 실행된다. 파일 닫기, 연결 종료 같은 정리 작업에 쓴다.

---

## 정리하며

1. **try/except**로 에러 발생 시 프로그램이 죽지 않게 할 수 있다
2. **bare except** 쓰지 말자 — 구체적인 에러 타입을 명시
3. 예외처리는 **예측 불가능한 외부 요인**에만 사용하자
4. `raise`로 의도적으로 에러를 발생시킬 수 있다
