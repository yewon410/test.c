#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 20000

long long sort(int a[], int n, int gap, int type)
{
    long long count = 0;

    while (gap > 0) {
        for (int i = gap; i < n; i++) {
            int key = a[i];
            int j = i;

            while (j >= gap) {
                count++;
                if (a[j - gap] <= key)
                    break;
                a[j] = a[j - gap];
                j -= gap;
            }
            a[j] = key;
        }

        if (type == 0)
            break;
        gap = (type == 1) ? gap / 2 : (gap - 1) / 3;
    }

    return count;
}

int main(void)
{
    int sizes[] = {1000, 2000, 4000, 8000, 16000, 20000};
    static int original[MAX], a[MAX];
    const char *names[] = {
        "Insertion Sort       ",
        "Shell Sort (Original)",
        "Shell Sort (Knuth)   "
    };

    srand(42);

    for (int t = 0; t < 6; t++) {
        int n = sizes[t];
        int knuth = 1;

        for (int i = 0; i < n; i++)
            original[i] = rand();

        while (knuth * 3 + 1 < n)
            knuth = knuth * 3 + 1;

        printf("Data Size : %d\n\n", n);

        for (int type = 0; type < 3; type++) {
            memcpy(a, original, n * sizeof(int));

            int gap = (type == 0) ? 1 :
                      (type == 1) ? n / 2 : knuth;

            long long count = sort(a, n, gap, type);

            for (int i = 1; i < n; i++) {
                if (a[i - 1] > a[i]) {
                    printf("Sorting failed.\n");
                    return 1;
                }
            }

            printf("%s: %lld comparisons\n", names[type], count);
        }
        printf("\n");
    }

    return 0;
}
