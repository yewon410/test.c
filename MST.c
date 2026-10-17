#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

#define V 50
#define MAXW 100
#define REPEAT 2000
#define SEED 20260707u

typedef struct { int u, v, w; } Edge;
typedef struct Node { int to, w; struct Node *next; } Node;
typedef struct { Edge mst[V - 1]; int n, cost; size_t mem; double ms; } Res;

static int mat[V][V];
static Edge elist[600];
static int ecount;
static unsigned state = SEED;

static int rnd(int n) {
    state ^= state << 13; state ^= state >> 17; state ^= state << 5;
    return (int)(state % (unsigned)n);
}

static void gen(int edges) {
    int order[V], cnt = 0;
    memset(mat, 0, sizeof mat);
    for (int i = 0; i < V; i++) order[i] = i;
    for (int i = V - 1; i > 0; i--) {
        int j = rnd(i + 1), t = order[i]; order[i] = order[j]; order[j] = t;
    }
    for (int i = 1; i < V; i++) {
        int a = order[i], b = order[rnd(i)], w = rnd(MAXW) + 1;
        mat[a][b] = mat[b][a] = w;
        cnt++;
    }
    while (cnt < edges) {
        int a = rnd(V), b = rnd(V);
        if (a == b || mat[a][b]) continue;
        mat[a][b] = mat[b][a] = rnd(MAXW) + 1;
        cnt++;
    }
    ecount = 0;
    for (int i = 0; i < V; i++)
        for (int j = i + 1; j < V; j++)
            if (mat[i][j]) {
                Edge e = {i, j, mat[i][j]};
                elist[ecount++] = e;
            }
}

static int is_connected(void) {
    int seen[V] = {0}, st[V], top = 0, cnt = 1;
    seen[0] = 1; st[top++] = 0;
    while (top) {
        int u = st[--top];
        for (int v = 0; v < V; v++)
            if (mat[u][v] && !seen[v]) { seen[v] = 1; cnt++; st[top++] = v; }
    }
    return cnt == V;
}

static int cmp(const void *a, const void *b) {
    const Edge *x = (const Edge *)a, *y = (const Edge *)b;
    if (x->w != y->w) return x->w - y->w;
    if (x->u != y->u) return x->u - y->u;
    return x->v - y->v;
}
static int find(int *p, int x) {
    while (p[x] != x) { p[x] = p[p[x]]; x = p[x]; }
    return x;
}

static void build_list(Node **head, Node *pool) {
    int k = 0;
    for (int i = 0; i < V; i++) head[i] = NULL;
    for (int i = 0; i < ecount; i++) {
        Edge e = elist[i];
        pool[k].to = e.v; pool[k].w = e.w; pool[k].next = head[e.u]; head[e.u] = &pool[k++];
        pool[k].to = e.u; pool[k].w = e.w; pool[k].next = head[e.v]; head[e.v] = &pool[k++];
    }
}

static void kruskal(Node **head, Res *r) {
    Edge *all = (Edge *)malloc(sizeof(Edge) * ecount);
    int n = 0, parent[V], rank_[V];
    for (int u = 0; u < V; u++)
        for (Node *p = head[u]; p; p = p->next)
            if (u < p->to) { all[n].u = u; all[n].v = p->to; all[n].w = p->w; n++; }
    qsort(all, n, sizeof(Edge), cmp);
    for (int i = 0; i < V; i++) { parent[i] = i; rank_[i] = 0; }
    r->n = 0; r->cost = 0;
    for (int i = 0; i < n && r->n < V - 1; i++) {
        int a = find(parent, all[i].u), b = find(parent, all[i].v);
        if (a == b) continue;
        if (rank_[a] < rank_[b]) { int t = a; a = b; b = t; }
        parent[b] = a;
        if (rank_[a] == rank_[b]) rank_[a]++;
        r->mst[r->n++] = all[i];
        r->cost += all[i].w;
    }
    free(all);
}

static void prim(Res *r) {
    int key[V], from[V], used[V];
    for (int i = 0; i < V; i++) { key[i] = 1 << 30; from[i] = -1; used[i] = 0; }
    key[0] = 0; r->n = 0; r->cost = 0;
    for (int it = 0; it < V; it++) {
        int u = -1;
        for (int i = 0; i < V; i++)
            if (!used[i] && (u < 0 || key[i] < key[u])) u = i;
        used[u] = 1;
        if (from[u] >= 0) {
            r->mst[r->n].u = from[u]; r->mst[r->n].v = u; r->mst[r->n].w = key[u];
            r->n++;
            r->cost += key[u];
        }
        for (int v = 0; v < V; v++)
            if (mat[u][v] && !used[v] && mat[u][v] < key[v]) {
                key[v] = mat[u][v]; from[v] = u;
            }
    }
}

static double now_ms(void) {
#ifdef _WIN32
    LARGE_INTEGER f, c;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&c);
    return (double)c.QuadPart * 1000.0 / (double)f.QuadPart;
#else
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000.0 + t.tv_nsec / 1e6;
#endif
}

static void print_res(int no, const char *kind, const char *rep, const char *alg, int edges, Res *r) {
    printf("\nCase %d : %s Graph - %s - %s\n", no, kind, rep, alg);
    printf("Vertices      : %d\n", V);
    printf("Edges         : %d\n", edges);
    printf("MST Edges     : %d\n", r->n);
    printf("MST Cost      : %d\n", r->cost);
    printf("Memory        : %zu Bytes\n", r->mem);
    printf("Execution Time: %.6f ms\n", r->ms);
    printf("MST: ");
    for (int i = 0; i < r->n; i++) printf("(%d,%d,%d) ", r->mst[i].u, r->mst[i].v, r->mst[i].w);
    printf("\n");
}

static void experiment(const char *kind, int edges, Res *kr, Res *pr) {
    do { gen(edges); } while (!is_connected());
    int m = 0;
    for (int i = 0; i < V; i++) for (int j = i + 1; j < V; j++) if (mat[i][j]) m++;
    if (m != edges) { printf("edge count error\n"); exit(1); }

    Node *head[V];
    Node *pool = (Node *)malloc(sizeof(Node) * 2 * edges);
    build_list(head, pool);

    double t0 = now_ms();
    for (int i = 0; i < REPEAT; i++) kruskal(head, kr);
    kr->ms = (now_ms() - t0) / REPEAT;
    t0 = now_ms();
    for (int i = 0; i < REPEAT; i++) prim(pr);
    pr->ms = (now_ms() - t0) / REPEAT;

    kr->mem = sizeof(Node *) * V + sizeof(Node) * 2 * edges
            + sizeof(Edge) * edges + sizeof(int) * V * 2;
    pr->mem = sizeof(int) * V * V + sizeof(int) * V * 3;
    if (kr->cost != pr->cost || kr->n != V - 1 || pr->n != V - 1) printf("WARNING: MST mismatch (%s)\n", kind);
    free(pool);
}

int main(void) {
    Res sk, sp, dk, dp;
    experiment("Sparse", 100, &sk, &sp);
    experiment("Dense", 600, &dk, &dp);
    print_res(1, "Sparse", "Adjacency List", "Kruskal", 100, &sk);
    print_res(2, "Dense", "Adjacency List", "Kruskal", 600, &dk);
    print_res(3, "Sparse", "Adjacency Matrix", "Prim", 100, &sp);
    print_res(4, "Dense", "Adjacency Matrix", "Prim", 600, &dp);
    printf("\nCheck: Sparse costs %s (%d), Dense costs %s (%d)\n",
           sk.cost == sp.cost ? "match" : "DIFFER", sk.cost,
           dk.cost == dp.cost ? "match" : "DIFFER", dk.cost);
    return 0;
}
