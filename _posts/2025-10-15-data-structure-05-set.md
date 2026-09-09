---
layout: post
title: "집합의 중복은 코드가 막아야 한다"
description: "연결리스트는 저절로 집합이 되지 않는다. 삽입 규칙과 정렬 전제를 확인하며 합집합과 교집합을 구현한다."
date: 2025-10-15
category: "Data Structure"
tags: [data-structure, c, set, linked-list]
series: data-structure
series_title: "자료구조 학습 기록"
series_order: 5
permalink: /2026/09/data-structure-05-set.html
comments: false
---

## 한 줄뿐인 노트에서 코드를 읽기

`집합 ADT` 노트에 남은 본문은 “집합ADT 구현!” 한 줄뿐이다. 당시 무엇이 어렵거나 어떤 오류를 고쳤는지는 이 문장만으로 알 수 없다. 그래서 저장소 구현과 다른 노트의 표현을 이번에 다시 대조하며 정리했다.

`배열과 연결리스트` 노트에는 연결리스트의 장점으로 “중복이 없음”이 적혀 있다. 이 부분은 바로잡아야 한다. 연결리스트 노드는 같은 값을 몇 번이든 담을 수 있다. 중복을 허용하지 않는 것은 연결리스트의 성질이 아니라 Set ADT가 요구하는 규칙이며, `add`가 그 규칙을 구현해야 한다.

[부분집합 코드의 `add`](https://github.com/yg2127/25-1_data_structure/blob/eab0311e196a27d98ca6aa493290c9078f206dbb/%EC%A4%91%EA%B0%84%EB%B2%94%EC%9C%84/7-1-2.c#L17-L32)는 먼저 `contain`으로 값을 찾는다.

```c
if (contain(set, data)) return set;
newnode->data = data;
newnode->next = set;
return newnode;
```

같은 값이 있으면 노드를 만들지 않는다. 이 한 줄이 빠지면 구조는 여전히 연결리스트지만 더는 집합의 표현이라고 보기 어렵다.

## 합집합과 교집합이 기대는 정렬

두 집합이 오름차순이면 현재 원소만 비교하며 함께 전진할 수 있다.

| A 현재값과 B 현재값 | 합집합 | 교집합 |
|---|---|---|
| A가 작음 | A를 넣고 A 이동 | A만 이동 |
| B가 작음 | B를 넣고 B 이동 | B만 이동 |
| 같음 | 한 번 넣고 둘 다 이동 | 넣고 둘 다 이동 |

이 방식은 [합집합·교집합 구현](https://github.com/yg2127/25-1_data_structure/blob/eab0311e196a27d98ca6aa493290c9078f206dbb/%EC%A4%91%EA%B0%84%EB%B2%94%EC%9C%84/7-2.c#L64-L129)처럼 각 리스트를 한 번씩 훑어 $O(n+m)$에 끝낼 수 있다. 단, 입력이 정렬되어 있다는 전제가 깨지면 “작은 쪽은 앞으로도 상대 리스트에 없다”는 판단도 깨진다.

이번에 만든 [집합 검증 코드](/assets/code/data-structure-study/05-set-merge.c)는 입력 리스트를 만들 때 정렬 위치를 찾고 같은 값이면 건너뛴다. 합집합과 교집합을 만들 때는 결과 tail에 상수 시간으로 붙이므로 두 입력을 훑는 데 $O(n+m)$이 든다. `A={3,1,3,2}`, `B={4,3,2}`를 넣어 실행한 결과다.

```text
A: 1 2 3
union: 1 2 3 4
intersection: 2 3
empty intersection: empty
```

입력 A의 3은 두 번 주어졌지만 한 노드만 남았다. 이는 연결리스트가 중복을 없앤 결과가 아니라 `add_sorted`가 기존 3을 발견해 삽입하지 않은 결과다. 전체 원소를 `assert`로 비교했고 한쪽이 공집합인 교집합도 빈 결과인지 확인했다.

## 더미 노드의 값은 읽지 않는다

저장소의 다른 버전인 [`7-2-2.c`](https://github.com/yg2127/25-1_data_structure/blob/eab0311e196a27d98ca6aa493290c9078f206dbb/%EC%A4%91%EA%B0%84%EB%B2%94%EC%9C%84/7-2-2.c#L12-L40)는 더미 헤더의 `data`를 초기화하지 않고 `contain`이 헤더부터 값을 읽는다. 초기화하지 않은 정수를 읽는 것은 정의되지 않은 동작이다. 또 `getnode()` 정의에는 인수가 없는데 `getnode(data)`로 호출한다. 이 결함은 이번에 코드를 읽으며 확인했다. 노트에는 당시 이 오류를 겪었다는 기록이 없다.

안전한 규칙은 간단하다. 더미 헤더는 연결의 시작점으로만 쓰고 검색은 `head->next`에서 시작한다. 값이 없는 노드의 값 필드를 비교하지 않는다.

집합 구현에서 확인할 것은 자료구조 이름이 아니다. 중복을 어디서 막는지, 정렬을 어디서 보장하는지, 공집합일 때 헤더 다음이 NULL인지가 실제 계약이다. [앞 글의 다항식 merge](/2026/09/data-structure-04-polynomial.html)와 같은 두 포인터 구조도 이 계약 위에서만 맞게 작동한다.
