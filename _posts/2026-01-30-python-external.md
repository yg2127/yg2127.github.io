---
layout: post
title: "Python 입문 - 외장 함수"
description: "sys, os, time, random — Python의 표준 라이브러리 핵심 모듈 정리"
date: 2026-01-30
category: "Python"
subcategory: "입문"
tags: python, external, module
comments: true
---

## 들어가며

Python의 강점 중 하나가 **풍부한 표준 라이브러리**다. import만 하면 파일 처리, 시간 계산, 랜덤 생성 등 다양한 기능을 바로 쓸 수 있다. 여기서는 자주 쓰이는 외장 모듈들을 정리한다.

---

## 핵심 개념

- **sys**: 시스템 접근 — `sys.argv`(명령줄 인자), `sys.exit()`(프로그램 종료)
- **pickle**: 파이썬 객체를 파일로 저장/불러오기
- **open**: 파일 읽기/쓰기
- **os**: 시스템 명령 — `os.mkdir()`(폴더 생성), `os.rmdir()`(폴더 삭제)
- **time**: 시간 관련 — `time.time()`, `time.localtime()`, `time.ctime()`
- **random**: 무작위 숫자 — `random.random()`, `random.randint()`
- **shuffle**: iterable 객체를 랜덤으로 섞기
- **choice**: iterable 객체에서 하나를 랜덤 선택
- **webbrowser**: 웹 브라우저 실행 — `webbrowser.open('url')`

---

## 헷갈리기 쉬운 포인트

### pip install vs import

표준 라이브러리(sys, os, time 등)는 설치 없이 바로 import 가능하다. 하지만 `numpy`, `pandas` 같은 **외부 패키지**는 `pip install numpy`로 먼저 설치해야 한다. 이 차이를 모르면 `ModuleNotFoundError`를 만난다.

### 가상환경은 왜 필요한지

프로젝트마다 필요한 패키지 버전이 다를 수 있다. A 프로젝트는 pandas 1.x, B 프로젝트는 pandas 2.x가 필요하면 충돌이 난다. **가상환경**(venv)으로 프로젝트별 독립된 환경을 만들면 이 문제가 해결된다.

```bash
python -m venv myenv        # 가상환경 생성
source myenv/bin/activate   # 활성화 (Mac/Linux)
pip install pandas          # 이 환경에서만 설치됨
```

### pickle 사용 시 보안 주의

pickle은 편하지만, **신뢰할 수 없는 소스의 pickle 파일을 로드하면 안 된다.** 임의 코드가 실행될 수 있는 보안 취약점이 있다.

---

## 정리하며

1. 표준 라이브러리는 설치 불필요, 외부 패키지는 `pip install` 필요
2. 프로젝트별 **가상환경**을 만드는 습관을 들이자
3. `os`, `sys`, `time`, `random`은 가장 많이 쓰는 표준 모듈이다
