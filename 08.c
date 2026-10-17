#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define V 10
#define E 20
#define INF 1000000

static int w[V][V];
static int dist[V][V], nxt[V][V];

static int rnd(int n) { return rand() % n; }

static void add_edge(int u, int v) {
    w[u][v] = w[v][u] = rnd(100) + 1;
}

static int connected(void) {
    int seen[V] = {0}, stack[V], top = 0, cnt = 1;
    seen[0] = 1; stack[top++] = 0;
    while (top) {
        int u = stack[--top];
        for (int v = 0; v < V; v++)
            if (w[u][v] && !seen[v]) { seen[v] = 1; cnt++; stack[top++] = v; }
    }
    return cnt == V;
}

static void generate(void) {
    int order[V], count = 0;
    for (int i = 0; i < V; i++) order[i] = i;
    for (int i = V - 1; i > 0; i--) {
        int j = rnd(i + 1), t = order[i]; order[i] = order[j]; order[j] = t;
    }
    for (int i = 1; i < V; i++) {
        add_edge(order[i], order[rnd(i)]);
        count++;
    }
    while (count < E) {
        int u = rnd(V), v = rnd(V);
        if (u == v || w[u][v]) continue;
        add_edge(u, v);
        count++;
    }
}

static void floyd(void) {
    for (int i = 0; i < V; i++)
        for (int j = 0; j < V; j++) {
            dist[i][j] = (i == j) ? 0 : (w[i][j] ? w[i][j] : INF);
            nxt[i][j] = w[i][j] ? j : -1;
        }
    for (int k = 0; k < V; k++)
        for (int i = 0; i < V; i++)
            for (int j = 0; j < V; j++)
                if (dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    nxt[i][j] = nxt[i][k];
                }
}

int main(void) {
    srand((unsigned)time(NULL));
    generate();

    int edges = 0, selfloop = 0;
    for (int i = 0; i < V; i++) {
        if (w[i][i]) selfloop = 1;
        for (int j = i + 1; j < V; j++) if (w[i][j]) edges++;
    }

    printf("Graph\nVertices : %d\nEdges    : %d\n\nEdges\n", V, edges);
    for (int i = 0; i < V; i++)
        for (int j = i + 1; j < V; j++)
            if (w[i][j]) printf("(%d, %d) Weight: %d\n", i, j, w[i][j]);

    printf("\nChecks\n");
    printf("Vertex count is 10   : %s\n", V == 10 ? "yes" : "no");
    printf("Edge count is 20     : %s\n", edges == 20 ? "yes" : "no");
    printf("No self-loop         : %s\n", selfloop ? "no" : "yes");
    printf("Connected            : %s\n", connected() ? "yes" : "no");

    floyd();

    printf("\nShortest Paths\n");
    int pairs = 0;
    for (int u = 0; u < V; u++)
        for (int v = u + 1; v < V; v++) {
            printf("\n%d -> %d\nPath     : %d", u, v, u);
            int sum = 0;
            for (int c = u; c != v; ) {
                int n = nxt[c][v];
                sum += w[c][n];
                printf(" -> %d", n);
                c = n;
            }
            printf("\nDistance : %d", dist[u][v]);
            if (sum != dist[u][v]) printf("  (MISMATCH: %d)", sum);
            printf("\n");
            pairs++;
        }
    printf("\nTotal vertex pairs : %d\n", pairs);
    return 0;
}
