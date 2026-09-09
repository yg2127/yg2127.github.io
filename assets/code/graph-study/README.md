# 그래프와 DP 글의 실행 코드

2026-09-08에 알고리즘 9~14편을 보완하기 위해 작성한 코드입니다. 과거 수업 저장소의 제출 파일이 아닙니다.

```bash
clang -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g graph_check.c -o /tmp/graph-check
/tmp/graph-check
```

`graph_check.c`는 C11과 표준 라이브러리만 사용합니다. 정점 수는 최대 16, 간선 무게는 -1000~1000인 작은 입력에 한정합니다. 간선 중복 삽입은 가중치 갱신으로 처리합니다.

| 글 | 함수 |
| --- | --- |
| 그래프의 저장 방식이 탐색 비용을 바꾼다 | `add_arc`, `remove_arc`, `to_matrix` |
| DFS는 바로 들어가고 BFS는 차례를 기다린다 | `dfs_visit`, `bfs` |
| 방향이 생기면 방문 표시만으로는 부족하다 | `cycle_visit`, `topological`, `scc` |
| 가장 싼 선택을 믿을 수 있는 조건 | `prim_forest`, `kruskal_forest` |
| 최단 경로는 음수 간선에서 갈라진다 | `dijkstra`, `bellman_ford` |
| 같은 부분문제를 다시 풀지 않는다 | `airtel_dc`, `airtel_dp` |

`main`은 작은 입력의 완전 열거와 별도 알고리즘 비교를 실행한 뒤 글의 예제를 출력합니다. 실행시간 벤치마크가 아닙니다. DFS 방문 순서는 이웃 번호 오름차순을 기준으로 합니다. SCC의 역방향 그래프는 선형 시간에 만들기 위해 머리 삽입하며 이웃 정렬이 필요하지 않습니다.

Prim과 Dijkstra는 배열을 훑어 다음 정점을 고릅니다. 힙을 사용하는 구현의 시간 복잡도와 구분해야 합니다. 분리 집합은 경로 압축과 크기 기준 병합을 사용합니다. Bellman-Ford의 `bad`는 음수 순환 자체에 있는 정점만이 아니라 그 영향을 받아 비용을 끝없이 낮출 수 있는 도착점을 표시합니다.

Airtel은 도시 번호가 증가하는 경로에 한정하고, 경유지에서만 숙박비를 셉니다. `previous`로 복원한 경로 비용도 검증합니다.
