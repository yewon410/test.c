#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 1024

typedef struct Node {
    char data;
    struct Node *left, *right;
} Node;

char input[MAX], tree[MAX];
int used[MAX], pos = 0;

Node* newNode(char data) {
    Node* p = (Node*)malloc(sizeof(Node));
    p->data = data; p->left = p->right = NULL;
    return p;
}

Node* makeTree() {
    if (!input[pos] || input[pos] == ')') return NULL;
    if (input[pos] == '(') {
        pos++;
        Node* p = makeTree();
        if (input[pos] == ',') pos++;
        if (p) p->right = makeTree();
        if (input[pos] == ')') pos++;
        return p;
    }
    Node* p = newNode(input[pos++]);
    if (input[pos] == '(') {
        pos++;
        p->left = makeTree();
        if (input[pos] == ',') pos++;
        p->right = makeTree();
        if (input[pos] == ')') pos++;
    }
    return p;
}

void makeArray(Node *p, int i) {
    if (!p || i >= MAX) return;
    tree[i] = p->data; used[i] = 1;
    makeArray(p->left, i * 2);
    makeArray(p->right, i * 2 + 1);
}

// 트리 계층 출력 (오른쪽 자식 -> 루트 -> 왼쪽 자식 순)
void printArray(int i, int level) {
    if (i >= MAX || !used[i]) return;
    printArray(i * 2 + 1, level + 1);
    for (int j = 0; j < level; j++) printf("    ");
    printf("%c\n", tree[i]);
    printArray(i * 2, level + 1);
}

void printPointer(Node *p, int level) {
    if (!p) return;
    printPointer(p->right, level + 1);
    for (int i = 0; i < level; i++) printf("    ");
    printf("%c\n", p->data);
    printPointer(p->left, level + 1);
}

// 기본 정보 계측 함수
int countNode(Node *p) { return p ? 1 + countNode(p->left) + countNode(p->right) : 0; }
int leafNode(Node *p) { return p ? (!p->left && !p->right ? 1 : leafNode(p->left) + leafNode(p->right)) : 0; }
int heightNode(Node *p) {
    if (!p) return 0;
    int l = heightNode(p->left), r = heightNode(p->right);
    return (l > r ? l : r) + 1;
}

int degreeNode(Node *p) {
    if (!p) return 0;
    int d = (p->left != NULL) + (p->right != NULL);
    int l = degreeNode(p->left), r = degreeNode(p->right);
    if (l > d) d = l; if (r > d) d = r;
    return d;
}

// 형태 판별 (포인터 기반)
int completeNode(Node *root) {
    if (!root) return 1;
    Node *q[MAX]; int f = 0, r = 0, empty = 0;
    q[r++] = root;
    while (f < r) {
        Node *p = q[f++];
        if (p->left) { if (empty) return 0; q[r++] = p->left; } else empty = 1;
        if (p->right) { if (empty) return 0; q[r++] = p->right; } else empty = 1;
    }
    return 1;
}

int fullNode(Node *p) { return countNode(p) == (1 << heightNode(p)) - 1; }
int skewNode(Node *p) {
    if (!p) return 1;
    if (p->left && p->right) return 0;
    return skewNode(p->left) && skewNode(p->right);
}

// 노드 관계 탐색
Node* findParent(Node *p, char data) {
    if (!p) return NULL;
    if ((p->left && p->left->data == data) || (p->right && p->right->data == data)) return p;
    Node *r = findParent(p->left, data);
    return r ? r : findParent(p->right, data);
}

Node* findNode(Node *p, char data) {
    if (!p || p->data == data) return p;
    Node *r = findNode(p->left, data);
    return r ? r : findNode(p->right, data);
}

void relationArray(char data) {
    int i = 1;
    while (i < MAX && (!used[i] || tree[i] != data)) i++;
    if (i >= MAX) { printf("노드가 없습니다.\n"); return; }
    
    printf("노드: %c\n", data);
    printf("부모: %c\n", i == 1 ? '-' : tree[i / 2]);
    printf("왼쪽 자식: %c\n", used[i * 2] ? tree[i * 2] : '-');
    printf("오른쪽 자식: %c\n", used[i * 2 + 1] ? tree[i * 2 + 1] : '-');
    printf("형제: %c\n", i == 1 ? '-' : (i % 2 == 0 ? (used[i + 1] ? tree[i + 1] : '-') : (used[i - 1] ? tree[i - 1] : '-')));
}

void relationNode(Node *root, char data) {
    Node *p = findNode(root, data);
    Node *parent = findParent(root, data);
    if (!p) { printf("노드가 없습니다.\n"); return; }

    printf("노드: %c\n", data);
    printf("부모: %c\n", parent ? parent->data : '-');
    printf("왼쪽 자식: %c\n", p->left ? p->left->data : '-');
    printf("오른쪽 자식: %c\n", p->right ? p->right->data : '-');
    printf("형제: %c\n", !parent ? '-' : (parent->left == p ? (parent->right ? parent->right->data : '-') : (parent->left ? parent->left->data : '-')));
}

void freeTree(Node *p) {
    if (!p) return;
    freeTree(p->left); freeTree(p->right); free(p);
}

int main() {
    Node *root; char target;
    printf("괄호 표기법 입력: ");
    scanf("%999s", input);

    root = makeTree();
    makeArray(root, 1);

    int n = countNode(root), leaf = leafNode(root), h = heightNode(root);

    printf("\n===== 트리 정보 =====\n");
    printf("노드 수: %d / 단말: %d / 비단말: %d / 높이: %d / 차수: %d\n", n, leaf, n - leaf, h, degreeNode(root));
    printf("완전 이진트리: %s | 포화: %s | 편향: %s\n", 
           completeNode(root) ? "예" : "아니오", 
           fullNode(root) ? "예" : "아니오", 
           skewNode(root) ? "예" : "아니오");

    printf("\n[배열 출력]\n"); printArray(1, 0);
    printf("\n[포인터 출력]\n"); printPointer(root, 0);

    printf("\n확인할 노드 입력: ");
    scanf(" %c", &target);

    printf("\n--- 배열 구현 관계 ---\n"); relationArray(target);
    printf("\n--- 포인터 구현 관계 ---\n"); relationNode(root, target);

    printf("\n[메모리 분석] 배열: %lu bytes | 포인터 노드: %lu bytes\n",
           (unsigned long)(sizeof(tree) + sizeof(used)), (unsigned long)(n * sizeof(Node)));

    freeTree(root);
    return 0;
}
