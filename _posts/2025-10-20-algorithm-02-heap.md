---
layout: post
title: "힙은 한 경로만 고쳐도 된다"
description: "OJ 60점과 DownHeap 의심 메모에서 출발해 힙 삽입과 삭제가 한 경로만 복구하는 이유를 확인한다."
date: 2025-10-20
category: "Algorithm"
tags: [algorithm, heap, priority-queue, c]
series: algorithms
series_title: "알고리즘 학습 기록"
series_order: 2
permalink: /2026/09/algorithm-02-heap.html
comments: false
---

## OJ 60점 옆에 남은 의심

힙 실습 코드에는 날짜와 함께 짧은 주석이 남아 있다.

> 현재 문제가 있음 oj 60점, 아무래도 DownHeap에서 문제 발생하나봄

이 문장은 원인을 찾았다는 기록이 아니라 당시 `DownHeap`을 의심했다는 기록이다. 현재 남은 코드만으로 그 추측을 원인으로 확정할 수는 없다. 여기서는 올바른 `DownHeap`이 보장해야 하는 조건을 다시 따라간다.

최대 힙에는 두 조건이 있다. 배열이 완전이진트리를 나타내고, 모든 자식의 값이 부모보다 크지 않아야 한다. 1번 인덱스를 루트로 쓰면 부모는 `i/2`, 왼쪽 자식은 `2*i`, 오른쪽 자식은 `2*i+1`이다. 형제끼리의 순서는 정해져 있지 않다.

## 삽입은 위로 한 경로만 본다

`[9, 7, 5, 2]`인 최대 힙에 12를 넣으면 새 값은 다음 빈 자리인 5번에 놓인다. 기존 노드끼리의 관계는 삽입 전부터 올바르다. 새로 깨질 수 있는 관계는 12와 부모 7, 그다음 부모 9뿐이다.

```c
static void up_heap(int i) {
    while (i > 1 && heap[i] > heap[i / 2]) {
        swap(&heap[i], &heap[i / 2]);
        i /= 2;
    }
}
```

12와 7을 바꾸고, 다시 12와 9를 바꾸면 끝난다. 다른 가지는 새 값과 닿지 않았으므로 다시 검사할 이유가 없다. 노트에서 “추가된 노드와 그 부모를 비교하고, 부모에 대해 재귀적으로 시행”한다고 쓴 부분이 바로 이 경로를 가리킨다.

## 삭제는 더 큰 자식 쪽으로 내려간다

최댓값을 꺼내면 루트가 빈다. 저장소 구현은 마지막 값을 루트로 옮겨 완전이진트리 모양을 먼저 복구한다. 이제 깨질 수 있는 곳은 새 루트에서 아래로 이어지는 한 경로다.

```c
static void down_heap(int i) {
    while (2 * i <= size) {
        int child = 2 * i;
        if (child + 1 <= size && heap[child + 1] > heap[child]) child++;
        if (heap[i] >= heap[child]) break;
        swap(&heap[i], &heap[child]);
        i = child;
    }
}
```

여기서 두 자식 중 더 큰 쪽을 골라야 한다. 작은 자식과만 바꾸면 큰 자식이 부모보다 큰 상태가 남을 수 있다. 자식이 하나뿐인 경우에는 왼쪽 자식만 비교해야 한다. `DownHeap`의 정확성은 자식 존재 범위와 더 큰 자식 선택을 모두 만족해야 한다.

노트에는 “remove는 root 제거이므로 O(1)”이라는 문장도 있다. 루트 값을 읽는 일만 보면 O(1)이다. 그러나 우선순위 큐의 삭제 연산은 모양과 힙 순서를 복구해야 끝난다. 완전이진트리의 높이가 O(log n)이므로 `removeMax` 전체는 O(log n)이다.

## 이번에 다시 확인한 불변식

이번에는 원본을 고쳐 썼다고 가정하지 않고, 별도의 [재현 코드](/assets/code/algorithm-study/02-heap-invariant.c)를 만들었다. `4, 9, 1, 7, 7, 12, 3`을 차례로 삽입하고 매번 모든 부모·자식 관계를 검사했다. 이후 최댓값을 연속으로 꺼내 `12, 9, 7, 7, 4, 3, 1`과 같은지도 확인했다.

검사는 통과했다. 이것이 과거 OJ 60점의 원인을 밝혔다는 뜻은 아니다. 당시 입력과 채점 명세가 없기 때문에 그 원인은 여전히 미해결이다. 지금 확인한 것은 올바른 `UpHeap`과 `DownHeap`이 지켜야 할 계약이다. 한 번의 변경이 한 자리에서 시작되므로, 그 자리에서 루트 또는 리프까지 이어지는 한 경로만 고치면 충분하다.

### 참고한 기록과 코드

- 개인 노트 「3주차 - 힙, 힙순서 성질 유지」, 2025-10-20
- [`Week3(Heap)/3-1.c`](https://github.com/yg2127/25-2_Algorithms/blob/8bc95230beafb8e1e3587349b9a98d163a011a96/Week3%28Heap%29/3-1.c)
- [`Week3(Heap)/0915-1.c`](https://github.com/yg2127/25-2_Algorithms/blob/8bc95230beafb8e1e3587349b9a98d163a011a96/Week3%28Heap%29/0915-1.c)

[이전 글: 삽입 정렬은 이미 정렬된 구간을 믿는다](/2026/09/algorithm-01-selection-insertion.html) · [다음 글: 모든 노드가 끝까지 내려가지는 않는다](/2026/09/algorithm-03-build-heap.html)
