#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

#define VALUE_RANGE 100000
#define REPEAT 100

typedef struct {
    int low;
    int high;
} Range;

double nowMs(void)
{
#ifdef _WIN32
    LARGE_INTEGER frequency;
    LARGE_INTEGER counter;

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);

    return (double)counter.QuadPart * 1000.0
        / (double)frequency.QuadPart;
#else
    return (double)clock() * 1000.0 / CLOCKS_PER_SEC;
#endif
}

void swap(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

int medianOfThree(const int arr[], int low, int high)
{
    int mid = low + (high - low) / 2;

    if (arr[low] < arr[mid]) {
        if (arr[mid] < arr[high]) {
            return mid;
        }

        if (arr[low] < arr[high]) {
            return high;
        }

        return low;
    }
    else {
        if (arr[low] < arr[high]) {
            return low;
        }

        if (arr[mid] < arr[high]) {
            return high;
        }

        return mid;
    }
}

int partition(int arr[], int low, int high)
{
    int pivotIndex = medianOfThree(arr, low, high);

    swap(&arr[pivotIndex], &arr[high]);

    int pivot = arr[high];
    int i = low;

    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            swap(&arr[i], &arr[j]);
            i++;
        }
    }

    swap(&arr[i], &arr[high]);

    return i;
}

void recursiveQuickSort(int arr[], int low, int high)
{
    while (low < high) {
        int p = partition(arr, low, high);

        if (p - low < high - p) {
            recursiveQuickSort(arr, low, p - 1);
            low = p + 1;
        }
        else {
            recursiveQuickSort(arr, p + 1, high);
            high = p - 1;
        }
    }
}

void iterativeQuickSort(int arr[], int n, Range stack[])
{
    int top = -1;
    int low = 0;
    int high = n - 1;

    while (1) {
        while (low < high) {
            int p = partition(arr, low, high);

            if (p - low < high - p) {
                if (p + 1 < high) {
                    top++;
                    stack[top].low = p + 1;
                    stack[top].high = high;
                }

                high = p - 1;
            }
            else {
                if (low < p - 1) {
                    top++;
                    stack[top].low = low;
                    stack[top].high = p - 1;
                }

                low = p + 1;
            }
        }

        if (top < 0) {
            break;
        }

        low = stack[top].low;
        high = stack[top].high;
        top--;
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
    unsigned long long first = (unsigned long long)rand();
    unsigned long long second = (unsigned long long)rand();

    unsigned long long value =
        first * ((unsigned long long)RAND_MAX + 1ULL) + second;

    return (int)(value % VALUE_RANGE);
}

int main(void)
{
    const int sizes[] = { 1000, 2000, 4000, 8000, 16000, 20000 };
    const int sizeCount = sizeof(sizes) / sizeof(sizes[0]);

    srand(42);

    for (int s = 0; s < sizeCount; s++) {
        int n = sizes[s];
        size_t bytes = (size_t)n * sizeof(int);

        int* original = (int*)malloc(bytes);
        int* work = (int*)malloc(bytes);
        Range* stack = (Range*)malloc((size_t)n * sizeof(Range));

        if (original == NULL || work == NULL || stack == NULL) {
            fprintf(stderr, "Memory allocation failed.\n");

            free(original);
            free(work);
            free(stack);

            return EXIT_FAILURE;
        }

        for (int i = 0; i < n; i++) {
            original[i] = randomValue();
        }

        double recursiveTotal = 0.0;
        double iterativeTotal = 0.0;

        memcpy(work, original, bytes);
        recursiveQuickSort(work, 0, n - 1);

        memcpy(work, original, bytes);
        iterativeQuickSort(work, n, stack);

        for (int repeat = 0; repeat < REPEAT; repeat++) {
            for (int turn = 0; turn < 2; turn++) {
                int method = (repeat + turn) % 2;

                memcpy(work, original, bytes);

                double start = nowMs();

                if (method == 0) {
                    recursiveQuickSort(work, 0, n - 1);
                }
                else {
                    iterativeQuickSort(work, n, stack);
                }

                double end = nowMs();

                if (method == 0) {
                    recursiveTotal += end - start;
                }
                else {
                    iterativeTotal += end - start;
                }

                if (!isSorted(work, n)) {
                    fprintf(stderr, "Sorting failed.\n");

                    free(original);
                    free(work);
                    free(stack);

                    return EXIT_FAILURE;
                }
            }
        }

        printf("Data Size : %d\n", n);
        printf("Pivot     : Median-of-Three\n\n");
        printf("Recursive Quick Sort : %.6f ms\n",
            recursiveTotal / REPEAT);
        printf("Iterative Quick Sort : %.6f ms\n\n",
            iterativeTotal / REPEAT);

        free(original);
        free(work);
        free(stack);
    }

    return EXIT_SUCCESS;
}
