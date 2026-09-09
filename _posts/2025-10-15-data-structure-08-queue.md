---
layout: post
title: "큐가 비는 순간 두 포인터를 함께 본다"
description: "연결 큐가 0개와 1개 사이를 오갈 때 front, rear, next, size를 어떤 순서로 바꿔야 하는지 확인한다."
date: 2025-10-15
category: "Data Structure"
tags: [data-structure, c, queue, deque]
series: data-structure
series_title: "자료구조 학습 기록"
series_order: 8
series_numbered: true
series_intro: "A를 넣고 빼 다시 빈 상태로 돌아가는 예제로 FIFO, front, rear와 연결 큐의 경계를 익힌다."
series_from: "스택은 같은 top에서 넣고 뺐지만 큐는 입력 위치와 출력 위치를 나누어 오래 기다린 값부터 처리한다."
series_to: "선형 자료구조의 끝 상태를 다룬 뒤 다음 편에서는 자식으로 갈라지는 트리를 재귀로 방문한다."
last_modified_at: 2026-09-09
update_note: "기초 개념과 예제를 보강하고 시리즈 흐름을 연결"
permalink: /2026/09/data-structure-08-queue.html
comments: false
---

## 먼저 들어온 값을 먼저 꺼내는 저장소

빈 저장소에 A를 넣고 B를 넣은 뒤 하나를 꺼내면 A가 나와야 한다. 이런 규칙을 **FIFO**, First In First Out이라고 하며 이를 제공하는 자료형이 **큐**다. 대기 줄처럼 먼저 들어온 항목을 먼저 처리할 때 쓴다.

큐의 뒤에 값을 넣는 연산을 **enqueue**, 앞의 값을 빼는 연산을 **dequeue**라고 한다. 연결 큐에서 `front`는 다음에 제거할 첫 노드, `rear`는 새 노드를 붙일 마지막 노드를 가리킨다. 노드는 데이터와 다음 노드 주소 `next`를 가진다.

`NULL`은 포인터가 어떤 유효한 노드도 가리키지 않는다는 표시다. `malloc`은 새 노드를 둘 메모리를 동적으로 확보하고 그 주소를 반환하며, `free`는 더 쓰지 않는 노드의 메모리를 반납한다. `free`한 노드의 주소를 계속 따라가면 안 되므로 삭제 뒤 남은 포인터도 현재 큐 상태에 맞게 고쳐야 한다.

여러 노드가 있을 때보다 0개와 1개 사이를 오갈 때 경계가 더 중요하다. 마지막 노드를 빼면 `front`와 `rear`가 모두 NULL이어야 하고, 빈 큐에 첫 노드를 넣으면 두 포인터가 같은 노드를 가리켜야 한다.

## 스택과 달리 두 위치를 기억한다

`큐 ADT` 노트는 스택과의 차이에서 출발한다.

> 스택에서는 top 위치에 넣고 빼고 했었는데 큐는 이 입출력 위치 포인터가 구분되어 있음!
>
> 맨 뒤(rear)에 추가, 맨 앞(front)에 삭제

노드가 여러 개일 때는 이 설명대로 rear 뒤에 붙이고 front에서 뺀다. 실제로 더 조심해야 하는 순간은 큐가 비어 있다가 첫 노드가 생길 때, 그리고 마지막 노드 하나를 제거해 다시 빌 때다.

```text
0개: front = NULL, rear = NULL
1개: front ─┐
             ├→ [A] → NULL
      rear ──┘
```

첫 삽입에서는 front와 rear가 같은 새 노드를 가리켜야 한다. 이후 삽입은 `rear->next`를 새 노드에 연결하고 rear만 옮긴다.

## next를 NULL로 만드는 이유

노트의 삽입 코드에는 다음 주석이 붙어 있다.

> `newnode->next = NULL;` 이거해줘야 빈 큐와 들어있는 큐 사이를 왔다갔다 할 수 있음

연결 큐의 순회와 삭제는 마지막 노드의 `next`가 NULL이라는 약속에 기대고 있다. 그런데 [저장소의 연결 큐 버전](https://github.com/yg2127/25-1_data_structure/blob/eab0311e196a27d98ca6aa493290c9078f206dbb/11-1-2.c#L38-L69)은 malloc한 새 노드의 `next`를 초기화하지 않는다. 초기화되지 않은 포인터를 마지막 링크로 남기면 출력이 큐 밖으로 진행할 수 있다.

이 결함은 이번에 코드를 읽으며 확인했다. 노트에는 당시 이 파일의 오류를 재현했다는 기록이 없다. 이번 예제에서는 노드를 만들 때 `{value, NULL}`로 초기화했다.

## 마지막 삭제의 순서

삭제는 front를 다음 노드로 옮기고 기존 노드를 해제한 뒤 size를 줄인다.

```c
Node *old = q->front;
q->front = old->next;
free(old);
q->size--;
if (q->front == NULL) q->rear = NULL;
```

노트에는 “사이즈 갱신의 위치주의”라는 말이 두 번 나온다. `isEmpty`가 `size==0`으로 판단한다면 감소시키기 전과 후의 답이 다르다. 마지막 노드 삭제 후 size를 줄이고 empty를 검사해야 rear도 NULL로 돌릴 수 있다.

이번 [큐 경계 검증 코드](/assets/code/data-structure-study/08-queue.c)는 빈 큐, 한 노드, 다시 빈 큐, 재사용을 차례로 실행했다.

```text
empty size=0 front=NULL rear=NULL
one size=1 front=node rear=node
removed=10 empty-again size=0 front=NULL rear=NULL
reused=20
```

마지막 노드를 지웠는데 rear가 해제된 노드를 계속 가리키면 위험한 dangling pointer가 남는다. 다음 enqueue가 `front==NULL`이나 `size==0`을 먼저 보고 빈 큐 분기로 들어가 rear를 새 노드로 덮어쓰면 곧바로 역참조하지 않을 수 있다. 반대로 `rear != NULL`만 보고 `rear->next`에 연결하는 구현이라면 use-after-free가 된다. 어느 분기를 택하든 빈 큐의 불변식 `front==NULL && rear==NULL`을 회복하는 편이 안전하다.

## 원형 큐와 덱으로 넓히기

[원형 배열 큐](https://github.com/yg2127/25-1_data_structure/blob/eab0311e196a27d98ca6aa493290c9078f206dbb/11-1-1.c#L11-L47)는 `front == rear`를 empty로, `front == (rear+1)%size`를 full로 둔다. 두 상태를 구분하려고 배열 한 칸을 비우므로 할당 크기가 n이면 실제 저장 용량은 n-1이다. 원래 과제에서 `size`가 할당 크기인지 목표 용량인지는 명세 확인이 필요하다.

크기 4인 배열의 인덱스는 0, 1, 2, 3이다. `% 4`를 사용하면 3 다음 위치가 `(3+1)%4=0`이 되어 배열 끝에서 처음으로 돌아간다. 한 칸을 비우는 구현에서는 세 원소까지만 저장한다.

```text
초기               [·, ·, ·, ·]  front=0 rear=0
10,20,30 enqueue   [·,10,20,30]  front=0 rear=3  full
10,20 dequeue      [·, ·, ·,30]  front=2 rear=3
40,50 enqueue      [40,50,·,30]  front=2 rear=1  full
dequeue 순서       30 → 40 → 50
```

`·`는 현재 큐의 원소로 사용하지 않는 칸을 뜻한다. 실제 메모리 값을 지웠다는 뜻은 아니다. front와 rear의 정확한 칸 사용 방식은 구현 관례에 따라 다를 수 있다. 위 상태는 front 칸을 비워 두고 rear가 마지막 원소를 가리키는 관례를 따른 설명용 예다. 핵심은 `%`가 끝 인덱스를 처음으로 순환시키고, 비어 있음과 가득 참을 구분하기 위해 한 칸을 예약한다는 점이다.

덱은 front와 rear 양쪽에서 삽입·삭제하지만 경계 원리는 같다. [덱 구현](https://github.com/yg2127/25-1_data_structure/blob/eab0311e196a27d98ca6aa493290c9078f206dbb/11-2-1.c#L32-L84)은 한 노드 삭제 후 비었으면 반대쪽 포인터도 NULL로 만들고, 남았다면 새 끝 노드의 바깥 링크를 NULL로 만든다.

큐에서 기억할 것은 FIFO라는 문장만이 아니다. 0개, 1개, 여러 개 상태 사이를 이동할 때 `front`, `rear`, `size`, 마지막 노드의 `next`가 같은 상태를 말하는지 함께 확인해야 한다.
