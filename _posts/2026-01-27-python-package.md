---
layout: post
title: "Python 입문 - 패키지"
description: "모듈을 모아놓은 폴더, 패키지 import와 as 약칭"
date: 2026-01-27
category: "Python"
subcategory: "입문"
tags: python, package
comments: true
---

## 패키지

- 모듈을 모아놓은 폴더가 패키지이다

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
