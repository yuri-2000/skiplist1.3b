#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAX_LEVEL 4 // ������
#define _CRT_SECURE_NO_WARNINGS
#define MAX_STACK_SIZE 200


// ����ڵ�ṹ
typedef struct Node {
    int key;
    int height;
    int levelNode;
    int hash;
    char RorH[2];
    struct Node* prev; // ָ��ǰһ���ڵ�
    struct Node* next; // ָ����һ���ڵ�
    struct Node* down; // ָ��Ͳ�Ľڵ�
    struct Node* up;  // ָ��߲�Ľڵ�
} Node;

// ����ṹ
typedef struct SkipList {
    Node* heads[MAX_LEVEL]; // ÿ���ͷ�ڵ�
    int level;                  // ��ǰ����
} SkipList;

struct Result {
    struct Node** nodes;
    int length;
};

typedef struct {
    char data[MAX_STACK_SIZE];
    int top;
} Stack;

void initializeStack(Stack* stack) {
    stack->top = -1;
}

int isStackEmpty(Stack* stack) {
    return stack->top == -1;
}

int isStackFull(Stack* stack) {
    return stack->top == MAX_STACK_SIZE - 1;
}

void push(Stack* stack, char value) {
    if (isStackFull(stack)) {
        printf("Stack is full. Cannot push element.\n");
        return;
    }
    stack->data[++stack->top] = value;
}

char pop(Stack* stack) {
    if (isStackEmpty(stack)) {
        printf("Stack is empty. Cannot pop element.\n");
        return '\0';
    }
    return stack->data[stack->top--];
}

void printStack(Stack* stack) {
    if (isStackEmpty(stack)) {
        printf("Stack is empty\n");
        return;
    }

    printf("Stack elements: ");

    for (int i = 0; i <= stack->top; i++) {
        printf("%c ", stack->data[i]);
    }

    printf("\n");
}

// ��������ڵ�
Node* createNode(int key) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->key = key;
    newNode->hash = 0;
    newNode->height = 1;
    newNode->prev = NULL;
    newNode->next = NULL;
    newNode->down = NULL;
    newNode->up = NULL;
    newNode->levelNode = 1;
    return newNode;
}

// ��������
SkipList* createSkipList() {
    SkipList* newList = (SkipList*)malloc(sizeof(SkipList));
    for (int i = 0; i < MAX_LEVEL; i++) {
        newList->heads[i] = createNode(-1); // ͷ�ڵ��keyֵ��Ϊ-1��������ʵ�ʴ洢
        newList->heads[i]->down = (i > 0) ? newList->heads[i - 1] : NULL;
    }
    newList->level = 1;
    return newList;
}

// �ͷ�����ڵ��ڴ�
void freeNode(Node* node) {
    free(node);
}

// ��������
void destroySkipList(SkipList* list) {
    for (int i = 0; i < MAX_LEVEL; i++) {
        Node* current = list->heads[i];
        while (current != NULL) {
            Node* temp = current->next;
            freeNode(current);
            current = temp;
        }
    }
    free(list);
}

// ����һ��������������ڽڵ����ʱ�����ڵ�Ĳ���
int randomLevel() {
    int level = 1;
    while (rand() % 2 == 0 && level < MAX_LEVEL) {
        level++;
    }
    return level;
}

// �ȽϺ��������ڱȽ������ڵ�� key ֵ
int compareNodes(const void* a, const void* b) {
    Node* NodeA = *(Node**)a;
    Node* NodeB = *(Node**)b;

    if (NodeA == NULL && NodeB == NULL) {
        return 0;
    }
    else if (NodeA == NULL) {
        return 1;
    }
    else if (NodeB == NULL) {
        return -1;
    }

    // �ڵ㶼��Ϊ��ʱ���бȽ�
    if (NodeA->key < NodeB->key) {
        return -1;
    }
    else if (NodeA->key > NodeB->key) {
        return 1;
    }
    else {
        return 0;
    }
}

// �������в����µĽڵ�
void insertKey(SkipList* list, int key) {
    Node* update[MAX_LEVEL]; // ���ڼ�¼ÿ�����λ�õ�ǰһ���ڵ�
    Node* current = list->heads[list->level - 1];
    for (int i = list->level - 1; i >= 0; i--) {
        while (current->next != NULL && current->next->key < key) {
            current = current->next;
        }
        update[i] = current;
        if (current->down != NULL) {
            current = current->down;
        }
        else {
            break;
        }
    }
    current = current->next;

    if (current != NULL && current->key == key) {
        // ����ڵ��Ѵ��ڣ�ֱ�ӷ���
        return;
    }

    int newLevel = randomLevel();
    if (newLevel > list->level) {
        // ����½ڵ�Ĳ������ڵ�ǰ����Ĳ���������ͷ�ڵ�ָ������
        for (int i = list->level; i < newLevel; i++) {
            update[i] = list->heads[i];
        }
        list->level = newLevel;
    }

    Node* downNode = NULL; // ���ڱ�����һ��Ľڵ�
    Node* newNode = NULL; // �½ڵ�
    for (int i = 0; i < newLevel; i++) {
        newNode = createNode(key);
        newNode->levelNode = newLevel;
        newNode->next = update[i]->next;
        update[i]->next = newNode;

        // �������ڽڵ��prevָ��
        if (newNode->next != NULL) {
            newNode->next->prev = newNode;
        }
        newNode->prev = update[i];

        newNode->down = downNode;
        downNode = newNode;

        // �����Ϸ�ָ�룬ָ�����·��ڵ�� down ָ���෴�ķ���
        if (downNode->down != NULL) {
            downNode->down->up = newNode;
        }
        newNode->up = NULL;  // �½ڵ���Ϸ�ָ��Ϊ NULL
    }
}



// ��ӡ�����е����нڵ�
void printSkipList(SkipList* list) {
    printf("******Skiplist******\n");
    for (int i = list->level - 1; i >= 0; i--) {
        printf("Level %d: ", i + 1);
        Node* current = list->heads[i];
        current->height = i + 1;
        current = current->next;
        while (current != NULL) {
            printf("%d ", current->key);
            current->height = i + 1;
            current = current->next;
        }
        printf("\n");
    }
}

// ������ҽڵ��·��
void search(SkipList* list, int key, Node** searchPath) {
    printf("Search %d:  ", key);
    Node* current = list->heads[list->level - 1];
    searchPath[0] = current;
    int temp = 1;
    for (int i = list->level - 1; i >= 0; i--) {
        while (current->next != NULL && current->next->key <= key) {
            printf("L%d: %d -> ", i + 1, current->key);
            current = current->next;
            searchPath[temp] = current;
            temp++;
        }

        if (current->down != NULL) {
            current = current->down;
            searchPath[temp] = current;
            temp++;
            printf("L%d: %d -> ", i + 1, current->key);
        }
    }

    if (current != NULL && current->key == key) {
        printf("L%d: %d \n", current->height, current->key);
    }
    else
    {
        printf("Key %d not found\n", key);
    }
}


// ����ָ��
//void updatePoint(SkipList* list) {
//    Node* p = list->heads[0]->next;
//    Node* t = p;
//
//    // ������������
//    while (t != NULL) {
//        p = t;
//        t = p->next;
//        do {
//            if (p->next != NULL && p->next->up != NULL) {
//                p->next = NULL;
//            }
//            p = p->up;
//        } while (p != NULL);
//    }
//}

 //���¹�ϣֵ
void updateHash(SkipList* list) {
    Node* tail = list->heads[0]; // ��ײ�ͷ�ڵ�
    Node* checkPoint = list->heads[0];

    // ���������һ���ڵ�
    while (tail->next != NULL) {
        tail = tail->next;
    }
    Node* p = tail;
    Node* t = p;

    // �Ӻ���ǰ����
    while (t != checkPoint) {
        do {
            if (p->next != NULL) {
                if (p->down != NULL) {
                    if (p->height >= p->next->levelNode) {
                        p->hash = p->down->hash + p->next->hash;
                    }
                    else
                    {
                        p->hash = p->down->hash;
                    }
                }
                else if (p->height >= p->next->levelNode) {
                    p->hash = p->key + p->next->hash;
                }
                else
                {
                    p->hash = p->key;
                }
            }
            else if (p == tail) {
                p->hash = p->key;
            }
            else {
                p->hash = p->down->hash;
            }
            p = p->up;
        } while (p != NULL);

        // �Ժ���ǰ
        t = t->prev;
        p = t;
    }

    // ������ڵ�Ĺ�ϣֵ
    int temp = 0;
    int rootHash = 0;
    while (temp <= 3)
    {
        Node* r = list->heads[temp]->next;
        while (r->up != NULL)
        {
            r = r->up;
            temp += 1;
        }
        temp += 1;
        rootHash += r->hash;
    }

    list->heads[3]->hash = rootHash;
    printf("rootHash: %d\n", rootHash);

}

Node* findCommonPath(SkipList* list, int key1, int key2) {
    Node* searchPath1[20];
    Node* searchPath2[20];
    search(list, key1, searchPath1);
    search(list, key2, searchPath2);
    Node* commonNode = list->heads[0];

    for (int i = 0; i < 20; i++) {
        if (searchPath1[i] != searchPath2[i]) {
            break;
        }
        else
        {
            commonNode = searchPath1[i];
        }
    }

    printf("Common Node: L%d: %d, hash:%d\n", commonNode->height, commonNode->key, commonNode->hash);

    return commonNode;
}

void generateVO(SkipList* list, Node* commonNode, int low, int high) {
    struct Node* firstNode = list->heads[3];
    struct Node* secondNode = list->heads[2];
    struct Node* thirdNode = list->heads[1];
    struct Node* fourNode = list->heads[0];
    struct Node* quaryLastNode = NULL;
    struct Node* rangeArr[10];
    struct Node* voArr[10];

    for (int i = 0; i < 10; i++) {
        voArr[i] = (Node*)malloc(sizeof(Node));
        voArr[i]->key = 999;
    }

    int vo = 0;
    for (int i = 0; i < 10; i++) {
        rangeArr[i] = (struct Node*)malloc(sizeof(struct Node));
        rangeArr[i]->key = -1;
        rangeArr[i]->next = NULL;
        rangeArr[i]->prev = NULL;
        rangeArr[i]->down = NULL;
    }

    struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
    struct Node* voFirst = list->heads[3];
    struct Node* oneVo = voFirst->next;
    firstNode = oneVo;
    struct Node* secondVo = secondNode->next;
    struct Node* thirdVo = thirdNode->next;
    struct Node* fourVo = fourNode->next;

    struct Node* searchQuaryNode = NULL;
    if (fourVo->key != -1) {
        searchQuaryNode = fourVo;
    }
    else if (thirdVo->key != -1) {
        searchQuaryNode = thirdVo;
    }
    else if (secondVo->key != -1) {
        searchQuaryNode = secondVo;
    }

    struct Node* nodeArr[10];
    for (int i = 0; i < 10; i++) {
        nodeArr[i] = (struct Node*)malloc(sizeof(struct Node));
        nodeArr[i]->key = -1;
        nodeArr[i]->next = NULL;
        nodeArr[i]->prev = NULL;
        nodeArr[i]->down = NULL;
    }

    int e = 0;
    while (searchQuaryNode->next) {
        if (searchQuaryNode->key < low) {
            searchQuaryNode = searchQuaryNode->next;
        }
        else if (searchQuaryNode->key == low) {
            nodeArr[e] = searchQuaryNode->prev;
            e++;
            nodeArr[e] = searchQuaryNode;
            e++;
            searchQuaryNode = searchQuaryNode->next;
        }
        else if (searchQuaryNode->next && searchQuaryNode->key > low && searchQuaryNode->key <= high) {
            nodeArr[e] = searchQuaryNode;
            searchQuaryNode = searchQuaryNode->next;
            e++;
        }
        else if (searchQuaryNode->key == high + 1) {
            nodeArr[e] = searchQuaryNode;
            quaryLastNode = searchQuaryNode;
            e++;
            break;
        }
    }
    if (!searchQuaryNode->next) {
        nodeArr[e] = searchQuaryNode;
    }
    int len = sizeof(nodeArr) / sizeof(nodeArr[0]);
    printf("������Χ: ");
    int m = 0;
    for (int i = 0; i < len; i++) {
        if (nodeArr[i]->key != -1) {
            rangeArr[m] = nodeArr[i];
            m++;
            printf("%d R%d ", nodeArr[i]->key, nodeArr[i]->key);
            voArr[vo] = nodeArr[i];
            strcpy(voArr[vo]->RorH, "R");
            vo++;
        }
    }
    printf("\n");

    // ����vo
    struct Node* underNode = NULL;
    int preMaxLevel = 1;
    if (fourNode) {
        underNode = fourNode;
    }
    else if (thirdNode) {
        underNode = thirdNode;
    }
    else {
        underNode = secondNode;
    }
    while (underNode->next->key < low) {
        if (underNode->levelNode > preMaxLevel) {
            preMaxLevel = underNode->levelNode;
        }
        underNode = underNode->next;
    }

    int maxLevel = 1;
    struct Node* maxLevelNode = NULL;
    for (int i = 0; i < 10; i++) {
        if (rangeArr[i]->levelNode > maxLevel) {
            maxLevel = rangeArr[i]->levelNode;
            maxLevelNode = rangeArr[i];
        }
    }

    if (commonNode->key == -1 && preMaxLevel <= maxLevel) {
        if (maxLevelNode->key > low - 1) {
            maxLevelNode = maxLevelNode->prev;
            while (maxLevelNode->prev && maxLevelNode->prev->levelNode >= maxLevelNode->levelNode) {
                printf("R%d", maxLevelNode->prev->key);
                voArr[vo] = maxLevelNode->prev;
                strcpy(voArr[vo]->RorH, "R");
                vo++;
                maxLevelNode = maxLevelNode->prev;
            }
            struct Node* temp = NULL;
            while (maxLevelNode->prev) {
                if (maxLevelNode->prev->prev->levelNode >= maxLevelNode->prev->levelNode && maxLevelNode->prev->prev->key != -1) {
                    maxLevelNode = maxLevelNode->prev;
                }
                else {
                    temp = maxLevelNode->prev;
                    break;
                }
            }
            while (temp->prev) {
                if (temp->prev->levelNode >= maxLevelNode->levelNode && temp->prev->key != -1) {
                    maxLevelNode = temp;
                }
                temp = temp->prev;
            }
            printf("H%d", maxLevelNode->prev->key);
            voArr[vo] = maxLevelNode->prev;
            strcpy(voArr[vo]->RorH, "H");
            vo++;
        }
    }
    else if (commonNode->key != -1) {
        struct Node* minQuaryNode = NULL;
        while (maxLevelNode->key > low - 1) {
            maxLevelNode = maxLevelNode->prev;
        }
        minQuaryNode = maxLevelNode;
        int isFlag = 1;
        while (minQuaryNode->prev && minQuaryNode->key > commonNode->key) {
            if (minQuaryNode->levelNode <= minQuaryNode->prev->levelNode && isFlag == 1) {
                printf("R%d", minQuaryNode->prev->key);
                voArr[vo] = minQuaryNode->prev;
                strcpy(voArr[vo]->RorH, "R");
                vo++;
            }
            else {
                isFlag = 0;
            }
            if (minQuaryNode->levelNode <= minQuaryNode->prev->levelNode && isFlag == 0) {
                printf("H%d", minQuaryNode->prev->key);
                voArr[vo] = minQuaryNode->prev;
                strcpy(voArr[vo]->RorH, "H");
                vo++;
            }
            minQuaryNode = minQuaryNode->prev;
        }
    }
    qsort(voArr, 10, sizeof(Node*), compareNodes);
    printf("\n");

    char voStr[100] = "";

    for (int num = 0; num < 5; num++) {
        Node* p = voArr[num];

        // r or h + key
        char rh[100] = "";
        strcpy(rh, p->RorH);
        char keyStr[20];
        sprintf_s(keyStr, sizeof(keyStr), "%d", voArr[num]->key);
        strcat(rh, keyStr);

        // ������
        if (p->levelNode >= p->next->levelNode) {
            while (p) {
                if (p->next->key != NULL && p->next->key <= quaryLastNode->key) {
                    strcat(voStr, "[");
                }
                p = p->up;
            }
            strcat(voStr, rh);
            strcat(voStr, ",");
        }
        // ������
        else
        {
            Node* pre = commonNode;
            strcat(voStr, rh);
            while (pre) {
                while (pre->next->key <= p->key) {
                    pre = pre->next;
                    strcat(voStr, "]");
                }

                pre = pre->down;
            }
            if (num != 4) {
                strcat(voStr, ",");
            }
        }
    }

    printf("%s\n", voStr);

    Stack stack;
    initializeStack(&stack);
    int volen = strlen(voStr);
    int res = 0;

    for (int i = 0; i < volen; i++) {
        if (voStr[i] == ']') {
            // ��ϣ�����Ҳ�
            char element = pop(&stack);
            char b[64] = "";
            int blen = 64;
            while (element != 'R' && element != ',') {
                memmove(b + 1, b, blen + 1);
                b[0] = element;
                element = pop(&stack);
            }
            // ��ϣ�������
            if (element == 'R' || element == 'H') {
                element = pop(&stack);
                element = pop(&stack);
            }
            else {
                element = pop(&stack);
            }
            char a[64] = "";
            int alen = 64;
            while (element != 'R' && element != '[') {
                memmove(a + 1, a, alen + 1);
                a[0] = element;
                element = pop(&stack);
            }
            if (element == 'H' || element == 'R') {
                element = pop(&stack);
            }
            // ��ϣ����
            int A = atoi(a);
            int B = atoi(b);
            char resStr[20];
            sprintf_s(resStr, sizeof(resStr), "%d", A + B);
            //strcat(res, a);
            //strcat(res, b);
            //memmove(res + 1, res, rlen + 1);
            //res[0] = 'H';
            int reslen = strlen(resStr);
            if (isStackEmpty(&stack)) {
                res = atoi(resStr);
            }
            for (int i = 0; i < reslen; i++) {
                push(&stack, resStr[i]);
            }
        }
        else
        {
            push(&stack, voStr[i]);
        }
    }

    printStack(&stack);
    if (res == commonNode->hash) {
        printf("��֤�ɹ���\n");
    }

}

int main() {
    // �������������
    srand(3);

    SkipList* list = createSkipList();

    for (int i = 1; i < 10; i++) {
        insertKey(list, i);
    }

    printSkipList(list);
    updateHash(list);
    Node* commonNode = findCommonPath(list, 5, 8);
    generateVO(list, commonNode, 6, 7);

    destroySkipList(list);

    return 0;
}
