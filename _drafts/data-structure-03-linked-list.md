---
layout: post
title: "자료구조 3편: 이중 연결리스트의 삽입·삭제와 포인터 변경"
description: "포인터 대입 순서를 잘못 바꾸자 새 노드가 자기 자신을 가리켰다. 당시 디버깅 노트의 사고를 따라가며 이중 연결리스트의 삽입과 삭제 불변식을 정리한다."
date: 2026-09-08 09:00:00 +0900
category: "Data Structure"
tags: [data-structure, c, linked-list, pointer, debugging]
series: data-structure
series_title: "자료구조 학습 기록"
series_order: 3
permalink: /2026/09/data-structure-03-linked-list.html
comments: false
---

## 포인터 네 줄에서 시작된 오류

자료구조를 공부하며 적은 `디버깅 기법` 노트에는 이중 연결리스트의 정의보다 먼저 이런 경고가 남아 있다.

> append에서 꼭 next와 prev를 지정할 때 순서를 조심해라!
>
> next와 prev를 갱신하는 과정에서 curr 기준의 prev와 next가 바뀔 수 있기 때문이다!

그 아래에는 문제가 된 네 줄도 그대로 적혀 있다.

```c
newnode->next = curr;
newnode->prev = curr->prev;
curr->prev = newnode;
(curr->prev)->next = newnode;
```

처음 읽으면 자연스러워 보인다. 새 노드의 오른쪽에는 `curr`을 두고, 왼쪽에는 원래 `curr`의 이전 노드를 둔다. 그다음 `curr`의 왼쪽을 새 노드로 바꾸고, 왼쪽 노드의 오른쪽도 새 노드로 바꾼다. 말로만 읽으면 양쪽 연결을 모두 고친 것 같다.

그런데 세 번째 줄을 실행한 순간 `curr->prev`의 의미가 달라진다. 이제 그것은 원래 왼쪽 노드가 아니라 방금 만든 `newnode`다. 따라서 네 번째 줄은 실제로 다음 문장이 된다.

```c
newnode->next = newnode;
```

노트에 적어 둔 원인 분석은 이 한 문장으로 정리할 수 있다. **다음 대입에서 다시 읽을 포인터를 먼저 덮어쓰면 안 된다.**

## 화살표를 한 줄씩 따라가기

삽입 전 상태를 `A`와 `B` 사이에 `X`를 넣는 상황으로 줄여 보자. `curr`은 `B`, `newnode`는 `X`다.

```text
A <-> B
     ^
    curr
```

잘못된 순서의 앞 두 줄까지는 문제가 없다. `X.next`에 `B`, `X.prev`에 `A`를 기록한 상태다. 이어지는 대입을 포함해 각 포인터가 가리키는 노드를 표로 적어 보자. `—`는 아직 예시에서 값을 넣지 않은 필드다.

| 실행한 줄 | `A.next` | `B.prev` | `X.prev` | `X.next` |
|---|---|---|---|---|
| 삽입 전 | B | A | — | — |
| `X.next = B` | B | A | — | B |
| `X.prev = B.prev` | B | A | A | B |
| `B.prev = X` | B | X | A | B |
| `(B.prev)->next = X` | B | X | A | X |

세 번째 대입 뒤에는 `B.prev`가 이미 `X`다. 따라서 마지막 줄에서 바뀌는 필드는 `A.next`가 아니라 `X.next`다. 표에서도 `A.next`는 처음부터 끝까지 `B`로 남고, 마지막 칸만 `X`로 바뀐다.

새 노드를 만들기는 했지만 `A`에서 `X`로 가는 길은 없고, `X`는 자기 자신을 따라 도는 구조가 된다. 출력 반복문이 이 고리에 들어가면 끝나지 않을 수 있다.

## 기존 연결을 먼저 보존하기

저장소의 이중 연결리스트 구현에서는 세 번째와 네 번째 대입의 순서가 바뀌어 있다.

```c
newnode->next = curr;
newnode->prev = curr->prev;
(curr->prev)->next = newnode;
curr->prev = newnode;
```

처음 두 줄에서 `newnode`가 양쪽 노드를 기억한다. 세 번째 줄을 실행할 때는 `curr->prev`가 아직 `A`이므로 `A.next`를 안전하게 `X`로 바꿀 수 있다. 마지막에야 `B.prev`를 `X`로 바꾼다.

| 실행한 줄 | `A.next` | `B.prev` | `X.prev` | `X.next` |
|---|---|---|---|---|
| 앞 두 줄까지 | B | A | A | B |
| `(B.prev)->next = X` | X | A | A | B |
| `B.prev = X` | X | X | A | B |

마지막 상태는 `A <-> X <-> B`다. 새 노드의 양쪽 연결과 기존 노드 두 개의 연결이 모두 맞물린다.

이 네 줄이 지켜야 하는 결과는 두 방향에서 말할 수 있다.

```c
newnode->prev->next == newnode;
newnode->next->prev == newnode;
```

정방향으로 걸어도 `A, X, B`, 역방향으로 걸어도 `B, X, A`가 나와야 한다. 둘 중 한 방향만 출력해 보면 절반만 연결된 오류를 놓칠 수 있다.

## 오늘 다시 실행해 본 결과

노트에 적힌 분석을 확인하기 위해 이번에 위 상황만 떼어 낸 작은 C 프로그램을 만들었다. 2026년 9월 8일에 AddressSanitizer와 UndefinedBehaviorSanitizer를 켜고 실행한 결과다.

```text
bad: new.next==new true, A.next==new false, B.prev==new true
good forward: AXB
good backward: BXA
after delete forward: AB
after delete backward: BA
empty sentinels linked: true
```

잘못된 순서에서는 노트의 결론대로 자기 참조가 생겼다. 올바른 순서에서는 양방향 순회가 모두 맞았고, 가운데 노드를 지운 뒤에도 두 방향의 결과가 서로 대응했다.

## 삭제는 남은 두 노드를 잇는 일

삽입에서는 새 노드가 기존 연결 사이로 들어갔다. 삭제는 반대로 지울 노드의 양옆을 직접 이어 주면 된다.

```c
curr->next->prev = curr->prev;
curr->prev->next = curr->next;
free(curr);
```

`X`를 지운다면 첫 줄은 `B.prev = A`, 두 번째 줄은 `A.next = B`가 된다. 그 뒤에야 `X`의 메모리를 해제한다. `free(curr)`를 먼저 호출하고 `curr->next`나 `curr->prev`를 읽으면 이미 수명이 끝난 메모리에 접근하게 된다.

여기서 Head와 Tail 더미 노드가 경계를 단순하게 만든다.

```text
빈 리스트       Head <------> Tail
첫 원소 삽입    Head <------> A <------> Tail
첫 원소 삭제    Head <------> Tail
```

실제 구현도 삭제 대상이 Tail이면 `invalid position`을 출력하고, 실제 데이터 노드일 때만 양옆의 `prev`와 `next`를 연결한 뒤 해제한다. Head와 Tail이 항상 남아 있으므로 첫 데이터 노드와 마지막 데이터 노드도 가운데 노드와 같은 연결 규칙으로 삭제할 수 있다.

다만 더미 노드가 모든 오류를 없애 주는 것은 아니다. `search`가 어느 순위를 Tail로 돌려주는지, 삭제 대상이 실제 노드인지, 메모리를 모두 해제했는지는 별도로 확인해야 한다. `디버깅 기법` 노트에 적어 둔 “반복문을 line by line으로 따라가야 한다”는 말은 여기서 그대로 적용된다. 이중 연결리스트에서는 값보다 화살표가 어떻게 바뀌는지를 한 줄씩 따라가야 한다.

## 이번 구현에서 남은 기준

노트의 분석과 이번 실행에서 확인한 기준을 정리하면 다음과 같다.

- 포인터를 바꾸기 전에 그 값을 뒤에서 다시 읽는지 확인한다.
- 삽입 뒤에는 `prev->next`와 `next->prev`를 함께 검사한다.
- 정방향 출력만 보지 않고 역방향도 확인한다.
- 삭제할 노드의 이웃을 먼저 연결하고 마지막에 메모리를 해제한다.
- 빈 리스트와 첫·마지막 원소는 더미 노드까지 포함한 그림으로 추적한다.

노트에서 길게 짚은 부분은 같은 표현식 `curr->prev`가 대입 한 줄 전후로 전혀 다른 노드를 뜻하게 된다는 점이었다. 양쪽을 연결한다는 설명만으로는 놓쳤던 차이가 각 줄 뒤의 포인터 값을 적으면 드러난다. 다음 연결리스트 코드를 읽을 때도 어떤 필드를 바꾸는지와 그 필드가 지금 어느 노드를 가리키는지를 함께 따라가면 된다.

## 코드 근거

- [이중 연결리스트 삽입·삭제 구현](https://github.com/yg2127/25-1_data_structure/blob/eab0311e196a27d98ca6aa493290c9078f206dbb/%EC%A4%91%EA%B0%84%EB%B2%94%EC%9C%84/5-2.c#L38-L81)
