#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VALUE_RANGE 100000

typedef enum {
    FIRST,
    RANDOM,
    MEDIAN3
} PivotType;

int lessThan(int a, int b, long long* comparisons)
{
    (*comparisons)++;
    return a < b;
}

void swap(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

int medianOfThree(const int arr[], int low, int high,
    long long* comparisons)
{
    int mid = low + (high - low) / 2;

    if (lessThan(arr[low], arr[mid], comparisons)) {
        if (lessThan(arr[mid], arr[high], comparisons)) {
            return mid;
        }

        if (lessThan(arr[low], arr[high], comparisons)) {
            return high;
        }

        return low;
    }
    else {
        if (lessThan(arr[low], arr[high], comparisons)) {
            return low;
        }

        if (lessThan(arr[mid], arr[high], comparisons)) {
            return high;
        }

        return mid;
    }
}

int selectPivot(const int arr[], int low, int high,
    PivotType type, long long* comparisons)
{
    switch (type) {
    case FIRST:
        return low;

    case RANDOM:
        return low + rand() % (high - low + 1);

    case MEDIAN3:
        return medianOfThree(arr, low, high, comparisons);
    }

    return low;
}

int partition(int arr[], int low, int high,
    PivotType type, long long* comparisons)
{
    int pivotIndex = selectPivot(arr, low, high, type, comparisons);

    swap(&arr[pivotIndex], &arr[high]);

    int pivot = arr[high];
    int i = low;

    for (int j = low; j < high; j++) {
        (*comparisons)++;

        if (arr[j] <= pivot) {
            swap(&arr[i], &arr[j]);
            i++;
        }
    }

    swap(&arr[i], &arr[high]);
    return i;
}

void quickSort(int arr[], int low, int high,
    PivotType type, long long* comparisons)
{
    while (low < high) {
        int p = partition(arr, low, high, type, comparisons);

        if (p - low < high - p) {
            quickSort(arr, low, p - 1, type, comparisons);
            low = p + 1;
        }
        else {
            quickSort(arr, p + 1, high, type, comparisons);
            high = p - 1;
        }
    }
}

int isSorted(const int arr[], int n)
{
    for (int i = 1; i < n; i++) {
        if (arr[i - 1] > arr[i]) {
            return 0;
        }
    }

    return 1;
}

int randomValue(void)
{
    unsigned long long value =
        (unsigned long long)rand() *
        ((unsigned long long)RAND_MAX + 1ULL)
        + (unsigned long long)rand();

    return (int)(value % VALUE_RANGE);
}

int main(void)
{
    const int sizes[] = { 1000, 2000, 4000, 8000, 16000, 20000 };
    const int sizeCount = sizeof(sizes) / sizeof(sizes[0]);

    const PivotType types[] = { FIRST, RANDOM, MEDIAN3 };
    const char* names[] = { "First", "Random", "Median-3" };

    srand(42);

    for (int s = 0; s < sizeCount; s++) {
        int n = sizes[s];
        size_t bytes = (size_t)n * sizeof(int);

        int* original = (int*)malloc(bytes);
        int* work = (int*)malloc(bytes);

        if (original == NULL || work == NULL) {
            fprintf(stderr, "Memory allocation failed.\n");
            free(original);
            free(work);
            return EXIT_FAILURE;
        }

        for (int i = 0; i < n; i++) {
            original[i] = randomValue();
        }

        printf("Data Size : %d\n\n", n);

        for (int method = 0; method < 3; method++) {
            long long comparisons = 0;

            memcpy(work, original, bytes);

            quickSort(work, 0, n - 1, types[method], &comparisons);

            if (!isSorted(work, n)) {
                fprintf(stderr,
                    "Sorting failed: size=%d, method=%s\n",
                    n, names[method]);

                free(original);
                free(work);
                return EXIT_FAILURE;
            }

            printf("Quick Sort (%s)%*s: %lld comparisons\n",
                names[method],
                9 - (int)strlen(names[method]), "",
                comparisons);
        }

        printf("\n");

        free(original);
        free(work);
    }

    return EXIT_SUCCESS;
}
