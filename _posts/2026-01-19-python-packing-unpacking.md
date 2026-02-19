---
layout: post
title: "Python 입문 - 패킹과 언패킹"
description: "packing과 unpacking 개념, *args 활용"
date: 2026-01-19
category: "Python"
subcategory: "입문"
tags: python, packing, unpacking
comments: true
---

## 패킹과 언패킹

- `v = [1, 2, 3, 4]`일 때 이를 packing한다고 말한다

- `v1, v2, v3, v4 = v`일 때 v1, v2, v3, v4는 1, 2, 3, 4이다. 이를 unpacking이라고 하며 파이썬에서 되게 많이 쓰이므로 잘 알아놓자

- packing(`*args`)은 여러 개의 단일 객체를 iterable한 객체로 변환하는 것이다
- unpacking은 iterable한 객체를 여러 개의 단일 객체로 변환하는 것이다
