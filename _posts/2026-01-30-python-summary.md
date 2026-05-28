---
layout: post
title: "Python 입문 총정리 - 코드와 정의 치트시트"
description: "print부터 외장 함수까지, 파이썬 입문 핵심을 코드와 정의로 한 번에 복습"
date: 2026-01-30
category: "Python"
subcategory: "입문"
tags: python, cheatsheet, summary
comments: true
---

파이썬 입문 시리즈 전체를 코드와 정의 위주로 압축한 정리. 빠르게 보고 기억을 되살리는 용도.

---

## 1. print 출력

```python
print("a", "b", sep="-")   # a-b   (값 사이 구분자)
print("a", end="")          # 끝 줄바꿈 제거
print(f"{name} {ver}")      # f-string (권장)
```

- 포맷팅 우선순위: **f-string** > `"{}".format()` > `"%s" %`(레거시)

---

## 2. 변수

- Python 변수 = **이름표(참조)**, 값을 담는 상자가 아님
- `b = a` → 같은 객체 참조 (`id(a) == id(b)`) / 복사는 `a[:]`, `a.copy()`
- **immutable**: `int`, `float`, `str`, `tuple`
- **mutable**: `list`, `dict`, `set`
- 네이밍 표준: 변수·함수 `snake_case`, 클래스 `PascalCase`

---

## 3. 숫자 자료형

| 연산 | 기호 | 예시 |
|---|---|---|
| 나눗셈(float) | `/` | `10 / 2` → `5.0` |
| 몫 | `//` | `10 // 3` → `3` |
| 나머지 | `%` | `10 % 3` → `1` |
| 제곱 | `**` | `2 ** 3` → `8` |
| 절댓값 | `abs()` | `abs(-3)` → `3` |

- `int(3.9)` → `3` (반올림 X, 버림 / 반올림은 `round()`)
- `int('3.5')` → 에러 → `int(float('3.5'))`
- `0.1 + 0.2 != 0.3` (부동소수점) → 비교는 `round()` / `math.isclose()`

---

## 4. 문자열 (immutable)

```python
a = 'abcdef'
len(a)                 # 길이
a[::-1]                # 역순
a[1:3]                 # 'bc' (이상:미만)
a.upper(); a.lower()
a.isupper(); a.islower()
'Hi World'.replace('World', 'Python')
```

- 수정 불가 → 새로 만들기: `s = 'h' + s[1:]`
- 슬라이싱 `[이상:미만:간격]`, 끝 인덱스 미포함
- 이스케이프: `\n`(개행), `\t`(탭)

---

## 5. 리스트 (mutable)

```python
a.append(x)        # 원소 1개 추가
a.extend([..])     # 이어붙이기 (= +)
a.insert(i, x)     # i 위치에 삽입
a.remove(x)        # 값 x 제거
a.pop()            # 마지막 제거+반환  /  a.pop(0) 맨 앞
del a[i]; a[1:3] = []
a.sort(); a.reverse(); a.index(x)
```

- `append([4,5])` → `[..,[4,5]]` vs `extend([4,5])` → `[..,4,5]`
- 복사: `b=a`(참조) / `a[:]`,`a.copy()`(얕은) / `copy.deepcopy(a)`(깊은)
- 2차원: `[[0]*3 for _ in range(3)]`  (✗ `[[0]*3]*3` — 같은 행 참조)

---

## 6. 튜플 (immutable)

```python
a = (1, 2, 3)
a, b = b, a        # 변수 swap
one = (1,)         # 원소 1개는 콤마 필수 ( (1)은 정수 )
```

- 딕셔너리 key로 사용 가능 (list는 불가)
- 함수의 다중 반환값 = 튜플

---

## 7. 집합 set (순서 X, 중복 X)

```python
s = {1, 2, 3}
e = set()          # 빈 set은 set()  ( {}는 빈 dict )
a & b              # 교집합
a | b              # 합집합
a - b              # 차집합
list(set(lst))     # 중복 제거 (순서 보장 X)
```

- 메서드: `add()`, `remove()`(없으면 KeyError), `discard()`(안전), `clear()`, `issubset()`, `isdisjoint()`
- 인덱싱·슬라이싱 불가

---

## 8. 딕셔너리 (key-value, key 중복 X)

```python
d = {'name': 'Kim', 'age': 25}
d['name']                  # 없으면 KeyError
d.get('key')               # 없으면 None
d.get('key', 기본값)
d.keys(); d.values(); d.items()
d.pop(key); d.update({..})
{x: x**2 for x in range(1, 6)}     # comprehension
```

- key는 immutable만 가능 (`str`, `int`, `float`, `tuple`)

---

## 9. 패킹 / 언패킹

```python
a, b, c = (1, 2, 3)          # 언패킹
a, *b, c = [1, 2, 3, 4, 5]   # b=[2,3,4] (확장 언패킹)

def f(*args): ...            # 위치 인자 → 튜플
def f(**kwargs): ...         # 키워드 인자 → 딕셔너리

f(*lst)                      # 호출 시 풀어서 전달
f(**dct)
```

---

## 10. 조건문 (if)

```python
if x: ...
elif y: ...
else: ...

x and y / x or y / not x
v in lst / v not in lst
status = "성인" if age >= 18 else "미성년"   # 삼항
```

- **Falsy**: `False`, `None`, `0`, `0.0`, `""`, `[]`, `()`, `{}`, `set()` → 나머지는 전부 Truthy
- `if my_list:` 권장 (`len(my_list) > 0` 대신)
- `elif`는 **좁은 범위부터** (위→아래 순차 검사, 하나 참이면 종료)
- 단축 평가: `and`(앞 False면 중단), `or`(앞 True면 중단)

---

## 11. 반복문 (for)

```python
for x in iterable: ...
for i, v in enumerate(lst): ...    # 인덱스+값
break       # 즉시 탈출
continue    # 현재 반복만 건너뜀
```

- `range(start, stop)` → **stop 미포함** (`range(5)` → 0~4)

---

## 12. 반복문 (while)

```python
while 조건:
    ...
    count += 1        # 업데이트 빼먹으면 무한루프

while True:
    if ...: break     # 의도적 무한루프 탈출
```

- 횟수 알면 `for`, 조건 기반이면 `while`

---

## 13. 입력 (input) — 항상 str 반환

```python
n = input("메시지")                  # str
n = int(input())                     # 형변환 필수
x, y, z = map(int, input().split())  # 공백 구분 다중 입력
```

---

## 14. 함수

```python
def f(x):
    return x          # 값 반환 (print는 화면 출력만)

def f(x):
    return y1, y2     # 다중 반환 = 튜플

square = lambda x: x * 2     # 한 줄 함수
```

- 인자 순서: 일반 → `*args` → 기본값 → `**kwargs`
- 기본값에 mutable(`target=[]`) 금지 → `None`으로 두고 내부에서 생성

---

## 15. 클래스

```python
class Dog:
    species = 'dog'               # 클래스 변수 (모든 인스턴스 공유)

    def __init__(self, name):     # 초기화 메서드
        self.name = name          # 인스턴스 변수 (개별)

    def bark(self):
        print(f'{self.name}!')

a = Dog('micky')
a.bark()            # == Dog.bark(a)
a.__dict__          # 인스턴스 속성 dict
```

- `self` = 메서드를 호출한 인스턴스 자기 자신
- 클래스 변수=공유 / 인스턴스 변수(`self.`)=개별

---

## 16. 모듈

```python
import math               # math.sqrt(4)
from math import sqrt     # sqrt(4)
import numpy as np        # 약칭
sys.path.append('경로')   # import 경로 추가

if __name__ == '__main__':   # 직접 실행 시에만 동작
    ...
```

- `from module import *` 비추천 (이름 충돌 위험)

---

## 17. 패키지 (모듈을 모은 폴더)

```python
from sub1 import module1
from sub2 import module2 as m2
from . import helper          # 상대 import (같은 폴더)
from ..utils import helper    # 상위 폴더
```

- `__init__.py`: 패키지 초기화 (Python 3.3+ 선택, 관리용으로 유용)

---

## 18. 예외처리

```python
try:
    ...
except ValueError:        # 구체적 타입 명시 (bare except 금지)
    ...
except (ValueError, KeyError):   # 여러 개
    ...
else:                     # try 정상 실행 시
    ...
finally:                  # 에러 여부와 무관하게 무조건 실행
    ...

raise ValueError          # 의도적 발생
```

| 에러 | 발생 상황 |
|---|---|
| `NameError` | 없는 변수 접근 |
| `IndexError` | 없는 인덱스 접근 |
| `KeyError` | 없는 딕셔너리 키 접근 |
| `ValueError` | 잘못된 값 |
| `TypeError` | 자료형 불일치 연산 |
| `ZeroDivisionError` | 0으로 나누기 |
| `FileNotFoundError` | 없는 파일 접근 |

- 예외처리는 **예측 불가능한 외부 요인**(입력·파일·네트워크·파싱)에만

---

## 19. 내장 함수 (import 불필요)

| 함수 | 기능 |
|---|---|
| `abs` | 절댓값 |
| `all` / `any` | 모두 참 / 하나라도 참 |
| `chr` / `ord` | 아스키 ↔ 문자 |
| `enumerate` | 인덱스+원소 |
| `map` / `filter` | 함수 적용 / 조건 필터 |
| `len`, `max`, `min` | 길이, 최대, 최소 |
| `range`, `type` | 숫자 시퀀스, 자료형 |

- `map`, `filter` → **iterator** 반환 → `list()`로 감싸기
- `all([])` → `True`, `any([])` → `False`
- 단순 변환은 list comprehension이 더 Pythonic

---

## 20. 외장 함수 (표준 라이브러리)

| 모듈 | 용도 |
|---|---|
| `sys` | `sys.argv`, `sys.exit()` |
| `os` | `os.mkdir()`, `os.rmdir()` |
| `time` | `time.time()`, `time.localtime()` |
| `random` | `randint()`, `choice()`, `shuffle()` |
| `pickle` | 객체 저장/로드 (신뢰 소스만 — 보안 주의) |
| `open` | 파일 읽기/쓰기 |

- 표준 라이브러리는 설치 불필요 / 외부 패키지(`numpy`, `pandas`)는 `pip install`
- 가상환경: `python -m venv myenv` → `source myenv/bin/activate`
