#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT   2048
#define MAX_STACK   256

typedef struct Node {
    char data;
    struct Node *left;
    struct Node *right;
} Node;

Node *createNode(char data) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (node == NULL) {
        printf("오류: 메모리 할당에 실패했습니다.\n");
        exit(1);
    }

    node->data = data;
    node->left = NULL;
    node->right = NULL;

    return node;
}

typedef struct {
    Node *parent;
    int state;
} Frame;

Node *buildTreeFromString(const char *str, char *errorMsg) {
    Frame frameStack[MAX_STACK];
    int frameTop = -1;

    Node *last = NULL;
    int i = 0;
    int len = (int)strlen(str);

    while (i < len) {
        char c = str[i];

        if (isspace((unsigned char)c)) {
            i++;
            continue;
        }

        if (isalnum((unsigned char)c)) {
            if (last != NULL) {
                sprintf(errorMsg,
                        "'%c' 앞에 연산자(콤마 또는 괄호)가 필요합니다. (위치 %d)",
                        c, i);
                return NULL;
            }

            last = createNode(c);
            i++;
        }
        else if (c == '(') {
            if (last == NULL) {
                sprintf(errorMsg,
                        "'(' 앞에 노드 데이터가 필요합니다. (위치 %d)",
                        i);
                return NULL;
            }

            frameTop++;

            if (frameTop >= MAX_STACK) {
                sprintf(errorMsg,
                        "트리가 너무 깊어 스택 용량을 초과했습니다.");
                return NULL;
            }

            frameStack[frameTop].parent = last;
            frameStack[frameTop].state = 0;

            last = NULL;
            i++;
        }
        else if (c == ',') {
            if (frameTop < 0 || frameStack[frameTop].state != 0) {
                sprintf(errorMsg,
                        "콤마 ','의 위치가 올바르지 않습니다. (위치 %d)",
                        i);
                return NULL;
            }

            frameStack[frameTop].parent->left = last;
            frameStack[frameTop].state = 1;
            last = NULL;

            i++;
        }
        else if (c == ')') {
            if (frameTop < 0 || frameStack[frameTop].state != 1) {
                sprintf(errorMsg,
                        "괄호 ')'의 위치가 올바르지 않습니다. (위치 %d)",
                        i);
                return NULL;
            }

            frameStack[frameTop].parent->right = last;
            last = frameStack[frameTop].parent;
            frameTop--;

            i++;
        }
        else {
            sprintf(errorMsg,
                    "허용되지 않는 문자 '%c'가 있습니다. (위치 %d)",
                    c, i);
            return NULL;
        }
    }

    if (frameTop != -1) {
        sprintf(errorMsg,
                "괄호의 짝이 맞지 않습니다. (닫히지 않은 '(' 존재)");
        return NULL;
    }

    if (last == NULL) {
        sprintf(errorMsg,
                "입력이 비어 있거나 유효한 트리가 아닙니다.");
        return NULL;
    }

    return last;
}

void printSideways(Node *root, int depth) {
    if (root == NULL) return;

    Node *stack[MAX_STACK];
    int depthStack[MAX_STACK];
    int top = -1;

    stack[++top] = root;
    depthStack[top] = depth;

    while (top >= 0) {
        Node *cur = stack[top];
        int curDepth = depthStack[top];
        top--;

        for (int i = 0; i < curDepth; i++) {
            printf("\t");
        }

        printf("%c\n", cur->data);

        if (cur->left != NULL) {
            stack[++top] = cur->left;
            depthStack[top] = curDepth + 1;
        }

        if (cur->right != NULL) {
            stack[++top] = cur->right;
            depthStack[top] = curDepth + 1;
        }
    }
}

void preorder(Node *root) {
    printf("Preorder  : ");

    if (root == NULL) {
        printf("(빈 트리)\n");
        return;
    }

    Node *stack[MAX_STACK];
    int top = -1;

    stack[++top] = root;

    while (top >= 0) {
        Node *cur = stack[top--];

        printf("%c ", cur->data);

        if (cur->right != NULL) {
            stack[++top] = cur->right;
        }

        if (cur->left != NULL) {
            stack[++top] = cur->left;
        }
    }

    printf("\n");
}

void inorder(Node *root) {
    printf("Inorder   : ");

    Node *stack[MAX_STACK];
    int top = -1;
    Node *cur = root;

    while (cur != NULL || top >= 0) {
        while (cur != NULL) {
            stack[++top] = cur;
            cur = cur->left;
        }

        cur = stack[top--];

        printf("%c ", cur->data);

        cur = cur->right;
    }

    printf("\n");
}

void postorder(Node *root) {
    printf("Postorder : ");

    if (root == NULL) {
        printf("(빈 트리)\n");
        return;
    }

    Node *stack1[MAX_STACK];
    Node *stack2[MAX_STACK];

    int top1 = -1;
    int top2 = -1;

    stack1[++top1] = root;

    while (top1 >= 0) {
        Node *cur = stack1[top1--];

        stack2[++top2] = cur;

        if (cur->left != NULL) {
            stack1[++top1] = cur->left;
        }

        if (cur->right != NULL) {
            stack1[++top1] = cur->right;
        }
    }

    while (top2 >= 0) {
        printf("%c ", stack2[top2--]->data);
    }

    printf("\n");
}

void freeTree(Node *root) {
    if (root == NULL) return;

    Node *stack[MAX_STACK];
    int top = -1;

    stack[++top] = root;

    while (top >= 0) {
        Node *cur = stack[top--];

        if (cur->left != NULL) {
            stack[++top] = cur->left;
        }

        if (cur->right != NULL) {
            stack[++top] = cur->right;
        }

        free(cur);
    }
}

int main(void) {
    char input[MAX_INPUT];
    char errorMsg[256] = "";

    printf("괄호 표기법으로 이진트리를 입력하세요.\n");
    printf("예) A(B(D,E),C(,F))\n> ");

    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("오류: 입력을 읽을 수 없습니다.\n");
        return 1;
    }

    input[strcspn(input, "\n")] = '\0';

    Node *root = buildTreeFromString(input, errorMsg);

    if (root == NULL) {
        printf("\n[오류] 잘못된 괄호 표기법입니다: %s\n", errorMsg);
        return 1;
    }

    printf("\n입력된 이진트리 구조 (왼쪽으로 눕힌 형태, 오른쪽 자식이 위로 표시됨)\n");
    printf("--------------------------------------------------------\n");

    printSideways(root, 0);

    printf("--------------------------------------------------------\n\n");

    preorder(root);
    inorder(root);
    postorder(root);

    freeTree(root);

    return 0;
}
