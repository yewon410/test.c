#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define V 100
#define P (V * (V - 1) / 2)

typedef struct { int u, v; } E;
typedef struct N { int v; struct N *next; } N;
typedef struct { int vertices, edges; int m[V][V]; } MG;
typedef struct { int vertices, edges; N *head[V]; } LG;

static MG mg;
static LG lg;
static unsigned s = 20260707;

static unsigned rnd(unsigned n) { s ^= s << 13; s ^= s >> 17; s ^= s << 5; return s % n; }
static void flip(E *e) { if (rnd(2)) { int t = e->u; e->u = e->v; e->v = t; } }
static void push(int u, int v) { N *a = malloc(sizeof *a); a->v = v; a->next = lg.head[u]; lg.head[u] = a; }

static int m_has(int u, int v, long *c) { ++*c; return mg.m[u][v]; }
static int m_ins(E e, long *c) {
    if (m_has(e.u, e.v, c)) return 0;
    mg.m[e.u][e.v] = mg.m[e.v][e.u] = 1; mg.edges++; return 1;
}
static int m_del(E e, long *c) {
    if (!m_has(e.u, e.v, c)) return 0;
    mg.m[e.u][e.v] = mg.m[e.v][e.u] = 0; mg.edges--; return 1;
}

static int l_has(int u, int v, long *c) {
    for (N *p = lg.head[u]; p; p = p->next) { ++*c; if (p->v == v) return 1; }
    return 0;
}
static int l_ins(E e, long *c) {
    if (l_has(e.u, e.v, c) || l_has(e.v, e.u, c)) return 0;
    push(e.u, e.v); push(e.v, e.u); lg.edges++; return 1;
}
static int l_rem(int u, int v, long *c) {
    for (N **h = &lg.head[u]; *h; h = &(*h)->next) {
        ++*c;
        if ((*h)->v == v) { N *t = *h; *h = t->next; free(t); return 1; }
    }
    return 0;
}
static int l_del(E e, long *c) {
    if (!l_rem(e.u, e.v, c)) return 0;
    l_rem(e.v, e.u, c); lg.edges--; return 1;
}

static void run(const char *name, int edges, int no) {
    E p[P]; int k = 0;
    memset(&mg, 0, sizeof mg); memset(&lg, 0, sizeof lg);
    mg.vertices = lg.vertices = V;
    for (int u = 0; u < V; u++) for (int v = u + 1; v < V; v++) p[k++] = (E){u, v};
    for (int i = P - 1; i > 0; i--) { int j = rnd(i + 1); E t = p[i]; p[i] = p[j]; p[j] = t; }
    for (int i = 0; i < edges; i++) {
        int u = p[i].u, v = p[i].v;
        mg.m[u][v] = mg.m[v][u] = 1; push(u, v); push(v, u);
    }
    mg.edges = lg.edges = edges;

    long mem[2] = {sizeof mg, sizeof lg}, c[2][4] = {{0}};
    for (int u = 0; u < V; u++) for (N *q = lg.head[u]; q; q = q->next) mem[1] += sizeof(N);

    E a = p[edges + rnd(P - edges)], d = p[rnd(edges)], q = p[rnd(P)];
    flip(&a); flip(&d); flip(&q);
    int x = rnd(V);
    printf("\n=== %s: V=%d, E=%d ===\nInsert (%d,%d), delete (%d,%d), query (%d,%d), neighbors of %d\n",
           name, V, edges, a.u, a.v, d.u, d.v, q.u, q.v, x);

    m_ins(a, &c[0][0]); l_ins(a, &c[1][0]);
    m_del(d, &c[0][1]); l_del(d, &c[1][1]);
    int mf = m_has(q.u, q.v, &c[0][2]);
    int lf = l_has(q.u, q.v, &c[1][2]);
    printf("Connected: matrix=%s, list=%s\n", mf ? "yes" : "no", lf ? "yes" : "no");

    printf("Matrix neighbors:");
    for (int v = 0; v < V; v++) { c[0][3]++; if (mg.m[x][v]) printf(" %d", v); }
    printf("\nList neighbors  :");
    for (N *n = lg.head[x]; n; n = n->next) { c[1][3]++; printf(" %d", n->v); }
    printf("\n");

    for (int r = 0; r < 2; r++) {
        printf("\nCase %d : %s Graph - %s\n", no + r, name, r ? "Adjacency List" : "Adjacency Matrix");
        printf("Memory                         : %ld Bytes\n", mem[r]);
        printf("Edge insertion comparisons     : %ld\n", c[r][0]);
        printf("Edge deletion comparisons      : %ld\n", c[r][1]);
        printf("Connectivity check comparisons : %ld\n", c[r][2]);
        printf("Adjacent vertices comparisons  : %ld\n", c[r][3]);
    }
    for (int u = 0; u < V; u++) for (N *n = lg.head[u], *t; n; n = t) { t = n->next; free(n); }
}

int main(void) {
    run("Sparse", 100, 1);
    run("Dense", 4000, 3);
    return 0;
}
