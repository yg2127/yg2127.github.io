---
layout: post
title: "퀵 정렬의 경계는 한 칸도 그냥 넘길 수 없다"
description: "Lomuto와 Hoare의 서로 다른 반환 경계를 구분하고 3-way partition의 세 구간을 직접 추적한다."
date: 2025-10-20
category: "Algorithm"
tags: [algorithm, quick-sort, partition, sanitizer, c]
series: algorithms
series_title: "알고리즘 학습 기록"
series_order: 7
series_numbered: true
series_intro: "pivot과 partition의 계약을 정의하고 Lomuto·Hoare·3-way의 재귀 경계를 구분한다."
series_from: "앞과 같은 분할 정복을 쓰되, 정렬 후 합치는 대신 pivot을 기준으로 먼저 구간을 나눈다."
series_to: "partition에서 후보 구간을 줄인 경험을 정렬 배열의 탐색 구간을 절반씩 버리는 이진 탐색으로 연결한다."
last_modified_at: 2026-09-09
update_note: "기초 개념과 예제를 보강하고 시리즈 흐름을 연결"
permalink: /2026/09/algorithm-07-quick-sort.html
comments: false
---

## pivot을 고른 뒤 남는 문제

퀵 정렬도 분할 정복 알고리즘이다. 배열에서 기준값인 pivot을 하나 고르고, partition으로 작은 값과 큰 값을 나눈 뒤, 아직 정렬되지 않은 양쪽 구간에 같은 작업을 반복한다. 길이가 0이나 1인 구간은 그대로 끝난다.

```text
[ 4, 1, 3, 2 ]  pivot=3
       partition
[ 1, 2 | 3 | 4 ]
  재귀      재귀
```

이 그림은 결과의 개념을 보여 줄 뿐, 모든 partition이 pivot을 가운데 최종 위치에 둔다는 뜻은 아니다. partition의 **계약**은 반환값이 무엇을 보장하고 다음 재귀가 어느 구간을 받아야 하는지 정한 약속이다. Lomuto와 Hoare를 섞으면 종료 조건과 경계가 깨지는 이유가 여기에 있다.

퀵 정렬 노트에는 pivot을 정한 뒤 왼쪽의 `i`가 큰 값을, 오른쪽의 `j`가 작은 값을 찾으면 서로 바꾸고 두 인덱스가 교차할 때까지 반복한다고 적혀 있다. 별도의 「lomulto」와 「Horae」 노트에는 두 partition 코드도 남아 있다.

둘 다 pivot보다 작은 값과 큰 값을 나누지만 반환값의 뜻은 다르다. Lomuto는 pivot을 최종 자리에 놓고 그 **pivot 인덱스**를 반환한다. 재귀 구간은 `left..p-1`과 `p+1..right`다. Hoare는 스캔이 교차한 **두 구간의 경계** `j`를 반환한다. pivot 값이 반드시 `j`에 있는 것은 아니며 재귀 구간은 `left..j`와 `j+1..right`다.

노트의 Hoare 코드가 `return j`를 한 뒤 `quicksort(l, p)`, `quicksort(p+1, r)`을 호출하는 이유가 여기에 있다. 반환값을 모두 “pivot의 새 위치”라고 읽으면 한 칸이 빠지거나 같은 구간을 다시 호출할 수 있다.

## 세 구간을 유지하는 3-way partition

저장소의 `0930-2.c`는 중복 키를 한 구간으로 모은다. 반복 중 `lt`, `i`, `gt`는 네 영역의 경계를 나타낸다.

- `left..lt-1`은 pivot보다 작다.
- `lt..i-1`은 pivot과 같다.
- `i..gt`는 아직 검사하지 않았다.
- `gt+1..right`는 pivot보다 크다.

```c
while (i <= gt) {
    if (a[i] < pivot) {
        swap(&a[i], &a[lt]);
        i++;
        lt++;
    } else if (a[i] > pivot) {
        swap(&a[i], &a[gt]);
        gt--;
    } else {
        i++;
    }
}
```

큰 값을 뒤로 보낼 때 `i`를 늘리지 않는 점이 중요하다. 뒤에서 새로 온 값은 아직 검사하지 않았기 때문이다. 작은 값을 앞쪽의 같은 값 구간과 바꾸면 새로 온 값은 이미 pivot과 같다는 것을 알고 있어 `i`와 `lt`를 함께 늘릴 수 있다.

## 중복 배열에서 움직이는 세 경계

`[3, 1, 3, 2, 3]`에서 pivot을 3으로 두고 시작하면 `lt=0`, `i=0`, `gt=4`다.

| 검사 값 | 수행 | 배열 | lt, i, gt |
|---:|---|---|---|
| 3 | 같은 구간 확장 | `3 1 3 2 3` | `0,1,4` |
| 1 | 작은 구간으로 swap | `1 3 3 2 3` | `1,2,4` |
| 3 | 같은 구간 확장 | `1 3 3 2 3` | `1,3,4` |
| 2 | 작은 구간으로 swap | `1 2 3 3 3` | `2,4,4` |
| 3 | 같은 구간 확장 | `1 2 3 3 3` | `2,5,4` |

종료하면 작은 구간은 `0..1`, 같은 구간은 `2..4`, 큰 구간은 비어 있다. 다음 재귀는 `left..lt-1`과 `gt+1..right`에만 적용한다. 세 개의 3은 이미 자기 구간에 있으므로 다시 정렬하지 않는다. 모든 값이 같은 배열이면 한 번의 선형 순회 뒤 양쪽 재귀가 모두 빈 구간이 된다.

## 이번에 재현한 범위 오류

초기 구현 `5-2.c`는 n칸 0-index 배열을 `QuickSort(arr, 1, n)`으로 호출한다. 유효 범위는 `0..n-1`인데 `findpivot`이 `arr[n]`을 읽는다. `5 4 3 2 1`을 AddressSanitizer로 실행해 `findpivot`의 heap-buffer-overflow를 확인했다. 당시 이 오류를 발견했다는 기록은 없으며, 이번에 재현한 결과다.

호출만 `0, n-1`로 바꿔도 초기 partition의 pivot 위치와 재귀 경계가 일관적인지는 다시 검증해야 한다. 그래서 [재현 코드](/assets/code/algorithm-study/07-quick-sort.c)는 위의 3-way 계약을 독립적으로 구현했다. 값이 0, 1, 2인 길이 0부터 6까지 모든 배열 1093개를 `qsort`와 대조했고 Sanitizer에서 통과했다.

퀵 정렬은 평균 O(n log n)이지만 계속 한쪽으로만 나뉘면 O(n²)이다. median-of-three도 최악을 없애지는 않는다. 그보다 먼저 맞아야 하는 것은 partition이 무엇을 반환하고 어느 구간을 끝냈는지다. Lomuto의 pivot 위치, Hoare의 분할 경계, 3-way의 같은 값 범위는 서로 바꾸어 쓸 수 없는 계약이다.

### 참고한 기록과 코드

- 개인 노트 「5주차 - Merge Sort, Quick Sort」, 「lomulto」, 「Horae」, 2025-10-19~20
- [`Week5 Merge, Quick Sort/5-2.c`](https://github.com/yg2127/25-2_Algorithms/blob/8bc95230beafb8e1e3587349b9a98d163a011a96/Week5%20Merge%2C%20Quick%20Sort/5-2.c)
- [`Week5 Merge, Quick Sort/0930-2.c`](https://github.com/yg2127/25-2_Algorithms/blob/8bc95230beafb8e1e3587349b9a98d163a011a96/Week5%20Merge%2C%20Quick%20Sort/0930-2.c)
