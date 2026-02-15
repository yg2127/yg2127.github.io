---
layout: post
title: "Python 입문 - 모듈과 패키지"
description: "모듈 import, sys.path, __name__, 패키지 구조 정리"
date: 2026-01-17
category: "Python"
tags: python, module, package
comments: true
---

## 모듈이란?

정의해놓은 함수를 사용할 수 있는 파이썬 파일(`.py`)이다.

## 모듈 불러오기

파이썬 기본 라이브러리(sys, time, math 등)의 모듈 경로는 `sys.path` 리스트에 저장되어 있다.

```python
import sys

sys.path  # 모듈을 불러올 수 있는 경로 리스트
sys.path.append('/Users/yugeon/Desktop/test_module')  # 경로 추가

import test1  # test_module 폴더의 test1.py를 불러옴
```

## \_\_name\_\_ == '\_\_main\_\_'

모듈을 import하면 해당 파일의 모든 실행 코드가 다시 실행된다. 이를 방지하기 위해 `__name__` 가드를 사용한다.

```python
if __name__ == '__main__':
    # import 시에는 실행되지 않고,
    # 직접 실행할 때만 실행되는 코드
    print(add(5, 5))
    print(subtract(15, 5))
```

## 패키지란?

모듈을 모아놓은 폴더가 패키지이다.

```python
import sys
sys.path.append('/path/to/sub')
# sub 폴더 안에 sub1, sub2 패키지(폴더)가 있고
# 각 패키지 안에 모듈 파일들이 있다.

from sub1 import module1
from sub2 import module2 as m2  # as로 약칭 지정

module1.mod1_test1()  # 모듈 안의 함수 호출
m2.mod2_test1()
```
