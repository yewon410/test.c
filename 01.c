#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT 1024
#define MAX_NODES 26

int main(void)
{
    char input[MAX_INPUT];
    char tree[MAX_INPUT];
    int length = 0;

    char nodeStack[MAX_NODES];
    int counterStack[MAX_NODES];
    int top = -1;

    int used[MAX_NODES] = {0};

    int totalNodes = 0;
    int nonLeafNodes = 0;
    int height = 0;
    int degree = 0;

    char parentOfC = '\0';
    char childrenOfC[MAX_NODES];
    int childCountOfC = 0;

    int expectNode = 1;
    int valid = 1;

    printf("트리의 괄호 표기법을 입력하세요: ");

    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("오류: 입력을 읽을 수 없습니다.\n");
        return 1;
    }

    if (strchr(input, '\n') == NULL) {
        int next = getchar();

        if (next != '\n' && next != EOF) {
            printf("오류: 입력 문자열이 너무 깁니다.\n");
            return 1;
        }
    }

    for (int i = 0; input[i] != '\0'; i++) {
        if (!isspace((unsigned char)input[i])) {
            tree[length++] = input[i];
        }
    }
    tree[length] = '\0';

    if (length == 0 || tree[0] != 'A') {
        printf("오류: 루트 노드는 A여야 합니다.\n");
        return 1;
    }

    for (int i = 0; i < length && valid; i++) {
        char ch = tree[i];

        if (ch >= 'A' && ch <= 'Z') {
            if (!expectNode || used[ch - 'A']) {
                valid = 0;
                break;
            }

            if (top == -1 && totalNodes != 0) {
                valid = 0;
                break;
            }

            used[ch - 'A'] = 1;
            totalNodes++;

            int depth = top + 1;
            if (depth > height) {
                height = depth;
            }

            if (top >= 0) {
                counterStack[top]++;

                if (ch == 'C') {
                    parentOfC = nodeStack[top];
                }

                if (nodeStack[top] == 'C') {
                    int position = counterStack[top] - 1;
                    childrenOfC[position] = ch;
                    childCountOfC = counterStack[top];
                }
            }

            expectNode = 0;
        }
        else if (ch == '(') {
            if (expectNode || i == 0 ||
                tree[i - 1] < 'A' || tree[i - 1] > 'Z' ||
                top + 1 >= MAX_NODES) {
                valid = 0;
                break;
            }

            top++;
            nodeStack[top] = tree[i - 1];
            counterStack[top] = 0;

            nonLeafNodes++;
            expectNode = 1;
        }
        else if (ch == ',') {
            if (expectNode || top < 0) {
                valid = 0;
                break;
            }

            expectNode = 1;
        }
        else if (ch == ')') {
            if (expectNode || top < 0) {
                valid = 0;
                break;
            }

            if (counterStack[top] > degree) {
                degree = counterStack[top];
            }

            top--;
            expectNode = 0;
        }
        else {
            valid = 0;
        }
    }

    if (top != -1 || expectNode) {
        valid = 0;
    }

    if (valid) {
        for (int i = 0; i < totalNodes; i++) {
            if (!used[i]) {
                valid = 0;
                break;
            }
        }
    }

    if (!valid) {
        printf("오류: 올바른 트리의 괄호 표기법이 아닙니다.\n");
        printf("괄호와 쉼표의 위치를 확인하고, "
               "노드는 A부터 연속된 대문자를 중복 없이 사용하세요.\n");
        return 1;
    }

    printf("\n전체 노드의 수: %d\n", totalNodes);
    printf("단말 노드의 수: %d\n", totalNodes - nonLeafNodes);
    printf("비단말 노드의 수: %d\n", nonLeafNodes);
    printf("트리의 높이: %d\n", height);
    printf("트리의 차수: %d\n", degree);

    if (!used['C' - 'A']) {
        printf("노드 C의 부모 노드: C가 존재하지 않음\n");
        printf("노드 C의 자식 노드: C가 존재하지 않음\n");
    }
    else {
        printf("노드 C의 부모 노드: ");
        if (parentOfC != '\0') {
            printf("%c\n", parentOfC);
        }
        else {
            printf("없음\n");
        }

        printf("노드 C의 자식 노드: ");
        if (childCountOfC == 0) {
            printf("없음\n");
        }
        else {
            for (int i = 0; i < childCountOfC; i++) {
                if (i > 0) {
                    printf(", ");
                }
                printf("%c", childrenOfC[i]);
            }
            printf("\n");
        }
    }

    printf("\n[트리 계층 구조]\n");

    int depth = 0;

    for (int i = 0; i < length; i++) {
        char ch = tree[i];

        if (ch == '(') {
            depth++;
        }
        else if (ch == ')') {
            depth--;
        }
        else if (ch >= 'A' && ch <= 'Z') {
            if (depth == 0) {
                printf("%c\n", ch);
            }
            else {
                for (int j = 1; j < depth; j++) {
                    printf("    ");
                }
                printf("+---%c\n", ch);
            }
        }
    }

    return 0;
}

