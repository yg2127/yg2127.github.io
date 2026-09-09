---
layout: post
title: "이진 탐색은 못 찾았을 때의 답도 정한다"
description: "같은 이진 탐색 구조가 exact search, predecessor, lower bound에서 다른 실패값을 내는 이유를 계약으로 정리한다."
date: 2025-10-13
category: "Algorithm"
tags: [algorithm, binary-search, lower-bound, c]
series: algorithms
series_title: "알고리즘 학습 기록"
series_order: 8
permalink: /2026/09/algorithm-08-binary-search.html
comments: false
---

## 비어 있는 Dictionary 아래의 세 코드

6주차 노트에는 비교 정렬의 하한을 정리한 다음 `Dictionary`라는 제목만 남아 있다. 설명은 거의 비어 있지만 저장소에는 이진 탐색 코드 세 개가 있다. 하나는 재귀 탐색, 하나는 lower bound, 하나는 Y/N 답으로 구간을 줄이는 문제다.

세 코드를 나란히 읽으니 이진 탐색에서 먼저 정할 것은 `mid` 공식보다 반환값의 의미였다. 찾는 값이 배열 안에 있을 때만 생각하면 코드들이 비슷해 보인다. 값이 없거나 중복될 때는 서로 다른 문제를 푼다.

## 같은 배열에서 달라지는 계약

정렬 배열 `A = [1, 3, 3, 7]`을 기준으로 세 계약을 구분해 보자.

| 함수 계약 | 성공 또는 실패의 의미 | k=3 | k=4 | k=0 | k=8 |
|---|---|---:|---:|---:|---:|
| exact search | 같은 값의 인덱스, 없으면 -1 | 1 또는 2 | -1 | -1 | -1 |
| predecessor | `A[i] <= k`인 마지막 인덱스, 없으면 -1 | 2 | 2 | -1 | 3 |
| lower bound | `A[i] >= k`인 첫 인덱스, 없으면 n | 1 | 3 | 0 | 4 |

`1013-1.c`는 값이 같으면 바로 인덱스를 반환하지만 탐색 구간이 비면 `r`을 반환한다. 흔한 exact search의 `-1`과 다르며 predecessor 결과와 닮았다. 다만 값을 찾았을 때는 중복 중 아무 위치나 즉시 반환하므로 완전한 predecessor 함수도 아니다. 과제 명세가 남아 있지 않아 이 혼합된 반환이 의도였는지는 확정할 수 없다.

`1013-2.c`는 첫 `A[i] >= k`를 찾는다. 값이 없을 때 n을 반환하는 것은 오류 표시가 아니다. k를 정렬 순서를 유지하며 삽입할 위치가 배열 끝이라는 뜻이다.

## lower bound가 지키는 두 구간

이번 재현 구현은 반열린 구간 `[left, right)`를 사용한다.

```c
int lower_bound(const int *a, int n, int key) {
    int left = 0;
    int right = n;

    while (left < right) {
        int mid = left + (right - left) / 2;
        if (a[mid] < key) left = mid + 1;
        else right = mid;
    }
    return left;
}
```

반복 중 `left`보다 앞의 원소는 모두 key보다 작고, `right`부터 뒤의 원소는 모두 key 이상이다. `left..right-1`만 아직 답 후보로 남는다. `a[mid] < key`이면 mid까지는 답일 수 없어 `left=mid+1`로 버린다. 반대로 `a[mid] >= key`이면 mid가 답일 가능성이 있으므로 mid를 버리지 않고 `right=mid`로 둔다.

이 두 번째 대입이 중복에서 중요하다. `[1, 3, 3, 7]`, key=3을 추적하면 처음 `mid=2`, 값은 3이다. 답 후보이므로 `right=2`가 된다. 다음 `mid=1`도 3이라 `right=1`이 된다. 아직 `left=0`이므로 한 번 더 비교한다. `mid=0`의 값 1은 3보다 작아 `left=1`이 되고, 이제 `left=right=1`에서 끝나 첫 번째 3의 위치 1을 반환한다. 같은 값을 찾았다고 즉시 끝냈다면 인덱스 2가 나와 lower bound 계약을 어겼다.

key=4에서는 처음 `mid=2`의 3이 작아 `left=3`이 된다. 다음 `mid=3`의 7은 크므로 `right=3`이 되고 답은 3이다. key=8이면 모든 원소가 작은 구간으로 들어가 `left=n`에서 끝난다. 빈 배열도 처음부터 `left=right=0`이라 안전하게 0을 반환한다.

## 실패값도 정상적인 답이다

predecessor는 반대 방향의 경계를 찾는다. `A[mid] <= key`이면 mid를 포함해 더 오른쪽을 찾아야 하므로 `left=mid+1`, 크면 `right=mid-1`로 옮긴다. 종료 후 `right`가 마지막 `<= key` 위치다. 최솟값보다 작은 key에서는 `right=-1`이 된다.

이번 [재현 코드](/assets/code/algorithm-study/08-binary-search-contracts.c)는 빈 배열과 작은 비감소 배열들을 만들고 각 key에 대해 선형 탐색 결과와 exact, predecessor, lower bound를 대조한다. 중복, 없는 중간값, 양 끝 바깥값도 포함하며 Sanitizer 실행에서 `binary-search contracts: ok`를 확인했다.

이진 탐색은 후보 구간을 절반으로 줄여 O(log n)에 동작한다. 그러나 빠른 반복문만으로 함수가 완성되는 것은 아니다. 못 찾았을 때 -1을 줄지, 앞 원소를 줄지, 삽입 위치를 줄지를 먼저 정해야 성공 조건과 경계 대입도 함께 결정된다.

### 참고한 기록과 코드

- 개인 노트 「6주차 - Dictionary(사전 , 선형탐색 & 이진탐색)」, 2025-10-13
- [`Week6 Dictionary/1013-1.c`](https://github.com/yg2127/25-2_Algorithms/blob/8bc95230beafb8e1e3587349b9a98d163a011a96/Week6%20Dictionary/1013-1.c)
- [`Week6 Dictionary/1013-2.c`](https://github.com/yg2127/25-2_Algorithms/blob/8bc95230beafb8e1e3587349b9a98d163a011a96/Week6%20Dictionary/1013-2.c)

[이전 글: 퀵 정렬의 경계는 한 칸도 그냥 넘길 수 없다](/2026/09/algorithm-07-quick-sort.html) · [다음 글: 그래프의 저장 방식이 탐색 비용을 바꾼다](/2026/09/algorithm-09-graph-storage.html)
