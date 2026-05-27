---
layout: post
title: "Python 입문 - print 출력"
description: "f-string이 짱인 건 알겠는데, sep이랑 end는 뭐야? print 옵션 총정리"
date: 2026-01-11
category: "Python"
subcategory: "입문"
tags: python, print, format
comments: true
---

## 들어가며

Python 배우면 제일 먼저 치는 게 `print("Hello World")`인데, 사실 print 함수에 생각보다 옵션이 많다. 처음엔 그냥 출력만 하면 되지 뭐가 이렇게 많아? 싶었는데, 나중에 데이터 포맷 맞춰서 출력해야 할 때 이걸 알아두면 꽤 편하다.

## 핵심 개념

### print의 숨은 옵션들

- `sep` 옵션으로 출력값 사이의 구분자를 지정할 수 있다
- `end` 옵션에 `end=''`을 주면 원래 print 내 문자열 맨 마지막에 있는 `\n` 개행문자를 제거한다
- `file` 옵션으로 파일에 출력할 수 있다
- `format` 함수를 사용해 포맷팅 출력이 가능하다

### 포맷팅 문법

- **string 형식**: 자리수 확보, 자리수 제한 등
- **digit 형식**: `%04d`하면 4자리만 출력된다
- **float 형식**: 자리수 확보, 제한
- **f-string 문법**: 가장 편리한 포맷팅 방법이다. 꼭 쓰자

```python
name = "Python"
version = 3.12
print(f"{name} {version}")  # Python 3.12
```

## 헷갈리기 쉬운 포인트

### f-string vs format() vs % — 뭘 써야 해?

Python에서 문자열 포맷팅 방법이 세 가지나 있어서 처음에 헷갈린다.

- **% 포맷팅**: `"이름: %s" % name` — C 스타일이라 레거시 코드에서 볼 수 있다. 요즘은 잘 안 쓴다
- **format() 메서드**: `"이름: {}".format(name)` — % 보다 유연하지만 좀 길다
- **f-string** (Python 3.6+): `f"이름: {name}"` — 가장 직관적이고 빠르다. **이거 쓰면 된다**

결론: 특별한 이유가 없으면 f-string 쓰자. 속도도 가장 빠르고 가독성도 좋다.

### sep과 end가 은근 유용한 순간

`sep`은 여러 값을 한 번에 출력할 때 구분자를 바꿔준다. CSV처럼 콤마로 구분하고 싶을 때 쓰면 된다. `end`는 print가 끝날 때 줄바꿈 대신 다른 걸 넣고 싶을 때 쓴다. 반복문에서 한 줄에 쭉 출력하고 싶으면 `end=' '` 하면 된다.

## 정리하며

print는 단순해 보이지만, 포맷팅까지 포함하면 의외로 알아야 할 게 많다. 핵심만 정리하면:

1. **f-string 쓰자** — 이것만 확실히 익히면 된다
2. `sep`, `end`는 출력 형태 바꿀 때 유용하다
3. `%` 포맷팅은 옛날 코드 읽을 때만 알면 된다
