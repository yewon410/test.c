#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 10000
#define RUNS 100
#define K 5

typedef struct { int key1, key2, order; } Data;
typedef struct { long long compare, move; } Count;

const char *names[K] = {
    "Selection", "Bubble", "Bubble Early", "Insertion >", "Insertion >="
};

int randomNumber(int max) {
    unsigned long long b = (unsigned long long)RAND_MAX + 1;
    unsigned long long range = b * b, limit = range - range % (max + 1ULL), v;
    do { v = (unsigned long long)rand() * b + rand(); } while (v >= limit);
    return (int)(v % (max + 1ULL));
}

void makeData(Data a[]) {
    for (int i = 0; i < N; i++) {
        a[i].key1 = randomNumber(1000000);
        a[i].key2 = randomNumber(1000000);
        a[i].order = i;
    }
}

Count selection(Data a[], int n) {
    Count c = {0, 0};
    for (int i = 0; i < n - 1; i++) {
        int min = i;
        for (int j = i + 1; j < n; j++) {
            c.compare++;
            if (a[j].key1 < a[min].key1) min = j;
        }
        if (min != i) {
            Data t = a[i]; a[i] = a[min]; a[min] = t;
            c.move += 3;
        }
    }
    return c;
}

Count bubble(Data a[], int n, int early) {
    Count c = {0, 0};
    for (int i = 0; i < n - 1; i++) {
        int changed = 0;
        for (int j = 0; j < n - 1 - i; j++) {
            c.compare++;
            if (a[j].key1 > a[j + 1].key1) {
                Data t = a[j]; a[j] = a[j + 1]; a[j + 1] = t;
                c.move += 3; changed = 1;
            }
        }
        if (early && !changed) break;
    }
    return c;
}

Count insertion(Data a[], int n, int equal) {
    Count c = {0, 0};
    for (int i = 1; i < n; i++) {
        Data t = a[i];
        int j = i - 1;
        c.move++;
        while (j >= 0) {
            c.compare++;
            if (!(equal ? a[j].key1 >= t.key1 : a[j].key1 > t.key1)) break;
            a[j + 1] = a[j]; c.move++; j--;
        }
        a[j + 1] = t; c.move++;
    }
    return c;
}

Count runSort(Data a[], int n, int type) {
    if (type == 0) return selection(a, n);
    if (type < 3) return bubble(a, n, type == 2);
    return insertion(a, n, type == 4);
}

int isSorted(Data a[], int n) {
    for (int i = 0; i < n - 1; i++)
        if (a[i].key1 > a[i + 1].key1) return 0;
    return 1;
}

int isStable(Data a[], int n) {
    for (int i = 0; i < n - 1; i++)
        if (a[i].key1 == a[i + 1].key1 && a[i].order > a[i + 1].order)
            return 0;
    return 1;
}

void stabilityTest(void) {
    Data src[] = {{527,100,0}, {527,200,1}, {125,300,2}, {892,400,3}}, a[4];
    puts("\n=== Stability ===");
    for (int k = 0; k < K; k++) {
        memcpy(a, src, sizeof a);
        runSort(a, 4, k);
        printf("%-13s ", names[k]);
        for (int i = 0; i < 4; i++) printf("(%d,%d) ", a[i].key1, a[i].key2);
        printf("Sorted=%d Stable=%d\n", isSorted(a, 4), isStable(a, 4));
    }
}

void randomTest(void) {
    static Data src[N], a[N];
    Count total[K] = {{0, 0}};
    int allSorted = 1;
    puts("\n=== Random 100 Trials ===");
    for (int r = 0; r < RUNS; r++) {
        makeData(src);
        for (int k = 0; k < K; k++) {
            memcpy(a, src, sizeof a);
            Count c = runSort(a, N, k);
            total[k].compare += c.compare;
            total[k].move += c.move;
            allSorted &= isSorted(a, N);
            printf("%3d %-13s Compare=%lld Move=%lld\n",
                   r + 1, names[k], c.compare, c.move);
        }
    }
    printf("\nAll sorted=%d\n", allSorted);
    for (int k = 0; k < K; k++)
        printf("%-13s TotalCompare=%lld AvgCompare=%.2f TotalMove=%lld AvgMove=%.2f\n",
               names[k], total[k].compare, (double)total[k].compare / RUNS,
               total[k].move, (double)total[k].move / RUNS);
}

int compareKey(const void *x, const void *y) {
    const Data *a = (const Data *)x;
    const Data *b = (const Data *)y;
    return (a->key1 > b->key1) - (a->key1 < b->key1);
}

void testState(Data src[], const char *name) {
    static Data a[N];
    printf("\n=== %s ===\n", name);
    for (int k = 0; k < K; k++) {
        memcpy(a, src, sizeof a);
        Count c = runSort(a, N, k);
        printf("%-13s Compare=%lld Move=%lld Sorted=%d\n",
               names[k], c.compare, c.move, isSorted(a, N));
    }
}

void stateTest(void) {
    static Data sorted[N], reverse[N], partial[N];
    makeData(sorted);
    qsort(sorted, N, sizeof(Data), compareKey);
    for (int i = 0; i < N; i++) reverse[i] = sorted[N - 1 - i];
    memcpy(partial, sorted, sizeof partial);
    for (int i = 0; i < 500; i++) {
        int x = randomNumber(N - 1), y;
        do { y = randomNumber(N - 1); } while (x == y);
        Data t = partial[x]; partial[x] = partial[y]; partial[y] = t;
    }
    testState(sorted, "Sorted");
    testState(reverse, "Reverse");
    testState(partial, "Partial: 500 swaps");
}

int main(void) {
    srand(2024);
    stabilityTest();
    randomTest();
    stateTest();
    return 0;
}
