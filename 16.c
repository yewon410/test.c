#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 1024
#define MAX_NAME 100
#define MAX_K 5000

typedef struct {
    char id[30];
    char name[MAX_NAME];
    char gender[10];
    int korean;
    int english;
    int math;
    int total;
} Student;

int loadCSV(const char* filename, Student** students) {
    FILE* fp = fopen(filename, "r");

    if (fp == NULL) {
        printf("CSV 파일을 열 수 없습니다.\n");
        return 0;
    }

    int capacity = 1000;
    int count = 0;
    char line[MAX_LINE];

    *students = (Student*)malloc(sizeof(Student) * capacity);

    if (*students == NULL) {
        fclose(fp);
        return 0;
    }

    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        if (count >= capacity) {
            capacity *= 2;
            Student* temp = (Student*)realloc(*students, sizeof(Student) * capacity);

            if (temp == NULL) {
                free(*students);
                *students = NULL;
                fclose(fp);
                return 0;
            }

            *students = temp;
        }

        char* token = strtok(line, ",\r\n");

        if (token == NULL) continue;
        strcpy((*students)[count].id, token);

        token = strtok(NULL, ",\r\n");
        if (token == NULL) continue;
        strcpy((*students)[count].name, token);

        token = strtok(NULL, ",\r\n");
        if (token == NULL) continue;
        strcpy((*students)[count].gender, token);

        token = strtok(NULL, ",\r\n");
        if (token == NULL) continue;
        (*students)[count].korean = atoi(token);

        token = strtok(NULL, ",\r\n");
        if (token == NULL) continue;
        (*students)[count].english = atoi(token);

        token = strtok(NULL, ",\r\n");
        if (token == NULL) continue;
        (*students)[count].math = atoi(token);

        (*students)[count].total =
            (*students)[count].korean +
            (*students)[count].english +
            (*students)[count].math;

        count++;
    }

    fclose(fp);
    return count;
}

void printStudent(Student s) {
    printf("ID      : %s\n", s.id);
    printf("Name    : %s\n", s.name);
    printf("Gender  : %s\n", s.gender);
    printf("Korean  : %d\n", s.korean);
    printf("English : %d\n", s.english);
    printf("Math    : %d\n", s.math);
    printf("Total   : %d\n", s.total);
    printf("-------------------------\n");
}

long long sequentialSearch(Student arr[], int n, char* name, int print) {
    long long comparisons = 0;
    int found = 0;

    for (int i = 0; i < n; i++) {
        comparisons++;

        if (strcmp(arr[i].name, name) == 0) {
            found++;

            if (print) {
                printStudent(arr[i]);
            }
        }
    }

    if (print && found == 0) {
        printf("검색 결과가 없습니다.\n");
    }

    return comparisons;
}

void swap(Student* a, Student* b) {
    Student temp = *a;
    *a = *b;
    *b = temp;
}

void quickSort(Student arr[], int left, int right, long long* comparisons) {
    int i = left;
    int j = right;

    Student pivot = arr[(left + right) / 2];

    while (i <= j) {
        while (1) {
            (*comparisons)++;

            if (strcmp(arr[i].name, pivot.name) < 0)
                i++;
            else
                break;
        }

        while (1) {
            (*comparisons)++;

            if (strcmp(arr[j].name, pivot.name) > 0)
                j--;
            else
                break;
        }

        if (i <= j) {
            swap(&arr[i], &arr[j]);
            i++;
            j--;
        }
    }

    if (left < j)
        quickSort(arr, left, j, comparisons);

    if (i < right)
        quickSort(arr, i, right, comparisons);
}

long long binarySearch(Student arr[], int n, char* name, int print) {
    int left = 0;
    int right = n - 1;
    int found = -1;

    long long comparisons = 0;

    while (left <= right) {
        int mid = (left + right) / 2;

        comparisons++;

        int result = strcmp(arr[mid].name, name);

        if (result == 0) {
            found = mid;
            break;
        }
        else if (result < 0) {
            left = mid + 1;
        }
        else {
            right = mid - 1;
        }
    }

    if (found == -1) {
        if (print)
            printf("검색 결과가 없습니다.\n");

        return comparisons;
    }

    int first = found;
    int last = found;

    while (first > 0) {
        comparisons++;

        if (strcmp(arr[first - 1].name, name) == 0)
            first--;
        else
            break;
    }

    while (last < n - 1) {
        comparisons++;

        if (strcmp(arr[last + 1].name, name) == 0)
            last++;
        else
            break;
    }

    if (print) {
        for (int i = first; i <= last; i++) {
            printStudent(arr[i]);
        }
    }

    return comparisons;
}

int main(int argc, char* argv[]) {
    const char* filename = argc >= 2
        ? argv[1]
        : "dataset_id_ascending (1) (2).csv";

    Student* students = NULL;

    int n = loadCSV(filename, &students);

    if (n == 0) {
        printf("학생 데이터를 불러오지 못했습니다.\n");
        return 1;
    }

    printf("전체 학생 수 : %d\n", n);

    Student* sorted = (Student*)malloc(sizeof(Student) * n);

    if (sorted == NULL) {
        free(students);
        return 1;
    }

    memcpy(sorted, students, sizeof(Student) * n);

    long long quickComparisons = 0;

    quickSort(sorted, 0, n - 1, &quickComparisons);

    char searchName[MAX_NAME];

    printf("\n검색할 학생 이름을 입력하세요: ");

    if (fgets(searchName, sizeof(searchName), stdin) == NULL) {
        free(students);
        free(sorted);
        return 1;
    }

    searchName[strcspn(searchName, "\r\n")] = '\0';

    printf("\n===== Sequential Search =====\n");

    long long seqSearchCount =
        sequentialSearch(students, n, searchName, 1);

    printf("Sequential Comparisons : %lld\n", seqSearchCount);

    printf("\n===== Quick Sort + Binary Search =====\n");

    long long binarySearchCount =
        binarySearch(sorted, n, searchName, 1);

    printf("Quick Sort Comparisons : %lld\n", quickComparisons);
    printf("Binary Comparisons     : %lld\n", binarySearchCount);
    printf("Total Comparisons      : %lld\n",
        quickComparisons + binarySearchCount);

    char (*queries)[MAX_NAME] =
        (char (*)[MAX_NAME])malloc(sizeof(char[MAX_NAME]) * MAX_K);

    if (queries == NULL) {
        free(students);
        free(sorted);
        return 1;
    }

    for (int i = 0; i < MAX_K; i++) {
        if (i % 10 == 9) {
            snprintf(queries[i], MAX_NAME, "NotFound%d", i);
        }
        else {
            int index = (int)(((long long)i * 7919) % n);
            strcpy(queries[i], students[index].name);
        }
    }

    int K_values[] = { 10, 50, 100, 200, 500, 1000, 2000, 5000 };

    int size = sizeof(K_values) / sizeof(K_values[0]);

    long long sequentialTotal = 0;
    long long binaryTotal = quickComparisons;

    int previousK = 0;

    printf("\n========== Comparison Results ==========\n");

    printf("%-10s %-20s %-20s\n",
        "K", "Sequential", "Quick + Binary");

    printf("--------------------------------------------------\n");

    for (int k = 0; k < size; k++) {
        int currentK = K_values[k];

        for (int i = previousK; i < currentK; i++) {
            sequentialTotal +=
                sequentialSearch(students, n, queries[i], 0);

            binaryTotal +=
                binarySearch(sorted, n, queries[i], 0);
        }

        printf("%-10d %-20lld %-20lld\n",
            currentK, sequentialTotal, binaryTotal);

        previousK = currentK;
    }

    printf("\nQuick Sort Initial Comparisons : %lld\n",
        quickComparisons);

    printf("\nFinal Result at K = 5000\n");

    if (binaryTotal < sequentialTotal) {
        printf("Quick + Binary Search is faster.\n");
    }
    else if (binaryTotal > sequentialTotal) {
        printf("Sequential Search is faster.\n");
    }
    else {
        printf("Both methods have the same comparisons.\n");
    }

    free(queries);
    free(students);
    free(sorted);

    return 0;
}
