---
layout: post
title: "Python 입문 - 패키지"
description: "모듈을 폴더로 묶으면 패키지 — import 경로와 as 약칭 정리"
date: 2026-01-27
category: "Python"
subcategory: "입문"
tags: python, package
comments: true
---

## 들어가며

모듈이 하나의 .py 파일이라면, **패키지는 모듈을 모아놓은 폴더**다. 프로젝트가 커지면 모듈이 수십 개가 되는데, 이걸 폴더로 구조화하는 게 패키지다.

---

## 핵심 개념

```python
import sys
sys.path.append('/Users/yugeon/DEEPDIVE/inflearn_python1/source_code/sub')
# sub 폴더 안에 모듈을 모아놓은 폴더 sub1, sub2가 들어있다

from sub1 import module1
from sub2 import module2 as m2  # as ~~ 는 약칭

# import한 모듈 사용하기
module1.mod1_test1()  # import한 모듈 안의 함수를 사용한다
m2.mod2_test1()
```

---

## 헷갈리기 쉬운 포인트

### \_\_init\_\_.py의 역할

Python 3.3 이전에는 폴더를 패키지로 인식하려면 `__init__.py` 파일이 반드시 있어야 했다. 3.3부터는 없어도 되지만, 패키지 초기화 코드를 넣거나 `from package import *` 시 공개할 모듈을 지정하는 용도로 여전히 쓰인다.

### 상대 import vs 절대 import

```python
# 절대 import — 프로젝트 루트부터 전체 경로
from myproject.utils import helper

# 상대 import — 현재 위치 기준
from . import helper        # 같은 폴더
from ..utils import helper  # 상위 폴더의 utils
```

상대 import는 패키지 내부에서만 쓸 수 있고, 직접 실행하는 스크립트에서는 동작하지 않는다. 처음에 이걸 몰라서 "왜 되다가 안 되지?" 하는 경험을 했다.

### as 약칭은 언제 쓰나

이름이 긴 모듈을 반복해서 쓸 때 유용하다. `import numpy as np`, `import pandas as pd`가 대표적. 이건 Python 커뮤니티에서 사실상 표준처럼 쓰이는 약칭이라 외워두면 좋다.

---

## 정리하며

1. 패키지 = 모듈을 모아놓은 폴더 — 프로젝트 구조화의 기본
2. `as`로 약칭을 지정하면 긴 모듈명을 줄일 수 있다
3. `__init__.py`는 선택이지만 패키지 관리에 유용하다
