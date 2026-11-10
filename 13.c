#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>

#define RUNS 5

static uint64_t comparisons;
static uint32_t pivot_state;
static size_t active_memory, peak_memory;

typedef struct {
    int *a, *temp;
    int left, right, mid, i, j, k;
} MergeFrame;

typedef struct {
    int *a;
    int left, right, mode, pivot_index, pivot, i, j, t, mid, x, y, z;
} QuickFrame;

typedef struct {
    double ms;
    uint64_t comparisons;
    size_t memory;
} Result;

static uint32_t next_random(uint32_t *state) {
    *state ^= *state << 13;
    *state ^= *state >> 17;
    *state ^= *state << 5;
    return *state;
}

static void enter_memory(size_t bytes) {
    active_memory += bytes;
    if (active_memory > peak_memory) peak_memory = active_memory;
}

static void merge_sort(int *a, int *temp, int left, int right) {
    MergeFrame f;
    enter_memory(sizeof f);
    f.a = a;
    f.temp = temp;
    f.left = left;
    f.right = right;
    if (f.left < f.right) {
        f.mid = f.left + (f.right - f.left) / 2;
        merge_sort(f.a, f.temp, f.left, f.mid);
        merge_sort(f.a, f.temp, f.mid + 1, f.right);
        f.i = f.left;
        f.j = f.mid + 1;
        f.k = f.left;
        while (f.i <= f.mid && f.j <= f.right) {
            comparisons++;
            if (f.a[f.i] <= f.a[f.j]) f.temp[f.k++] = f.a[f.i++];
            else f.temp[f.k++] = f.a[f.j++];
        }
        while (f.i <= f.mid) f.temp[f.k++] = f.a[f.i++];
        while (f.j <= f.right) f.temp[f.k++] = f.a[f.j++];
        for (f.k = f.left; f.k <= f.right; f.k++) f.a[f.k] = f.temp[f.k];
    }
    active_memory -= sizeof f;
}

static void quick_sort(int *a, int left, int right, int mode) {
    QuickFrame f;
    enter_memory(sizeof f);
    f.a = a;
    f.left = left;
    f.right = right;
    f.mode = mode;
    while (f.left < f.right) {
        f.pivot_index = f.left;
        if (f.mode == 1) {
            f.pivot_index = f.left + (int)(next_random(&pivot_state) %
                (uint32_t)(f.right - f.left + 1));
        } else if (f.mode == 2 && f.right - f.left >= 2) {
            f.mid = f.left + (f.right - f.left) / 2;
            f.x = f.left;
            f.y = f.mid;
            f.z = f.right;
            comparisons++;
            if (f.a[f.x] > f.a[f.y]) {
                f.t = f.x; f.x = f.y; f.y = f.t;
            }
            comparisons++;
            if (f.a[f.y] > f.a[f.z]) {
                f.t = f.y; f.y = f.z; f.z = f.t;
            }
            comparisons++;
            if (f.a[f.x] > f.a[f.y]) {
                f.t = f.x; f.x = f.y; f.y = f.t;
            }
            f.pivot_index = f.y;
        }
        f.t = f.a[f.pivot_index];
        f.a[f.pivot_index] = f.a[f.right];
        f.a[f.right] = f.t;
        f.pivot = f.a[f.right];
        f.i = f.left;
        for (f.j = f.left; f.j < f.right; f.j++) {
            comparisons++;
            if (f.a[f.j] < f.pivot) {
                f.t = f.a[f.i]; f.a[f.i] = f.a[f.j]; f.a[f.j] = f.t;
                f.i++;
            }
        }
        f.t = f.a[f.i]; f.a[f.i] = f.a[f.right]; f.a[f.right] = f.t;
        if (f.i - f.left < f.right - f.i) {
            if (f.left < f.i - 1) quick_sort(f.a, f.left, f.i - 1, f.mode);
            f.left = f.i + 1;
        } else {
            if (f.i + 1 < f.right) quick_sort(f.a, f.i + 1, f.right, f.mode);
            f.right = f.i - 1;
        }
    }
    active_memory -= sizeof f;
}

static int compare_ints(const void *p, const void *q) {
    int a = *(const int *)p, b = *(const int *)q;
    return (a > b) - (a < b);
}

int main(void) {
    const int sizes[] = {1000, 2000, 4000, 8000, 16000, 20000};
    const char *names[] = {"Merge Sort", "Quick (First)", "Quick (Random)", "Quick (Median3)"};
    FILE *csv = fopen("results.csv", "w");
    int s, alg, run, i;
    if (!csv) { perror("results.csv"); return EXIT_FAILURE; }
    fprintf(csv, "n,algorithm,comparisons,time_ms,memory_bytes\n");
    printf("Time: median of %d runs, clock(), milliseconds\n", RUNS);
    printf("Memory: calculated auxiliary storage, not physical stack measurement\n");
    printf("sizeof(int)=%zu, MergeFrame=%zu, QuickFrame=%zu\n\n",
           sizeof(int), sizeof(MergeFrame), sizeof(QuickFrame));
    for (s = 0; s < 6; s++) {
        int n = sizes[s];
        int *original = (int *)malloc((size_t)n * sizeof(int));
        int *work = (int *)malloc((size_t)n * sizeof(int));
        int *expected = (int *)malloc((size_t)n * sizeof(int));
        uint32_t data_state = 20260301u + (uint32_t)n;
        if (!original || !work || !expected) {
            fprintf(stderr, "Allocation failed\n");
            free(original); free(work); free(expected); fclose(csv);
            return EXIT_FAILURE;
        }
        for (i = 0; i < n; i++) original[i] = (int)(next_random(&data_state) % 100001u);
        memcpy(expected, original, (size_t)n * sizeof(int));
        qsort(expected, (size_t)n, sizeof(int), compare_ints);
        printf("Data Size : %d\n\n", n);
        printf("%-17s %14s %12s %16s\n", "Algorithm", "Comparisons", "Time(ms)", "Memory(Bytes)");
        printf("---------------------------------------------------------------\n");
        for (alg = 0; alg < 4; alg++) {
            Result results[RUNS], chosen;
            for (run = 0; run < RUNS; run++) {
                clock_t begin, end;
                int *temp = NULL;
                memcpy(work, original, (size_t)n * sizeof(int));
                comparisons = 0;
                active_memory = peak_memory = 0;
                pivot_state = 987654321u + (uint32_t)n;
                begin = clock();
                if (alg == 0) {
                    temp = (int *)malloc((size_t)n * sizeof(int));
                    if (!temp) {
                        fprintf(stderr, "Allocation failed\n");
                        free(original); free(work); free(expected); fclose(csv);
                        return EXIT_FAILURE;
                    }
                    enter_memory((size_t)n * sizeof(int));
                    merge_sort(work, temp, 0, n - 1);
                    free(temp);
                    active_memory -= (size_t)n * sizeof(int);
                } else quick_sort(work, 0, n - 1, alg - 1);
                end = clock();
                if (begin == (clock_t)-1 || end == (clock_t)-1) {
                    fprintf(stderr, "Timer unavailable\n"); return EXIT_FAILURE;
                }
                if (memcmp(work, expected, (size_t)n * sizeof(int)) != 0) {
                    fprintf(stderr, "Sorting verification failed\n"); return EXIT_FAILURE;
                }
                results[run].ms = 1000.0 * (double)(end - begin) / CLOCKS_PER_SEC;
                results[run].comparisons = comparisons;
                results[run].memory = peak_memory;
            }
            for (run = 1; run < RUNS; run++) {
                Result item = results[run];
                int j = run - 1;
                while (j >= 0 && results[j].ms > item.ms) {
                    results[j + 1] = results[j]; j--;
                }
                results[j + 1] = item;
            }
            chosen = results[RUNS / 2];
            printf("%-17s %14" PRIu64 " %12.3f %16zu\n",
                   names[alg], chosen.comparisons, chosen.ms, chosen.memory);
            fprintf(csv, "%d,%s,%" PRIu64 ",%.3f,%zu\n",
                    n, names[alg], chosen.comparisons, chosen.ms, chosen.memory);
        }
        printf("Verification: PASS (all 4 algorithms, all 5 runs)\n\n");
        free(original); free(work); free(expected);
    }
    fclose(csv);
    return EXIT_SUCCESS;
}
