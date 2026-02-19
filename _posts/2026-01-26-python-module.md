---
layout: post
title: "Python 입문 - 모듈"
description: "모듈 개념, sys.path, import, __name__ == '__main__'"
date: 2026-01-26
category: "Python"
subcategory: "입문"
tags: python, module, import
comments: true
---

## 모듈이란

- module = 정의해놓은 함수를 사용할 수 있는 파이썬 파일이다
- 파이썬 기본 라이브러리인 내장 라이브러리에는 sys, time, math 같은 라이브러리 모듈들이 있고, 이 모듈들을 불러올 수 있는 파일 경로는 `sys.path` 리스트 안에 저장되어 있다

```python
sys.path  # list 형태. append 메서드로 파일 불러오는 경로를 추가할 수 있다
sys.path.append('/Users/yugeon/Desktop/test_module')

import test1  # test_module 폴더 안의 test1.py를 불러와서 함수들을 사용할 수 있다
```

## __name__ == '__main__'

- import하면 파이썬에서 실행한 함수들(print문부터 기타 함수들)을 전부 다시 실행하게 된다. 따라서 import하는 파일 내부에서 main 블록 안에 실행할 함수들을 넣어줘야 한다

```python
if __name__ == '__main__':  # 이걸 사용해서 import해도 실행되지 않게 한다
    print('-' * 10)
    print('called inner!!')
    print(add(5, 5))
    print(subtract(15, 5))
```
