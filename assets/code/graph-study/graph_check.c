/* 2026-09-08: New explanatory code for Algorithm 9--14, not archived coursework.
 * clang -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -g graph_check.c -o /tmp/graph-check
 * /tmp/graph-check
 * Small simple graphs only: 0 <= vertex < n <= 16; |weight| <= 1000.
 * Repeated insertion replaces weight; adjacency lists stay in ascending order.
 */
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { N = 16, MAX_EDGES = N * N };
static const long long INF = 1000000000LL;
typedef struct Node { int vertex, weight; struct Node *next; } Node;
typedef struct { int n; Node *adj[N]; } Graph;
typedef struct { int u, v, w; } Edge;

static Graph graph_new(int n) {
    assert(n >= 0 && n <= N);
    Graph g = {0}; g.n = n; return g;
}
static void add_arc(Graph *g, int u, int v, int w) {
    assert(u >= 0 && u < g->n && v >= 0 && v < g->n);
    assert(w >= -1000 && w <= 1000);
    Node **slot = &g->adj[u];
    while (*slot && (*slot)->vertex < v) slot = &(*slot)->next;
    if (*slot && (*slot)->vertex == v) { (*slot)->weight = w; return; }
    Node *p = malloc(sizeof *p); assert(p);
    *p = (Node){v, w, *slot}; *slot = p;
}
static void add_edge(Graph *g, int u, int v, int w) {
    assert(u != v); add_arc(g, u, v, w); add_arc(g, v, u, w);
}
static bool remove_arc(Graph *g, int u, int v) {
    Node **slot = &g->adj[u];
    while (*slot && (*slot)->vertex < v) slot = &(*slot)->next;
    if (!*slot || (*slot)->vertex != v) return false;
    Node *old = *slot; *slot = old->next; free(old); return true;
}
static void graph_free(Graph *g) {
    for (int u = 0; u < g->n; ++u) {
        Node *p = g->adj[u];
        while (p) { Node *next = p->next; free(p); p = next; }
        g->adj[u] = NULL;
    }
}
static void to_matrix(const Graph *g, bool has[N][N], int weight[N][N]) {
    memset(has, 0, N * N * sizeof has[0][0]);
    memset(weight, 0, N * N * sizeof weight[0][0]);
    for (int u = 0; u < g->n; ++u)
        for (Node *p = g->adj[u]; p; p = p->next) {
            has[u][p->vertex] = true; weight[u][p->vertex] = p->weight;
        }
}
static int edge_array(const Graph *g, Edge edges[MAX_EDGES], bool undirected) {
    int m = 0;
    for (int u = 0; u < g->n; ++u)
        for (Node *p = g->adj[u]; p; p = p->next)
            if (!undirected || u < p->vertex) edges[m++] = (Edge){u, p->vertex, p->weight};
    return m;
}
static void dfs_visit(const Graph *g, int u, bool visited[N], int order[N], int *count) {
    visited[u] = true; order[(*count)++] = u;
    for (Node *p = g->adj[u]; p; p = p->next)
        if (!visited[p->vertex]) dfs_visit(g, p->vertex, visited, order, count);
}
static int bfs(const Graph *g, int start, int order[N], int dist[N], int parent[N]) {
    int queue[N], head = 0, tail = 0;
    for (int v = 0; v < g->n; ++v) { dist[v] = -1; parent[v] = -1; }
    dist[start] = 0; queue[tail++] = start;
    while (head < tail) {
        int u = queue[head]; order[head++] = u;
        for (Node *p = g->adj[u]; p; p = p->next) {
            int v = p->vertex;
            if (dist[v] != -1) continue;
            dist[v] = dist[u] + 1; parent[v] = u;
            queue[tail++] = v;
        }
    }
    return tail;
}
static bool cycle_visit(const Graph *g, int u, int color[N]) {
    color[u] = 1; /* active call */
    for (Node *p = g->adj[u]; p; p = p->next) {
        int v = p->vertex;
        if (color[v] == 1) return true;
        if (color[v] == 0 && cycle_visit(g, v, color)) return true;
    }
    color[u] = 2; /* completed call */
    return false;
}
static bool has_cycle(const Graph *g) {
    int color[N] = {0};
    for (int u = 0; u < g->n; ++u)
        if (color[u] == 0 && cycle_visit(g, u, color)) return true;
    return false;
}
static int topological(const Graph *g, int order[N]) {
    int degree[N] = {0}, queue[N], head = 0, tail = 0;
    for (int u = 0; u < g->n; ++u)
        for (Node *p = g->adj[u]; p; p = p->next) ++degree[p->vertex];
    for (int u = 0; u < g->n; ++u) if (degree[u] == 0) queue[tail++] = u;
    while (head < tail) {
        int u = queue[head]; order[head++] = u;
        for (Node *p = g->adj[u]; p; p = p->next)
            if (--degree[p->vertex] == 0) queue[tail++] = p->vertex;
    }
    return tail; /* count < n means a cycle, possibly with downstream vertices */
}
static void finish_visit(const Graph *g, int u, bool seen[N], int finish[N], int *count) {
    seen[u] = true;
    for (Node *p = g->adj[u]; p; p = p->next)
        if (!seen[p->vertex]) finish_visit(g, p->vertex, seen, finish, count);
    finish[(*count)++] = u;
}
static void label_visit(const Graph *g, int u, int component[N], int id) {
    component[u] = id;
    for (Node *p = g->adj[u]; p; p = p->next)
        if (component[p->vertex] == -1) label_visit(g, p->vertex, component, id);
}
static int scc(const Graph *g, int component[N]) {
    Graph reversed = graph_new(g->n);
    for (int u = 0; u < g->n; ++u)
        for (Node *p = g->adj[u]; p; p = p->next) {
            /* Prepend in O(1); SCC does not require sorted neighbors. */
            Node *r = malloc(sizeof *r); assert(r);
            *r = (Node){u, p->weight, reversed.adj[p->vertex]};
            reversed.adj[p->vertex] = r;
        }
    bool seen[N] = {0}; int finish[N], count = 0, groups = 0;
    for (int u = 0; u < g->n; ++u) {
        component[u] = -1;
        if (!seen[u]) finish_visit(g, u, seen, finish, &count);
    }
    while (count) {
        int u = finish[--count];
        if (component[u] == -1) label_visit(&reversed, u, component, groups++);
    }
    graph_free(&reversed); return groups;
}
static long long prim_forest(const Graph *g, int *components) {
    long long key[N], total = 0; bool used[N] = {0}; *components = 0;
    for (int v = 0; v < g->n; ++v) key[v] = INF;
    for (int step = 0; step < g->n; ++step) {
        int u = -1;
        for (int v = 0; v < g->n; ++v)
            if (!used[v] && (u == -1 || key[v] < key[u])) u = v;
        if (key[u] == INF) { key[u] = 0; ++*components; }
        used[u] = true; total += key[u];
        for (Node *p = g->adj[u]; p; p = p->next)
            if (!used[p->vertex] && p->weight < key[p->vertex]) key[p->vertex] = p->weight;
    }
    return total;
}
static int root(int parent[N], int x) {
    while (parent[x] != x) { parent[x] = parent[parent[x]]; x = parent[x]; }
    return x;
}
static bool join(int parent[N], int size[N], int a, int b) {
    a = root(parent, a); b = root(parent, b); if (a == b) return false;
    if (size[a] < size[b]) { int t = a; a = b; b = t; }
    parent[b] = a; size[a] += size[b]; return true;
}
static int edge_compare(const void *a, const void *b) {
    int x = ((const Edge *)a)->w, y = ((const Edge *)b)->w;
    return (x > y) - (x < y);
}
static long long kruskal_forest(const Graph *g, int *components) {
    Edge edges[MAX_EDGES]; int m = edge_array(g, edges, true);
    qsort(edges, (size_t)m, sizeof edges[0], edge_compare);
    int parent[N], size[N];
    for (int v = 0; v < g->n; ++v) { parent[v] = v; size[v] = 1; }
    long long total = 0; *components = g->n;
    for (int i = 0; i < m; ++i)
        if (join(parent, size, edges[i].u, edges[i].v)) { total += edges[i].w; --*components; }
    return total;
}
static bool dijkstra(const Graph *g, int start, long long dist[N]) {
    for (int u = 0; u < g->n; ++u)
        for (Node *p = g->adj[u]; p; p = p->next) if (p->weight < 0) return false;
    bool done[N] = {0};
    for (int v = 0; v < g->n; ++v) dist[v] = INF;
    dist[start] = 0;
    for (int step = 0; step < g->n; ++step) {
        int u = -1;
        for (int v = 0; v < g->n; ++v)
            if (!done[v] && (u == -1 || dist[v] < dist[u])) u = v;
        if (u == -1 || dist[u] == INF) break;
        done[u] = true;
        for (Node *p = g->adj[u]; p; p = p->next) {
            int v = p->vertex;
            if (!done[v] && dist[u] + p->weight < dist[v]) dist[v] = dist[u] + p->weight;
        }
    }
    return true;
}
static void bellman_ford(const Graph *g, int start, long long dist[N], bool bad[N]) {
    Edge edges[MAX_EDGES]; int m = edge_array(g, edges, false);
    for (int v = 0; v < g->n; ++v) { dist[v] = INF; bad[v] = false; }
    dist[start] = 0;
    for (int pass = 1; pass < g->n; ++pass) {
        bool changed = false;
        for (int i = 0; i < m; ++i) {
            Edge e = edges[i];
            if (dist[e.u] != INF && dist[e.u] + e.w < dist[e.v]) {
                dist[e.v] = dist[e.u] + e.w; changed = true;
            }
        }
        if (!changed) break;
    }
    int queue[N], head = 0, tail = 0;
    for (int i = 0; i < m; ++i) {
        Edge e = edges[i];
        if (dist[e.u] != INF && dist[e.u] + e.w < dist[e.v] && !bad[e.v]) {
            bad[e.v] = true; queue[tail++] = e.v;
        }
    }
    while (head < tail) {
        int u = queue[head++];
        for (Node *p = g->adj[u]; p; p = p->next)
            if (!bad[p->vertex]) { bad[p->vertex] = true; queue[tail++] = p->vertex; }
    }
}
/* Independent matrix oracle: finite distances and negative-cycle reachability. */
static void floyd(const Graph *g, long long distance[N][N]) {
    for (int u = 0; u < g->n; ++u)
        for (int v = 0; v < g->n; ++v) distance[u][v] = u == v ? 0 : INF;
    for (int u = 0; u < g->n; ++u)
        for (Node *p = g->adj[u]; p; p = p->next)
            if (p->weight < distance[u][p->vertex]) distance[u][p->vertex] = p->weight;
    for (int k = 0; k < g->n; ++k)
        for (int u = 0; u < g->n; ++u)
            for (int v = 0; v < g->n; ++v)
                if (distance[u][k] != INF && distance[k][v] != INF &&
                    distance[u][k] + distance[k][v] < distance[u][v])
                    distance[u][v] = distance[u][k] + distance[k][v];
}
static void check_directed(void) {
    for (unsigned mask = 0; mask < (1u << 12); ++mask) {
        Graph g = graph_new(4); int bit = 0;
        for (int u = 0; u < 4; ++u) for (int v = 0; v < 4; ++v)
            if (u != v && (mask & (1u << bit++))) add_arc(&g, u, v, 1);
        long long distance[N][N]; floyd(&g, distance);
        int component[N]; scc(&g, component);
        bool cyclic = false;
        for (int u = 0; u < 4; ++u) for (int v = 0; v < 4; ++v) {
            bool mutual = distance[u][v] != INF && distance[v][u] != INF;
            assert((component[u] == component[v]) == mutual);
            if (u != v && mutual) cyclic = true;
        }
        assert(has_cycle(&g) == cyclic);
        int order[N], count = topological(&g, order);
        assert((count < g.n) == cyclic);
        if (!cyclic) {
            int position[N]; for (int i = 0; i < count; ++i) position[order[i]] = i;
            for (int u = 0; u < 4; ++u) for (Node *p = g.adj[u]; p; p = p->next)
                assert(position[u] < position[p->vertex]);
        }
        for (int s = 0; s < 4; ++s) {
            int dist[N], parent[N]; bfs(&g, s, order, dist, parent);
            bool seen[N] = {0}; int visits = 0; dfs_visit(&g, s, seen, order, &visits);
            for (int v = 0; v < 4; ++v) {
                assert(dist[v] == (distance[s][v] == INF ? -1 : distance[s][v]));
                assert(seen[v] == (distance[s][v] != INF));
                if (parent[v] != -1) assert(dist[v] == dist[parent[v]] + 1);
            }
        }
        graph_free(&g);
    }
}
static long long exhaustive_tree(const Graph *g) {
    Edge edges[MAX_EDGES]; int m = edge_array(g, edges, true); assert(m < 20);
    long long best = INF;
    for (unsigned mask = 0; mask < (1u << m); ++mask) {
        int parent[N], size[N], chosen = 0; long long sum = 0; bool acyclic = true;
        for (int v = 0; v < g->n; ++v) { parent[v] = v; size[v] = 1; }
        for (int i = 0; i < m; ++i) if (mask & (1u << i)) {
            ++chosen; sum += edges[i].w;
            if (!join(parent, size, edges[i].u, edges[i].v)) acyclic = false;
        }
        if (acyclic && chosen == g->n - 1 && sum < best) best = sum;
    }
    return best;
}
static void check_mst(void) {
    for (unsigned code = 0; code < 4096; ++code) {
        Graph g = graph_new(4); unsigned state = code; const int weights[] = {0, -2, 0, 3};
        for (int u = 0; u < 4; ++u) for (int v = u + 1; v < 4; ++v) {
            int x = (int)(state % 4); state /= 4;
            if (x) add_edge(&g, u, v, weights[x]);
        }
        int cp, ck; long long a = prim_forest(&g, &cp), b = kruskal_forest(&g, &ck);
        assert(a == b && cp == ck);
        long long exact = exhaustive_tree(&g);
        assert((exact != INF) == (cp == 1));
        if (cp == 1) assert(a == exact);
        graph_free(&g);
    }
}
static void check_shortest_paths(void) {
    for (unsigned code = 0; code < 4096; ++code) {
        Graph g = graph_new(3); unsigned state = code; bool nonnegative = true;
        const int weights[] = {0, -2, 0, 3};
        for (int u = 0; u < 3; ++u) for (int v = 0; v < 3; ++v) if (u != v) {
            int x = (int)(state % 4); state /= 4;
            if (x) { add_arc(&g, u, v, weights[x]); if (weights[x] < 0) nonnegative = false; }
        }
        long long reference[N][N]; floyd(&g, reference);
        for (int s = 0; s < 3; ++s) {
            long long dist[N], dij[N]; bool bad[N]; bellman_ford(&g, s, dist, bad);
            assert(dijkstra(&g, s, dij) == nonnegative);
            for (int v = 0; v < 3; ++v) {
                bool affected = false;
                for (int k = 0; k < 3; ++k)
                    if (reference[s][k] != INF && reference[k][k] < 0 && reference[k][v] != INF) affected = true;
                assert(bad[v] == affected);
                if (!affected) assert(dist[v] == reference[s][v]);
                if (nonnegative) assert(dij[v] == reference[s][v]);
            }
        }
        graph_free(&g);
    }
    Graph g = graph_new(4); add_arc(&g, 0, 1, 2); add_arc(&g, 2, 3, -2); add_arc(&g, 3, 2, 1);
    long long dist[N]; bool bad[N]; bellman_ford(&g, 0, dist, bad);
    assert(dist[1] == 2 && dist[2] == INF && !bad[2]);
    add_arc(&g, 1, 2, 0); bellman_ford(&g, 0, dist, bad);
    assert(!bad[0] && !bad[1] && bad[2] && bad[3]); graph_free(&g);
}
static unsigned long long dc_calls;
static unsigned long long dp_transitions;
static long long airtel_dc(const int fare[N], const int hotel[N], int s, int d) {
    ++dc_calls; if (s == d) return 0;
    long long best = INF;
    for (int k = s; k < d; ++k) {
        long long value = airtel_dc(fare, hotel, s, k) + (k == s ? 0 : hotel[k]) + fare[d - k];
        if (value < best) best = value;
    }
    return best;
}
static long long airtel_dp(const int fare[N], const int hotel[N], int s, int d, int previous[N]) {
    long long cost[N]; cost[s] = 0; previous[s] = -1;
    dp_transitions = 0;
    for (int i = s + 1; i <= d; ++i) {
        cost[i] = INF; previous[i] = -1;
        for (int k = s; k < i; ++k) {
            ++dp_transitions;
            long long value = cost[k] + (k == s ? 0 : hotel[k]) + fare[i - k];
            if (value < cost[i]) { cost[i] = value; previous[i] = k; }
        }
    }
    return cost[d];
}
static long long airtel_routes(const int fare[N], const int hotel[N], int s, int d) {
    if (s == d) return 0;
    long long best = INF;
    for (unsigned mask = 0; mask < (1u << (d - s - 1)); ++mask) {
        long long total = 0; int last = s;
        for (int i = s + 1; i < d; ++i) if (mask & (1u << (i - s - 1))) {
            total += fare[i - last] + hotel[i]; last = i;
        }
        total += fare[d - last]; if (total < best) best = total;
    }
    return best;
}
static void check_dp(void) {
    for (int seed = 0; seed < 40; ++seed) {
        int fare[N] = {0}, hotel[N] = {0};
        for (int i = 1; i < N; ++i) { fare[i] = 1 + (seed * 7 + i * 11) % 31; hotel[i] = (seed + i * 3) % 8; }
        for (int s = 0; s <= 3; ++s) for (int d = s; d <= 8; ++d) {
            int previous[N]; dc_calls = 0;
            long long expected = airtel_routes(fare, hotel, s, d);
            assert(airtel_dc(fare, hotel, s, d) == expected);
            assert(dc_calls == (1ULL << (d - s)));
            assert(airtel_dp(fare, hotel, s, d, previous) == expected);
            assert(dp_transitions == (unsigned long long)(d - s) * (d - s + 1) / 2);
            long long route_cost = 0;
            for (int v = d; v != s; v = previous[v]) {
                int k = previous[v]; assert(s <= k && k < v);
                route_cost += fare[v - k] + (k == s ? 0 : hotel[k]);
            }
            assert(route_cost == expected);
        }
    }
}
static void demos(void) {
    Graph g = graph_new(5); add_edge(&g, 0, 1, 1); add_edge(&g, 0, 2, 0);
    add_edge(&g, 1, 3, 1); add_edge(&g, 2, 3, 1); /* vertex 4 is isolated */
    bool has[N][N]; int weights[N][N]; to_matrix(&g, has, weights);
    assert(has[0][2] && weights[0][2] == 0 && !has[0][4]);
    add_arc(&g, 0, 2, 5);
    to_matrix(&g, has, weights); assert(has[0][2] && weights[0][2] == 5);
    assert(remove_arc(&g, 0, 2)); assert(!remove_arc(&g, 0, 2));
    to_matrix(&g, has, weights); assert(!has[0][2]);
    add_arc(&g, 0, 2, 0);
    to_matrix(&g, has, weights); assert(has[0][2] && weights[0][2] == 0);
    bool seen[N] = {0}; int order[N], count = 0, dist[N], parent[N];
    dfs_visit(&g, 0, seen, order, &count);
    const int expected_dfs[] = {0, 1, 3, 2};
    assert(count == 4 && memcmp(order, expected_dfs, sizeof expected_dfs) == 0);
    printf("DFS:"); for (int i = 0; i < count; ++i) printf(" %d", order[i]);
    count = bfs(&g, 0, order, dist, parent);
    const int expected_bfs[] = {0, 1, 2, 3}, expected_dist[] = {0, 1, 1, 2, -1};
    assert(count == 4 && memcmp(order, expected_bfs, sizeof expected_bfs) == 0);
    assert(memcmp(dist, expected_dist, sizeof expected_dist) == 0);
    printf("\nBFS:"); for (int i = 0; i < count; ++i) printf(" %d", order[i]);
    printf("\nBFS distances:"); for (int i = 0; i < g.n; ++i) printf(" %d", dist[i]); printf("\n"); graph_free(&g);
    g = graph_new(4); add_edge(&g, 0, 1, 1); add_edge(&g, 0, 2, 4); add_edge(&g, 1, 2, 2); add_edge(&g, 1, 3, 5); add_edge(&g, 2, 3, 3);
    int components; long long mst_weight = prim_forest(&g, &components);
    assert(mst_weight == 6 && components == 1);
    printf("MST total: %lld\n", mst_weight); graph_free(&g);
    g = graph_new(4); add_arc(&g, 0, 1, 2); add_arc(&g, 0, 2, 5); add_arc(&g, 2, 1, -4);
    long long distances[N]; bool bad[N]; bellman_ford(&g, 0, distances, bad);
    assert(distances[0] == 0 && distances[1] == 1 && distances[2] == 5 && distances[3] == INF);
    for (int v = 0; v < g.n; ++v) assert(!bad[v]);
    printf("Bellman-Ford: %lld %lld %lld unreachable\n", distances[0], distances[1], distances[2]); graph_free(&g);
    int fare[N] = {0, 4, 7, 13, 20}, hotel[N] = {99, 2, 1, 3, 99}, previous[N]; dc_calls = 0;
    long long dc = airtel_dc(fare, hotel, 0, 4), dp = airtel_dp(fare, hotel, 0, 4, previous);
    assert(dc == 15 && dp == 15 && dc_calls == 16 && dp_transitions == 10);
    assert(previous[4] == 2 && previous[2] == 0 && previous[0] == -1);
    printf("Airtel 0->4: %lld, DC calls: %llu, DP transitions: %llu, reverse route: 4", dp, dc_calls, dp_transitions);
    for (int v = 4; previous[v] != -1; v = previous[v]) printf(" %d", previous[v]); printf("\n");
}
int main(void) {
    check_directed(); check_mst(); check_shortest_paths(); check_dp(); demos();
    puts("PASS: 4096 directed graphs, 4096 MST graphs, 4096 weighted digraphs, 1200 Airtel queries");
    return 0;
}
