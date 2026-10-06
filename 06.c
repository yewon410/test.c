#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DATA_COUNT 100
#define SEARCH_COUNT 50
#define MAX_VALUE 1000

typedef struct {
    int data[DATA_COUNT];
    int size;
} Array;

typedef struct BSTNode {
    int data;
    struct BSTNode* left;
    struct BSTNode* right;
} BSTNode;

typedef struct AVLNode {
    int data;
    int height;
    struct AVLNode* left;
    struct AVLNode* right;
} AVLNode;

int arrayInsert(Array* arr, int value) {
    int i;
    int comparisons = 0;

    for (i = 0; i < arr->size; i++) {
        comparisons++;

        if (arr->data[i] == value) {
            return comparisons;
        }
    }

    arr->data[arr->size] = value;
    arr->size++;

    return comparisons;
}

int arraySearch(Array* arr, int key, int* comparisons) {
    int i;

    *comparisons = 0;

    for (i = 0; i < arr->size; i++) {
        (*comparisons)++;

        if (arr->data[i] == key) {
            return 1;
        }
    }

    return 0;
}

BSTNode* createBSTNode(int value) {
    BSTNode* node = (BSTNode*)malloc(sizeof(BSTNode));

    if (node == NULL) {
        printf("메모리 할당 실패\n");
        exit(1);
    }

    node->data = value;
    node->left = NULL;
    node->right = NULL;

    return node;
}

BSTNode* bstInsert(BSTNode* root, int value, int* comparisons) {
    BSTNode* current;
    BSTNode* parent;

    if (root == NULL) {
        return createBSTNode(value);
    }

    current = root;
    parent = NULL;

    while (current != NULL) {
        (*comparisons)++;

        if (value == current->data) {
            return root;
        }

        parent = current;

        if (value < current->data) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }

    if (value < parent->data) {
        parent->left = createBSTNode(value);
    }
    else {
        parent->right = createBSTNode(value);
    }

    return root;
}

int bstSearch(BSTNode* root, int key, int* comparisons) {
    BSTNode* current = root;

    *comparisons = 0;

    while (current != NULL) {
        (*comparisons)++;

        if (key == current->data) {
            return 1;
        }

        if (key < current->data) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }

    return 0;
}

int bstHeight(BSTNode* root) {
    int leftH;
    int rightH;

    if (root == NULL) {
        return 0;
    }

    leftH = bstHeight(root->left);
    rightH = bstHeight(root->right);

    if (leftH > rightH) {
        return leftH + 1;
    }
    else {
        return rightH + 1;
    }
}

void freeBST(BSTNode* root) {
    if (root == NULL) {
        return;
    }

    freeBST(root->left);
    freeBST(root->right);
    free(root);
}

AVLNode* createAVLNode(int value) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));

    if (node == NULL) {
        printf("메모리 할당 실패\n");
        exit(1);
    }

    node->data = value;
    node->height = 1;
    node->left = NULL;
    node->right = NULL;

    return node;
}

int avlHeight(AVLNode* node) {
    if (node == NULL) {
        return 0;
    }

    return node->height;
}

int getMax(int a, int b) {
    if (a > b) {
        return a;
    }

    return b;
}

AVLNode* rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = getMax(avlHeight(y->left),
                        avlHeight(y->right)) + 1;

    x->height = getMax(avlHeight(x->left),
                        avlHeight(x->right)) + 1;

    return x;
}

AVLNode* rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = getMax(avlHeight(x->left),
                       avlHeight(x->right)) + 1;

    y->height = getMax(avlHeight(y->left),
                       avlHeight(y->right)) + 1;

    return y;
}

int getBalance(AVLNode* node) {
    if (node == NULL) {
        return 0;
    }

    return avlHeight(node->left) - avlHeight(node->right);
}

AVLNode* avlInsert(AVLNode* node, int value, int* comparisons) {
    int balance;

    if (node == NULL) {
        return createAVLNode(value);
    }

    (*comparisons)++;

    if (value < node->data) {
        node->left = avlInsert(node->left, value, comparisons);
    }
    else if (value > node->data) {
        node->right = avlInsert(node->right, value, comparisons);
    }
    else {
        return node;
    }

    node->height =
        1 + getMax(avlHeight(node->left),
                   avlHeight(node->right));

    balance = getBalance(node);

    if (balance > 1 && value < node->left->data) {
        return rotateRight(node);
    }

    if (balance < -1 && value > node->right->data) {
        return rotateLeft(node);
    }

    if (balance > 1 && value > node->left->data) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    if (balance < -1 && value < node->right->data) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

int avlSearch(AVLNode* root, int key, int* comparisons) {
    AVLNode* current = root;

    *comparisons = 0;

    while (current != NULL) {
        (*comparisons)++;

        if (key == current->data) {
            return 1;
        }

        if (key < current->data) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }

    return 0;
}

void freeAVL(AVLNode* root) {
    if (root == NULL) {
        return;
    }

    freeAVL(root->left);
    freeAVL(root->right);
    free(root);
}

int main(void) {
    Array arr;
    BSTNode* bstRoot = NULL;
    AVLNode* avlRoot = NULL;

    int arrayInsertComp = 0;
    int bstInsertComp = 0;
    int avlInsertComp = 0;

    int searchKeys[SEARCH_COUNT];

    int totalArraySearchComp = 0;
    int totalBSTSearchComp = 0;
    int totalAVLSearchComp = 0;

    int i;
    int compTemp;

    srand((unsigned int)time(NULL));

    arr.size = 0;

    printf("========================================\n");
    printf("1. 100개 난수 생성 및 자료구조 저장\n");
    printf("========================================\n");

    for (i = 0; i < DATA_COUNT; i++) {
        int val = rand() % (MAX_VALUE + 1);

        arrayInsertComp += arrayInsert(&arr, val);

        compTemp = 0;

        if (bstRoot == NULL) {
            bstRoot = createBSTNode(val);
        }
        else {
            bstRoot = bstInsert(bstRoot, val, &compTemp);
        }

        bstInsertComp += compTemp;

        compTemp = 0;

        if (avlRoot == NULL) {
            avlRoot = createAVLNode(val);
        }
        else {
            avlRoot = avlInsert(avlRoot, val, &compTemp);
        }

        avlInsertComp += compTemp;

        printf("%3d번째 생성된 값: %d\n", i + 1, val);
    }

    printf("\n========================================\n");
    printf("2. 생성 및 저장 결과\n");
    printf("========================================\n");

    printf("생성된 총 데이터 수     : %d\n", DATA_COUNT);
    printf("실제로 저장된 값의 수   : %d\n", arr.size);
    printf("중복되어 제외된 값의 수 : %d\n\n", DATA_COUNT - arr.size);

    printf("Construction Comparisons\n");
    printf("Array comparisons : %d\n", arrayInsertComp);
    printf("BST comparisons   : %d\n", bstInsertComp);
    printf("AVL comparisons   : %d\n", avlInsertComp);

    printf("\n========================================\n");
    printf("3. 자료구조 크기 및 높이\n");
    printf("========================================\n");

    printf("Array length : %d\n", arr.size);
    printf("BST height   : %d\n", bstHeight(bstRoot));
    printf("AVL height   : %d\n", avlHeight(avlRoot));

    printf("\n========================================\n");
    printf("4. 50개 탐색 키 생성\n");
    printf("========================================\n");

    for (i = 0; i < SEARCH_COUNT; i++) {
        searchKeys[i] = rand() % (MAX_VALUE + 1);

        printf("%2d번째 Search Key: %d\n",
               i + 1,
               searchKeys[i]);
    }

    printf("\n========================================\n");
    printf("5. 탐색 수행 결과\n");
    printf("========================================\n");

    for (i = 0; i < SEARCH_COUNT; i++) {
        int key = searchKeys[i];
        int arrC;
        int bstC;
        int avlC;

        int arrRes;
        int bstRes;
        int avlRes;

        arrRes = arraySearch(&arr, key, &arrC);
        bstRes = bstSearch(bstRoot, key, &bstC);
        avlRes = avlSearch(avlRoot, key, &avlC);

        totalArraySearchComp += arrC;
        totalBSTSearchComp += bstC;
        totalAVLSearchComp += avlC;

        printf("\n----------------------------------------\n");
        printf("Search Key : %d\n", key);

        printf("\nSequential Search\n");
        printf("Result      : %s\n",
               arrRes ? "Found" : "Not Found");
        printf("Comparisons : %d\n", arrC);

        printf("\nBST Search\n");
        printf("Result      : %s\n",
               bstRes ? "Found" : "Not Found");
        printf("Comparisons : %d\n", bstC);

        printf("\nAVL Search\n");
        printf("Result      : %s\n",
               avlRes ? "Found" : "Not Found");
        printf("Comparisons : %d\n", avlC);
    }

    printf("\n\n========================================\n");
    printf("6. 전체 탐색 비교 결과 (50회)\n");
    printf("========================================\n");

    printf("\nSequential Search\n");
    printf("Total comparisons   : %d\n",
           totalArraySearchComp);
    printf("Average comparisons : %.2f\n",
           (double)totalArraySearchComp / SEARCH_COUNT);

    printf("\nBST Search\n");
    printf("Total comparisons   : %d\n",
           totalBSTSearchComp);
    printf("Average comparisons : %.2f\n",
           (double)totalBSTSearchComp / SEARCH_COUNT);

    printf("\nAVL Search\n");
    printf("Total comparisons   : %d\n",
           totalAVLSearchComp);
    printf("Average comparisons : %.2f\n",
           (double)totalAVLSearchComp / SEARCH_COUNT);

    freeBST(bstRoot);
    freeAVL(avlRoot);

    return 0;
}
