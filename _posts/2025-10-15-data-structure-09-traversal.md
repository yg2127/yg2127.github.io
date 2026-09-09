---
layout: post
title: "트리 순회는 처리하는 시점이 다르다"
description: "전위·중위·후위 순회를 세 가지 암기 순서가 아니라 현재 노드를 처리하는 시점의 차이로 읽는다."
date: 2025-10-15
category: "Data Structure"
tags: [data-structure, c, tree, traversal, recursion]
series: data-structure
series_title: "자료구조 학습 기록"
series_order: 9
permalink: /2026/09/data-structure-09-traversal.html
comments: false
---

## 세 순서를 하나의 코드로 보기

`트리 순회` 노트에는 전위, 중위, 후위 순서가 나란히 적혀 있다.

```text
preorder  : root → left → right
inorder   : left → root → right
postorder : left → right → root
```

세 함수를 따로 외우기보다 같은 재귀 코드에서 root 처리 줄만 옮겨 보면 차이가 작아진다.

```c
void traverse(Node *root) {
    if (root == NULL) return;
    /* preorder: 여기서 처리 */
    traverse(root->left);
    /* inorder: 여기서 처리 */
    traverse(root->right);
    /* postorder: 여기서 처리 */
}
```

[저장소 구현](https://github.com/yg2127/25-1_data_structure/blob/eab0311e196a27d98ca6aa493290c9078f206dbb/13-1.c#L22-L51)도 정확히 이 세 위치에 `printf`를 둔다. `findid`는 현재 노드를 보고 왼쪽에서 찾은 결과가 있으면 즉시 반환한 뒤 오른쪽으로 간다. 전위 순회와 같은 탐색 순서다.

## 작은 트리에서 처리 시점 표시하기

다음 트리를 사용했다.

```text
        1
      /   \
     2     3
    / \
   4   5
```

이번에 만든 [순회 검증 코드](/assets/code/data-structure-study/09-tree-traversal.c)의 출력은 다음과 같다.

```text
pre  12453
in   42513
post 45231
sum 15
```

전위 순회는 자식으로 내려가기 전에 1을 처리하므로 root가 처음이다. 중위 순회는 왼쪽 subtree를 마친 뒤 1을 처리한다. 후위 순회는 양쪽 자식을 모두 마친 뒤 1을 처리하므로 root가 마지막이다.

노트가 후위 순회를 다음처럼 정리한 이유도 여기서 보인다.

> 한 노드를 처리하려면 그 자식 전체가 정리된 뒤여야 한다

subtree 합은 왼쪽 합과 오른쪽 합을 먼저 구해야 현재 노드 값을 더할 수 있다.

```c
return sum(root->left) + sum(root->right) + root->data;
```

[subtree 합 코드](https://github.com/yg2127/25-1_data_structure/blob/eab0311e196a27d98ca6aa493290c9078f206dbb/13-2.c#L25-L38)는 후위 처리의 또 다른 형태다. 동적으로 만든 트리를 해제할 때도 자식을 먼저 free한 뒤 root를 free해야 하므로 후위 순회가 맞는다.

## 중위 순회가 정렬인 조건

`이진트리 구현` 노트에는 “이진트리의 정렬은? 그건 알고리즘에서!”라는 질문이 있다. 모든 이진트리의 중위 순회가 정렬 결과를 만드는 것은 아니다. 각 노드에서 왼쪽 값은 더 작고 오른쪽 값은 더 크다는 이진 탐색 트리의 규칙이 있을 때만 중위 순서가 오름차순이다.

위 예제는 값 4와 5가 노드 2의 자식이므로 BST가 아니다. 실제 중위 결과 `4,2,5,1,3`도 정렬되어 있지 않다. 순회 순서와 트리가 만족하는 값의 규칙을 구분해야 한다.

재귀 깊이는 트리 높이 h만큼 필요하다. 균형 잡힌 트리라면 $O(\log n)$ 깊이지만 한쪽으로 기울면 $O(n)$이 되어 호출 스택이 커진다. 방문 시간은 모양과 관계없이 모든 노드를 한 번씩 처리하므로 $O(n)$이다.

[재귀를 다룬 2편](/2026/09/data-structure-02-recursion.html)에서 “돌아온 뒤 남은 문장”을 확인했다. 트리 순회에서는 그 문장을 왼쪽 호출 전, 두 호출 사이, 오른쪽 호출 뒤 어디에 두는지가 처리 순서를 결정한다.
