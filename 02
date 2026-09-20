#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT 1024
#define EMPTY     '.'
#define INDENT    "     "

typedef struct TreeNode {
    char             data;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

char buffer[MAX_INPUT];
int  lastIndex = 0;

TreeNode *createNode(char data)
{
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
    if (node == NULL) {
        printf("메모리 할당 실패\n");
        exit(1);
    }
    node->data  = data;
    node->left  = NULL;
    node->right = NULL;
    return node;
}

void inputLevelOrder(void)
{
    char line[4096];
    char *token;
    int  index = 1;
    int  i;

    for (i = 0; i < MAX_INPUT; i++) buffer[i] = EMPTY;

    printf("레벨 순서로 이진트리를 입력하세요 (빈 노드는 '.')\n");
    printf("예) A B C D . E F\n");
    printf(">> ");

    if (fgets(line, sizeof(line), stdin) == NULL) {
        printf("입력 오류\n");
        exit(1);
    }

    token = strtok(line, " \t\n\r");
    while (token != NULL && index < MAX_INPUT) {
        buffer[index] = token[0];
        if (buffer[index] != EMPTY) lastIndex = index;
        index++;
        token = strtok(NULL, " \t\n\r");
    }
}

TreeNode *buildTree(int index)
{
    TreeNode *node;

    if (index >= MAX_INPUT || index > lastIndex || buffer[index] == EMPTY)
        return NULL;

    node = createNode(buffer[index]);
    node->left  = buildTree(2 * index);
    node->right = buildTree(2 * index + 1);

    return node;
}

void freeTree(TreeNode *root)
{
    if (root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

void printTree(TreeNode *root, int level)
{
    int i;

    if (root == NULL) return;

    printTree(root->right, level + 1);

    for (i = 0; i < level; i++)
        printf("%s", INDENT);
    printf("%c\n\n", root->data);

    printTree(root->left, level + 1);
}

int countNodes(TreeNode *root)
{
    if (root == NULL) return 0;
    return 1 + countNodes(root->left) + countNodes(root->right);
}

int countLeafNodes(TreeNode *root)
{
    if (root == NULL) return 0;
    if (root->left == NULL && root->right == NULL) return 1;
    return countLeafNodes(root->left) + countLeafNodes(root->right);
}

int countNonLeafNodes(TreeNode *root)
{
    if (root == NULL) return 0;
    if (root->left == NULL && root->right == NULL) return 0;
    return 1 + countNonLeafNodes(root->left) + countNonLeafNodes(root->right);
}

int getHeight(TreeNode *root)
{
    int hLeft, hRight;

    if (root == NULL) return 0;

    hLeft  = getHeight(root->left);
    hRight = getHeight(root->right);

    return (hLeft > hRight ? hLeft : hRight) + 1;
}

int getDegree(TreeNode *root)
{
    int d = 0, dLeft, dRight, max;

    if (root == NULL) return 0;

    if (root->left  != NULL) d++;
    if (root->right != NULL) d++;

    dLeft  = getDegree(root->left);
    dRight = getDegree(root->right);

    max = d;
    if (dLeft  > max) max = dLeft;
    if (dRight > max) max = dRight;

    return max;
}

void printTreeInfo(TreeNode *root)
{
    printf("1. 전체 노드의 수     : %d\n", countNodes(root));
    printf("2. 단말 노드의 수     : %d\n", countLeafNodes(root));
    printf("3. 비단말 노드의 수   : %d\n", countNonLeafNodes(root));
    printf("4. 트리의 높이        : %d\n", getHeight(root));
    printf("5. 트리의 차수        : %d\n", getDegree(root));
}

int isCompleteUtil(TreeNode *root, int index, int n)
{
    if (root == NULL) return 1;
    if (index > n)    return 0;

    return isCompleteUtil(root->left,  2 * index,     n) &&
           isCompleteUtil(root->right, 2 * index + 1, n);
}

int isComplete(TreeNode *root)
{
    return isCompleteUtil(root, 1, countNodes(root));
}

int isFull(TreeNode *root)
{
    int n = countNodes(root);
    int h = getHeight(root);

    if (n == 0) return 1;
    return (n == (1 << h) - 1);
}

int isSkewed(TreeNode *root)
{
    if (root == NULL) return 0;
    return (countNodes(root) == getHeight(root));
}

void countChildDirection(TreeNode *root, int *left, int *right)
{
    if (root == NULL) return;
    if (root->left  != NULL) (*left)++;
    if (root->right != NULL) (*right)++;
    countChildDirection(root->left,  left, right);
    countChildDirection(root->right, left, right);
}

void printTreeType(TreeNode *root)
{
    printf("완전 이진트리 여부   : %s\n", isComplete(root) ? "예" : "아니오");
    printf("포화 이진트리 여부   : %s\n", isFull(root)     ? "예" : "아니오");

    printf("편향 이진트리 여부   : %s", isSkewed(root) ? "예" : "아니오");
    if (isSkewed(root)) {
        int left = 0, right = 0;
        countChildDirection(root, &left, &right);
        if (left > 0 && right == 0)      printf(" (왼쪽 편향)");
        else if (right > 0 && left == 0) printf(" (오른쪽 편향)");
        else if (left > 0 && right > 0)  printf(" (혼합 편향)");
    }
    printf("\n");
}

void preorder(TreeNode *root)
{
    if (root == NULL) return;
    printf("%c ", root->data);
    preorder(root->left);
    preorder(root->right);
}

void inorder(TreeNode *root)
{
    if (root == NULL) return;
    inorder(root->left);
    printf("%c ", root->data);
    inorder(root->right);
}

void postorder(TreeNode *root)
{
    if (root == NULL) return;
    postorder(root->left);
    postorder(root->right);
    printf("%c ", root->data);
}

int main(void)
{
    TreeNode *root;

    inputLevelOrder();
    root = buildTree(1);

    printf("\n================ [1] 이진트리 출력 ================\n\n");
    printTree(root, 0);

    printf("================ [2] 트리 정보 ================\n");
    printTreeInfo(root);

    printf("\n================ [3] 트리 형태 판별 ================\n");
    printTreeType(root);

    printf("\n================ 참고 : 순회 결과 ================\n");
    printf("전위 순회 : "); preorder(root);  printf("\n");
    printf("중위 순회 : "); inorder(root);   printf("\n");
    printf("후위 순회 : "); postorder(root); printf("\n");

    freeTree(root);
    return 0;
}
