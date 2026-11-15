#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <time.h>

#define REPEATS 7
#define ALGORITHMS 5

typedef struct {
    uint64_t comparisons;
    size_t live;
    size_t peak;
    uint64_t rng;
} Stats;

typedef void (*SortFunction)(int *, size_t, Stats *);

static void enter(Stats *s, size_t bytes) {
    s->live += bytes;
    if (s->live > s->peak) s->peak = s->live;
}

static void leave(Stats *s, size_t bytes) {
    s->live -= bytes;
}

static uint64_t random64(uint64_t *state) {
    uint64_t x = *state;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    *state = x;
    return x * UINT64_C(2685821657736338717);
}

static int less(int a, int b, Stats *s) {
    s->comparisons++;
    return a < b;
}

static void swap(int *a, int *b, Stats *s) {
    int temp = *a;
    enter(s, sizeof temp);
    *a = *b;
    *b = temp;
    leave(s, sizeof temp);
}

static void sift_down(int *a, size_t n, size_t root, Stats *s) {
    struct { size_t n, root, child; int temp; } f;
    enter(s, sizeof f);
    f.n = n;
    f.root = root;
    f.temp = a[root];
    while (f.root < f.n / 2) {
        f.child = 2 * f.root + 1;
        if (f.child + 1 < f.n && less(a[f.child], a[f.child + 1], s))
            f.child++;
        if (!less(f.temp, a[f.child], s)) break;
        a[f.root] = a[f.child];
        f.root = f.child;
    }
    a[f.root] = f.temp;
    leave(s, sizeof f);
}

static void heap_sort(int *a, size_t n, Stats *s) {
    struct { size_t i; } f;
    enter(s, sizeof f);
    for (f.i = n / 2; f.i > 0; f.i--) sift_down(a, n, f.i - 1, s);
    for (f.i = n; f.i > 1; f.i--) {
        swap(&a[0], &a[f.i - 1], s);
        sift_down(a, f.i - 1, 0, s);
    }
    leave(s, sizeof f);
}

static void merge_recursive(int *a, int *temp, size_t lo, size_t hi, Stats *s) {
    struct { size_t lo, hi, mid, i, j, k; } f;
    enter(s, sizeof f);
    f.lo = lo;
    f.hi = hi;
    if (hi - lo > 1) {
        f.mid = lo + (hi - lo) / 2;
        merge_recursive(a, temp, lo, f.mid, s);
        merge_recursive(a, temp, f.mid, hi, s);
        f.i = lo;
        f.j = f.mid;
        f.k = lo;
        while (f.i < f.mid && f.j < hi) {
            if (less(a[f.j], a[f.i], s)) temp[f.k++] = a[f.j++];
            else temp[f.k++] = a[f.i++];
        }
        while (f.i < f.mid) temp[f.k++] = a[f.i++];
        while (f.j < hi) temp[f.k++] = a[f.j++];
        for (f.k = lo; f.k < hi; f.k++) a[f.k] = temp[f.k];
    }
    leave(s, sizeof f);
}

static void merge_sort(int *a, size_t n, Stats *s) {
    struct { int *temp; } f;
    enter(s, sizeof f);
    f.temp = (int *)malloc(n * sizeof *f.temp);
    if (!f.temp) { fprintf(stderr, "Allocation failed\n"); exit(EXIT_FAILURE); }
    enter(s, n * sizeof *f.temp);
    merge_recursive(a, f.temp, 0, n, s);
    leave(s, n * sizeof *f.temp);
    free(f.temp);
    leave(s, sizeof f);
}

static size_t median_index(int *a, size_t lo, size_t hi, Stats *s) {
    struct { size_t mid; } f;
    enter(s, sizeof f);
    f.mid = lo + (hi - lo) / 2;
    if (less(a[lo], a[f.mid], s)) {
        if (less(a[f.mid], a[hi], s)) { leave(s, sizeof f); return f.mid; }
        if (less(a[lo], a[hi], s)) { leave(s, sizeof f); return hi; }
        leave(s, sizeof f); return lo;
    }
    if (less(a[lo], a[hi], s)) { leave(s, sizeof f); return lo; }
    if (less(a[f.mid], a[hi], s)) { leave(s, sizeof f); return hi; }
    leave(s, sizeof f); return f.mid;
}

static void quick_recursive(int *a, size_t lo, size_t hi, int mode, Stats *s) {
    struct { size_t lo, hi, pivot_index, lt, i, gt; int pivot, mode; } f;
    enter(s, sizeof f);
    f.lo = lo;
    f.hi = hi;
    f.mode = mode;
    if (hi - lo > 1) {
        f.pivot_index = lo;
        if (mode == 1) {
            enter(s, sizeof(uint64_t));
            f.pivot_index = lo + (size_t)(random64(&s->rng) % (hi - lo));
            leave(s, sizeof(uint64_t));
        }
        else if (mode == 2) f.pivot_index = median_index(a, lo, hi - 1, s);
        f.pivot = a[f.pivot_index];
        f.lt = lo;
        f.i = lo;
        f.gt = hi;
        while (f.i < f.gt) {
            if (less(a[f.i], f.pivot, s)) swap(&a[f.lt++], &a[f.i++], s);
            else if (less(f.pivot, a[f.i], s)) swap(&a[f.i], &a[--f.gt], s);
            else f.i++;
        }
        quick_recursive(a, lo, f.lt, mode, s);
        quick_recursive(a, f.gt, hi, mode, s);
    }
    leave(s, sizeof f);
}

static void quick_first(int *a, size_t n, Stats *s) { quick_recursive(a, 0, n, 0, s); }
static void quick_random(int *a, size_t n, Stats *s) { quick_recursive(a, 0, n, 1, s); }
static void quick_median(int *a, size_t n, Stats *s) { quick_recursive(a, 0, n, 2, s); }

static int reference_compare(const void *x, const void *y) {
    int a = *(const int *)x, b = *(const int *)y;
    return (a > b) - (a < b);
}

static double median_time(double *values) {
    for (int i = 1; i < REPEATS; i++) {
        double v = values[i];
        int j = i;
        while (j > 0 && values[j - 1] > v) { values[j] = values[j - 1]; j--; }
        values[j] = v;
    }
    return values[REPEATS / 2];
}

int main(void) {
    const size_t sizes[] = {1000, 2000, 4000, 8000, 16000, 20000};
    const char *names[] = {"Heap Sort", "Merge Sort", "Quick (First)", "Quick (Random)", "Quick (Median3)"};
    SortFunction sorts[] = {heap_sort, merge_sort, quick_first, quick_random, quick_median};
    uint64_t data_rng = UINT64_C(20261115);
    FILE *csv = fopen("results.csv", "w");
    if (!csv) { perror("results.csv"); return EXIT_FAILURE; }
    fprintf(csv, "n,algorithm,comparisons,time_ms,memory_bytes\n");
    for (size_t z = 0; z < sizeof sizes / sizeof sizes[0]; z++) {
        size_t n = sizes[z];
        int *original = (int *)malloc(n * sizeof *original);
        int *work = (int *)malloc(n * sizeof *work);
        int *reference = (int *)malloc(n * sizeof *reference);
        uint64_t comparisons[ALGORITHMS] = {0};
        size_t memories[ALGORITHMS] = {0};
        double times[ALGORITHMS][REPEATS];
        if (!original || !work || !reference) {
            fprintf(stderr, "Allocation failed\n");
            free(original); free(work); free(reference); fclose(csv);
            return EXIT_FAILURE;
        }
        for (size_t i = 0; i < n; i++) original[i] = (int)(random64(&data_rng) % UINT64_C(1000001));
        memcpy(reference, original, n * sizeof *reference);
        qsort(reference, n, sizeof *reference, reference_compare);
        for (int r = -1; r < REPEATS; r++) {
            for (int order = 0; order < ALGORITHMS; order++) {
                int k = (order + r + 1) % ALGORITHMS;
                Stats s = {0, 0, 0, UINT64_C(917321) + n};
                memcpy(work, original, n * sizeof *work);
                clock_t start = clock();
                if (start == (clock_t)-1) { fprintf(stderr, "Clock unavailable\n"); return EXIT_FAILURE; }
                sorts[k](work, n, &s);
                clock_t end = clock();
                if (end == (clock_t)-1) { fprintf(stderr, "Clock unavailable\n"); return EXIT_FAILURE; }
                if (memcmp(work, reference, n * sizeof *work) != 0 || s.live != 0) {
                    fprintf(stderr, "Validation failed: %s, n=%zu\n", names[k], n);
                    return EXIT_FAILURE;
                }
                if (r >= 0) {
                    times[k][r] = 1000.0 * (double)(end - start) / CLOCKS_PER_SEC;
                    comparisons[k] = s.comparisons;
                    memories[k] = s.peak;
                }
            }
        }
        printf("Data Size : %zu\n\n", n);
        printf("%-17s %14s %12s %16s\n", "Algorithm", "Comparisons", "Time(ms)", "Memory(Bytes)");
        printf("----------------------------------------------------------------\n");
        for (int k = 0; k < ALGORITHMS; k++) {
            double ms = median_time(times[k]);
            printf("%-17s %14" PRIu64 " %12.3f %16zu\n", names[k], comparisons[k], ms, memories[k]);
            fprintf(csv, "%zu,%s,%" PRIu64 ",%.6f,%zu\n", n, names[k], comparisons[k], ms, memories[k]);
        }
        putchar('\n');
        free(reference); free(work); free(original);
    }
    if (fclose(csv) != 0) { perror("results.csv"); return EXIT_FAILURE; }
    return EXIT_SUCCESS;
}
