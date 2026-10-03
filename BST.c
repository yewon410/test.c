#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100
#define K 50
#define MAX_VAL 1000

typedef struct Node {
    int data;
    struct Node *left;
    struct Node *right;
} Node;

static Node *new_node(int data) {
    Node *n = (Node *)malloc(sizeof(Node));

    if (n == NULL) {
        printf("메모리 할당 실패\n");
        exit(1);
    }

    n->data = data;
    n->left = NULL;
    n->right = NULL;

    return n;
}

static Node *bst_insert(Node *root, int data, long *cmp) {
    if (root == NULL)
        return new_node(data);

    Node *cur = root;

    while (1) {
        (*cmp)++;

        if (data < cur->data) {
            if (cur->left == NULL) {
                cur->left = new_node(data);
                break;
            }
            cur = cur->left;
        }
        else {
            if (cur->right == NULL) {
                cur->right = new_node(data);
                break;
            }
            cur = cur->right;
        }
    }

    return root;
}

static int seq_search(const int a[], int n, int key, long *cmp) {
    for (int i = 0; i < n; i++) {
        (*cmp)++;

        if (a[i] == key)
            return 1;
    }

    return 0;
}

static int bst_search(Node *root, int key, long *cmp) {
    Node *cur = root;

    while (cur != NULL) {
        (*cmp)++;

        if (key == cur->data)
            return 1;

        if (key < cur->data)
            cur = cur->left;
        else
            cur = cur->right;
    }

    return 0;
}

static void bst_free(Node *n) {
    if (n == NULL)
        return;

    bst_free(n->left);
    bst_free(n->right);
    free(n);
}

int main(void) {
    int arr[N];
    int keys[K];
    int used[MAX_VAL + 1] = {0};
    int count = 0;

    srand((unsigned)time(NULL));

    while (count < N) {
        int v = rand() % (MAX_VAL + 1);

        if (used[v])
            continue;

        used[v] = 1;
        arr[count] = v;
        count++;
    }

    printf("========================================\n");
    printf("생성된 100개의 정수\n");
    printf("========================================\n");

    for (int i = 0; i < N; i++) {
        printf("%d ", arr[i]);

        if ((i + 1) % 10 == 0)
            printf("\n");
    }

    Node *root = NULL;
    long build_cmp = 0;

    for (int i = 0; i < N; i++)
        root = bst_insert(root, arr[i], &build_cmp);

    printf("\nBST 생성 총 비교 횟수: %ld\n", build_cmp);

    for (int i = 0; i < K; i++)
        keys[i] = rand() % (MAX_VAL + 1);

    printf("\n========================================\n");
    printf("생성된 50개의 탐색 대상\n");
    printf("========================================\n");

    for (int i = 0; i < K; i++) {
        printf("%d ", keys[i]);

        if ((i + 1) % 10 == 0)
            printf("\n");
    }

    long seq_total = 0;
    long bst_total = 0;

    printf("\n========================================\n");
    printf("탐색 결과\n");
    printf("========================================\n");

    printf("%-4s %-8s %-12s %-12s %-8s\n",
           "No.", "Key", "순차비교", "BST비교", "결과");

    printf("------------------------------------------------\n");

    for (int i = 0; i < K; i++) {
        long c_seq = 0;
        long c_bst = 0;

        int f_seq = seq_search(arr, N, keys[i], &c_seq);
        int f_bst = bst_search(root, keys[i], &c_bst);

        if (f_seq != f_bst) {
            printf("오류: 탐색 결과 불일치 (key=%d)\n", keys[i]);
            bst_free(root);
            return 1;
        }

        seq_total += c_seq;
        bst_total += c_bst;

        printf("%-4d %-8d %-12ld %-12ld %-8s\n",
               i + 1,
               keys[i],
               c_seq,
               c_bst,
               f_seq ? "성공" : "실패");
    }

    double seq_avg = (double)seq_total / K;
    double bst_avg = (double)bst_total / K;

    printf("\n========================================\n");
    printf("탐색 통계\n");
    printf("========================================\n");

    printf("탐색 횟수                  : %d\n", K);

    printf("\n[순차 탐색]\n");
    printf("총 비교 횟수               : %ld\n", seq_total);
    printf("평균 비교 횟수             : %.2f\n", seq_avg);

    printf("\n[BST 탐색]\n");
    printf("총 비교 횟수               : %ld\n", bst_total);
    printf("평균 비교 횟수             : %.2f\n", bst_avg);

    printf("\n[BST 생성 비용 포함]\n");
    printf("BST 생성 총 비교 횟수      : %ld\n", build_cmp);
    printf("BST 생성 + 50회 탐색       : %ld\n",
           build_cmp + bst_total);

    bst_free(root);

    return 0;
}
