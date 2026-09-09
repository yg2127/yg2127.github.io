---
layout: post
title: "스택에서 기억해야 하는 것은 맨 위다"
description: "top=-1의 이유와 연결 스택의 top 포인터를 따라가며 배열과 연결리스트 구현의 같은 불변식을 찾는다."
date: 2025-10-15
category: "Data Structure"
tags: [data-structure, c, stack, linked-list]
series: data-structure
series_title: "자료구조 학습 기록"
series_order: 6
permalink: /2026/09/data-structure-06-stack.html
comments: false
---

## top을 -1에서 시작한 이유

`스택 ADT` 노트는 “stack에서 가장 중요한 부분은 가장 마지막에 들어간 원소”라고 적고, 배열의 `top`을 -1로 두는 이유를 다음처럼 풀었다.

> 첫 번째 push 연산 시 ++top을 먼저하면 0이 되어 자연스럽게 data[0]부터 채워짐

저장소 주석에도 같은 이해가 더 짧게 남아 있다.

> 내 top 인덱스는 항상 최상단원소를 가리키고 있음!

빈 스택에는 최상단 원소가 없으므로 유효 인덱스 0보다 하나 작은 -1을 상태로 쓴다. push는 먼저 증가시킨 뒤 저장하고, pop은 현재 값을 꺼낸 뒤 감소시킨다.

```c
data[++top] = value;
value = data[top--];
```

따라서 빈 상태는 `top == -1`, 가득 찬 상태는 `top + 1 == capacity`다. [배열 스택 구현](https://github.com/yg2127/25-1_data_structure/blob/eab0311e196a27d98ca6aa493290c9078f206dbb/9-1-2.c#L10-L40)은 이 기준을 그대로 사용한다.

## 연결 스택에서도 같은 위치를 본다

노트는 연결리스트 기반 스택을 “헤드 부분만 컨트롤하면 된다”고 정리했다. 배열 인덱스 대신 `top`이 맨 앞 노드를 가리킨다.

```text
push C     top → C → B → A → NULL
pop        top → B → A → NULL
```

```c
newnode->next = stack->top;
stack->top = newnode;
stack->size++;
```

pop에서는 기존 top을 임시 포인터에 보관하고 top을 다음 노드로 옮긴 뒤 기존 노드를 해제한다. 배열과 저장 방식은 달라도 “다음 pop이 꺼낼 위치를 top이 가리킨다”는 불변식은 같다.

이번에 만든 [연결 스택 경계 검증](/assets/code/data-structure-study/06-stack.c)은 최대 크기 2에서 세 번 push한 뒤 빌 때까지 pop했다.

```text
push A 1
push B 1
overflow C 0
pop B size=1
pop A size=0
underflow 0 top_null=1
```

LIFO 순서와 함께 `size==0`일 때 `top==NULL`인 상태도 확인했다. 연결리스트를 쓴다고 용량 제한이 반드시 없어지는 것은 아니다. 노트처럼 `max`와 `size`를 두면 논리적 overflow를 정의할 수 있다.

## 코드 오류와 사고 기록을 구분하기

[연결 스택 파일](https://github.com/yg2127/25-1_data_structure/blob/eab0311e196a27d98ca6aa493290c9078f206dbb/9-1-3%28%EC%97%B0%EA%B2%B0%EB%A6%AC%EC%8A%A4%ED%8A%B8%29.c#L55-L65)의 `dup`에는 선언되지 않은 `max`와 타입명 `node`가 있어 현재 상태로 컴파일되지 않는다. 이는 C11 구문 검사로 확인한 코드 사실이다. 노트와 주석에는 당시 이 오류를 발견하거나 고쳤다는 기록이 없어, 여기서는 현재 코드 상태로만 다룬다.

노트에 남은 `strcmp(str, "POP") == 0`도 같은 상태 관리와 연결된다. C에서 `str == "POP"`은 문자열 내용이 아니라 주소를 비교한다. 명령 문자열의 내용이 같은지를 검사해야 올바른 연산을 선택한다.

배열과 연결 구현을 비교하고 나니 스택을 외울 항목은 push와 pop 함수의 모양이 아니었다. `top`이 무엇을 가리키는지, 빈 상태와 가득 찬 상태를 어떤 식으로 표현하는지, 한 연산 뒤 `size`와 `top`이 함께 맞는지가 기준이었다. 다음 글에서는 이 작은 계약으로 괄호와 수식을 읽는다.
