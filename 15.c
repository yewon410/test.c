#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif


typedef struct { unsigned long long comparisons, moves; size_t live, peak; } Stat;
typedef struct Node { int key, count, height; struct Node *left, *right; } Node;
typedef struct { int insert, value; } Operation;

typedef struct { int lo, hi, i, j, pivot; } QuickFrame;
typedef struct { Node *node, *temp; int key, result; } TreeFrame;

static void memory_add(Stat *s, size_t bytes) {
    s->live += bytes;
    if (s->live > s->peak) s->peak = s->live;
}
static void memory_sub(Stat *s, size_t bytes) { s->live -= bytes; }
static void *allocate(size_t bytes) {
    void *p = malloc(bytes);
    if (!p) { fprintf(stderr, "Memory allocation failed\n"); exit(1); }
    return p;
}
static int cmp(int a, int b, Stat *s) {
    s->comparisons++;
    return (a > b) - (a < b);
}
static void swap(int *a, int *b, Stat *s) {
    int temp = *a; *a = *b; *b = temp;
    s->moves += 3;
}
static void quick_sort(int *a, int lo, int hi, Stat *s) {
    int i, j, pivot;
    if (lo >= hi) return;
    memory_add(s, sizeof(QuickFrame));
    i = lo; j = hi;
    pivot = a[lo + (hi - lo) / 2]; s->moves++;
    while (i <= j) {
        while (cmp(a[i], pivot, s) < 0) i++;
        while (cmp(a[j], pivot, s) > 0) j--;
        if (i <= j) { swap(&a[i], &a[j], s); i++; j--; }
    }
    if (lo < j) quick_sort(a, lo, j, s);
    if (i < hi) quick_sort(a, i, hi, s);
    memory_sub(s, sizeof(QuickFrame));
}
static void array_operation(int *a, int *length, Operation op, Stat *s) {
    int i = 0, j, result;
    if (op.insert) {
        while (i < *length && cmp(a[i], op.value, s) < 0) i++;
        for (j = *length; j > i; j--) { a[j] = a[j - 1]; s->moves++; }
        a[i] = op.value; s->moves++; (*length)++;
    } else {
        while (i < *length) {
            result = cmp(a[i], op.value, s);
            if (result >= 0) break;
            i++;
        }
        if (i == *length || result != 0) return;
        for (j = i; j < *length - 1; j++) { a[j] = a[j + 1]; s->moves++; }
        (*length)--;
    }
}
static int height(Node *p) { return p ? p->height : 0; }
static void update(Node *p) {
    int l = height(p->left), r = height(p->right);
    p->height = 1 + (l > r ? l : r);
}
static Node *rotate_right(Node *p) {
    Node *q = p->left;
    p->left = q->right; q->right = p;
    update(p); update(q); return q;
}
static Node *rotate_left(Node *p) {
    Node *q = p->right;
    p->right = q->left; q->left = p;
    update(p); update(q); return q;
}
static Node *balance(Node *p) {
    int b;
    if (!p) return NULL;
    update(p); b = height(p->left) - height(p->right);
    if (b > 1) {
        if (height(p->left->left) < height(p->left->right))
            p->left = rotate_left(p->left);
        return rotate_right(p);
    }
    if (b < -1) {
        if (height(p->right->right) < height(p->right->left))
            p->right = rotate_right(p->right);
        return rotate_left(p);
    }
    return p;
}
static Node *insert(Node *p, int key, Stat *s) {
    int result;
    memory_add(s, sizeof(TreeFrame));
    if (!p) {
        p = (Node *)allocate(sizeof(Node)); memory_add(s, sizeof(Node));
        p->key = key; s->moves++;
        p->count = 1; p->height = 1; p->left = p->right = NULL;
    } else {
        result = cmp(key, p->key, s);
        if (result < 0) p->left = insert(p->left, key, s);
        else if (result > 0) p->right = insert(p->right, key, s);
        else p->count++;
        p = balance(p);
    }
    memory_sub(s, sizeof(TreeFrame)); return p;
}

static Node *remove_min(Node *p, Stat *s) {
    Node *temp;
    memory_add(s, sizeof(TreeFrame));
    if (!p->left) {
        temp = p->right; free(p); memory_sub(s, sizeof(Node));
        memory_sub(s, sizeof(TreeFrame)); return temp;
    }
    p->left = remove_min(p->left, s); p = balance(p);
    memory_sub(s, sizeof(TreeFrame)); return p;
}
static Node *erase(Node *p, int key, Stat *s) {
    Node *temp; int result;
    memory_add(s, sizeof(TreeFrame));
    if (p) {
        result = cmp(key, p->key, s);
        if (result < 0) p->left = erase(p->left, key, s);
        else if (result > 0) p->right = erase(p->right, key, s);
        else if (p->count > 1) p->count--;
        else if (!p->left || !p->right) {
            temp = p->left ? p->left : p->right;
            free(p); memory_sub(s, sizeof(Node)); p = temp;
        } else {
            temp = p->right;
            while (temp->left) temp = temp->left;
            p->key = temp->key; s->moves++;
            p->count = temp->count;
            p->right = remove_min(p->right, s);
        }
        p = balance(p);
    }
    memory_sub(s, sizeof(TreeFrame)); return p;
}

static volatile unsigned long long traversal_sum;
static void inorder(Node *p, Stat *s) {
    int i;
    if (!p) return;
    memory_add(s, sizeof(TreeFrame));
    inorder(p->left, s);
    for (i = 0; i < p->count; i++) traversal_sum += (unsigned int)p->key;
    inorder(p->right, s);
    memory_sub(s, sizeof(TreeFrame));
}

static int verify(Node *p, const int *a, int length, int *index) {
    int i, l, r;
    if (!p) return 1;
    if (!verify(p->left, a, length, index)) return 0;
    for (i = 0; i < p->count; i++) {
        if (*index >= length || a[*index] != p->key) return 0;
        (*index)++;
    }
    l = height(p->left); r = height(p->right);
    if (abs(l-r) > 1 || p->height != 1 + (l > r ? l : r)) return 0;
    return verify(p->right, a, length, index);
}
static void destroy(Node *p) {
    if (p) { destroy(p->left); destroy(p->right); free(p); }
}
static double now_seconds(void) {
#ifdef _WIN32
    LARGE_INTEGER value, frequency;
    QueryPerformanceFrequency(&frequency); QueryPerformanceCounter(&value);
    return (double)value.QuadPart / (double)frequency.QuadPart;
#else
    return (double)clock() / CLOCKS_PER_SEC;
#endif
}

static uint32_t random_state = 2026u;
static uint32_t next_random(void) {
    random_state ^= random_state << 13;
    random_state ^= random_state >> 17;
    random_state ^= random_state << 5;
    return random_state;
}
int main(void) {
    int sizes[] = {1000, 2000, 4000, 8000, 16000, 20000};
    int test;
    printf("Seed: 2026 | Value range: 0 .. 2*n-1\n");
    printf("Memory Usage: estimated algorithm-owned peak bytes\n\n");
    for (test = 0; test < 6; test++) {
        int n = sizes[test], operations = n / 5;
        int capacity = n + operations, length = n, index = 0, i, valid;
        int *source = (int *)allocate((size_t)n * sizeof(int));
        int *array = (int *)allocate((size_t)capacity * sizeof(int));
        Operation *ops = (Operation *)allocate((size_t)operations * sizeof(Operation));
        Node *root = NULL;
        Stat q = {0}, t = {0};
        double start, quick_ms, tree_ms;
        for (i = 0; i < n; i++) {
            source[i] = (int)(next_random() % (uint32_t)(2*n));
            array[i] = source[i];
        }
        for (i = 0; i < operations; i++) {
            ops[i].insert = (next_random() % 2u == 0);
            ops[i].value = (int)(next_random() % (uint32_t)(2*n));
        }
        memory_add(&q, (size_t)capacity * sizeof(int));
        start = now_seconds();
        quick_sort(array, 0, n-1, &q);
        for (i = 0; i < operations; i++) array_operation(array, &length, ops[i], &q);
        quick_ms = (now_seconds() - start) * 1000.0;
        start = now_seconds();
        for (i = 0; i < n; i++) root = insert(root, source[i], &t);
        traversal_sum = 0; inorder(root, &t);
        for (i = 0; i < operations; i++) {
            if (ops[i].insert) root = insert(root, ops[i].value, &t);
            else root = erase(root, ops[i].value, &t);
        }
        tree_ms = (now_seconds() - start) * 1000.0;
        valid = verify(root, array, length, &index) && index == length;
        for (i = 1; i < length; i++) if (array[i-1] > array[i]) valid = 0;
        printf("Data Size : %d\nOperations: %d\n\n", n, operations);
        printf("                  Quick Sort        AVL Tree\n");
        printf("------------------------------------------------------\n");
        printf("Total Time        %.3f ms          %.3f ms\n", quick_ms, tree_ms);
        printf("Comparisons       %llu             %llu\n", q.comparisons, t.comparisons);
        printf("Data Movements    %llu             %llu\n", q.moves, t.moves);
        printf("Memory Usage      %zu Bytes        %zu Bytes\n", q.peak, t.peak);
        printf("Final Data Count  %d               %d\n", length, index);
        printf("Validation        %s\n\n", valid ? "PASS" : "FAIL");
        destroy(root); free(ops); free(array); free(source);
        if (!valid) return 1;
    }
    return 0;
}
