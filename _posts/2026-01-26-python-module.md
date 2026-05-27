---
layout: post
title: "Python 입문 - 모듈"
description: "__name__ == '__main__'이 뭔지 처음에 진짜 몰랐다 — 모듈과 import의 동작 원리"
date: 2026-01-26
category: "Python"
subcategory: "입문"
tags: python, module, import
comments: true
---

## 들어가며

코드가 길어지면 하나의 파일에 다 넣을 수 없다. 기능별로 파일을 나누고 필요할 때 불러와서 쓰는 게 **모듈**이다. 개념은 간단한데, `import`가 정확히 뭘 하는 건지, `__name__ == '__main__'`은 왜 필요한지가 처음에 좀 헷갈렸다.

---

## 핵심 개념

### 모듈이란

module = 정의해놓은 함수를 사용할 수 있는 파이썬 파일이다. 파이썬 기본 라이브러리인 내장 라이브러리에는 sys, time, math 같은 모듈들이 있고, 이 모듈들의 파일 경로는 `sys.path` 리스트에 저장되어 있다.

```python
sys.path  # list 형태. append 메서드로 파일 불러오는 경로를 추가할 수 있다
sys.path.append('/Users/yugeon/Desktop/test_module')

import test1  # test_module 폴더 안의 test1.py를 불러와서 함수들을 사용할 수 있다
```

### \_\_name\_\_ == '\_\_main\_\_'

import하면 파이썬에서 그 파일의 실행 코드(print문 등)를 **전부 다시 실행**하게 된다. 이걸 방지하려면 main 블록 안에 실행할 코드를 넣어야 한다.

```python
if __name__ == '__main__':  # 이걸 사용해서 import해도 실행되지 않게 한다
    print('-' * 10)
    print('called inner!!')
    print(add(5, 5))
    print(subtract(15, 5))
```

---

## 헷갈리기 쉬운 포인트

### import의 여러 방식

```python
import math            # math.sqrt(4) 형태로 사용
from math import sqrt  # sqrt(4)로 바로 사용 가능
from math import *     # math의 모든 것을 가져옴 (비추천 — 이름 충돌 위험)
import math as m       # m.sqrt(4) 형태로 약칭 사용
```

`from module import *`는 편하지만, 어디서 온 함수인지 알 수 없어서 코드 가독성이 떨어진다. 팀 프로젝트에서는 쓰지 않는 게 좋다.

### \_\_name\_\_이 뭔데?

모든 Python 파일에는 `__name__`이라는 특수 변수가 있다. 직접 실행하면 `'__main__'`이 되고, import되면 **파일 이름**이 된다. 이 차이를 이용해서 "직접 실행할 때만 동작하는 코드"를 분리하는 거다.

### sys.path에 없으면 import 에러

`ModuleNotFoundError`가 나면 십중팔구 sys.path에 해당 모듈의 경로가 없는 거다. `sys.path.append()`로 경로를 추가하거나, 프로젝트 구조를 정리해야 한다.

---

## 정리하며

1. 모듈 = 함수가 정의된 .py 파일 — `import`로 불러온다
2. `__name__ == '__main__'`은 **직접 실행 시에만** 코드가 동작하게 하는 가드
3. `from module import *`는 편하지만 이름 충돌 위험 — 명시적 import를 쓰자
