#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int numberOfSteps = 0;

// NODE STRUCTURE BEING //

typedef struct node{
    int *board;
    struct node *parent;
    int move;
    int boardSize;
    int distance;
} Node;


Node *initialiseNode(const int *board, Node *previous, int boardSize, int move){
    Node *newNode = malloc(sizeof(Node));
    newNode->board =(int*)malloc(boardSize * sizeof(int));
    for(int i = 0; i < boardSize; i ++){
        newNode->board[i] = board[i];
    }
    newNode->parent = previous;
    newNode->move = move;
    newNode->boardSize = boardSize;
    newNode->distance = newNode->parent->distance + 1;
    return newNode;
}

Node *initialiseFirstNode(const int *board, Node *previous, int boardSize, int move, int distance){
    Node *newNode = malloc(sizeof(Node));
    newNode->board =(int*)malloc(boardSize * sizeof(int));
    for(int i = 0; i < boardSize; i ++){
        newNode->board[i] = board[i];
    }
    newNode->parent = previous;
    newNode->move = move;
    newNode->boardSize = boardSize;
    newNode->distance = distance;
    return newNode;
}

// NODE STRUCTURE END //

// HASH TABLE STRUCTURE BEGIN //

int Hash(const int *arr, int length){
    unsigned int hashValue = 0;
    for(int i = 0; i < length; i++){
        hashValue = (hashValue + (arr[i] * i)) * 31;
    }
    return hashValue % 5000;
}

typedef struct HT_node{
    Node *node;
    struct HT_node *next;
} HT_Node;

HT_Node *initialiseHTNode(Node *node, HT_Node *next){
    HT_Node *newNode = (HT_Node*)malloc(sizeof(HT_Node));
    newNode->node = node;
    newNode->next = next;
    return newNode;
}

typedef struct openHashTable{
    HT_Node **table;
} OpenHashTable;

// Initialise new Hash Table
OpenHashTable *initializeHashTable(int size, int boardLength){
    OpenHashTable *hashTable = malloc(sizeof(OpenHashTable));
    hashTable->table = malloc(size * sizeof(HT_Node));
    for(int i = 0; i < size; i++) {
        hashTable->table[i] = NULL;
    }
    return hashTable;
}

// Insert new element into hash table
void insert(OpenHashTable *hashTable, Node *node){
    int position = Hash(node->board, node->boardSize);
    hashTable->table[position] = initialiseHTNode(node,hashTable->table[position]);
}

int isHTMember(OpenHashTable *hashTable, int *board, int boardLength){
    int position = Hash(board, boardLength);
    HT_Node *ptr = hashTable->table[position];
    int trigger;
    while(ptr != NULL){
        trigger = 0;
        for(int i = 0; i < boardLength; i++){
            if(ptr->node->board[i] != board[i]){
                trigger = 1;
            }
        }
        if(trigger == 0)
            return 1;
        else
            ptr=ptr->next;
    }
    return 0;

}

void printHashTable(OpenHashTable *hashTable){
    for(int i = 0; i < 5000; i++){
        printf("Row %d: [", i);
        HT_Node *ptr = hashTable->table[i];
        while(ptr!=NULL){
            for(int i = 0; i < ptr->node->boardSize; i++){
                printf("%d ", ptr->node->board[i]);
            }
            printf(", ");
            ptr = ptr->next;
        }
        printf(" ]\n");
    }
}

// HASH TABLE STRUCTURE END //

// PRIORITY QUEUE BEGIN//


int manhattanH(int *A, int distance, int size){
    int edge = sqrt(size);
    int rowGoal;
    int columnGoal;
    int rowCurrent;
    int columnCurrent;
    int testA[size];
    int total = 0;

    for(int i = 0; i < size-1; i++){
        testA[i] = i + 1;
    }
    testA[size-1] = 0;

    for(int i = 0; i < size; i++){
        if(A[i] != 0){
            for(int j = 0; j < size; j++){
                if(A[i] == testA[j]){
                    rowGoal = j / edge;
                    columnGoal = j % edge;
                    columnCurrent = i % edge;
                    rowCurrent = i / edge;
                    total = total + abs(columnCurrent - columnGoal) + abs(rowCurrent - rowGoal);
                }
            }
        }
    }
    total = total + distance;
    return total;
}
typedef struct queueNode{
    Node *node;
    int priority;
    struct queueNode *link;
} QueueNode;

QueueNode *front = NULL;

// insert method
void push(Node *node){
    QueueNode *temp, *q;
    int priority = manhattanH(node->board,node->distance,node->boardSize);
    temp = (QueueNode *)malloc(sizeof(QueueNode));
    temp->node = node;
    temp->priority = priority;
    if( front == NULL || priority < front->priority ){
        temp->link = front;
        front = temp;
    }
    else{
        q = front;
        while( q->link != NULL && q->link->priority <= priority )
            q=q->link;
        temp->link = q->link;
        q->link = temp;
    }
}

void pop() {
    QueueNode *temp;
    if (front == NULL)
        printf("Queue Underflow\n");
    else {
        temp = front;
        front = front->link;
        free(temp);
    }
}

Node *peek(){
    QueueNode *ptr;
    ptr = front;
    if(front == NULL) {
        printf("Queue is empty\n");
        return NULL;
    }
    else{
        return ptr->node;
    }
}

// Try to open a file
int tryOpen(FILE *fp){
    if (fp == NULL){
        printf("Could not open a file.\n");
        return -1;
    }
    return 0;
}


// Creates all possible children for a given node
void createChildren(Node *node, OpenHashTable *ht){
    int edge = sqrt(node->boardSize);
    int count = 0;
    int tempVariable;
    int temp2D[edge][edge];
    int tempLinear[node->boardSize];
    Node *n1 = NULL;
    Node *n2 = NULL;
    Node *n3 = NULL;
    Node *n4 = NULL;

    for(int i = 0; i < edge; i++){
        for(int j = 0; j < edge; j++){
            temp2D[i][j] = node->board[count];
            count++;
        }
    }
    count = 0;
    while (node->board[count] != 0){
        count++;
    }

    int i = count % edge; //column
    int j = count / edge; //row

    count = 0;
    // CASE 1 - 0 is not touching any edge
    if(i != 0 && i < edge - 1 && j != 0 && j < edge - 1){
        // SWITCH DOWN ONE ROW
        tempVariable = temp2D[j+1][i];
        temp2D[j+1][i] = temp2D[j][i];
        temp2D[j][i] = tempVariable;
        for(int x = 0; x < edge; x++){
            for(int y = 0; y < edge; y++){
                tempLinear[count] = temp2D[x][y];
                count++;
            }
        }
        if(isHTMember(ht, tempLinear, node->boardSize) == 0){
            n1 = initialiseNode(tempLinear, node, node->boardSize, tempVariable);
            insert(ht, n1);
        }

        // RETURN TO NORMAL
        tempVariable = temp2D[j+1][i];
        temp2D[j+1][i] = temp2D[j][i];
        temp2D[j][i] = tempVariable;

        // SWITCH UP ONE ROW
        tempVariable = temp2D[j-1][i];
        temp2D[j-1][i] = temp2D[j][i];
        temp2D[j][i] = tempVariable;
        count = 0;

        for(int x = 0; x < edge; x++){
            for(int y = 0; y < edge; y++){
                tempLinear[count] = temp2D[x][y];
                count++;
            }
        }
        if(isHTMember(ht, tempLinear, node->boardSize) == 0){
            n2 = initialiseNode(tempLinear, node, node->boardSize, tempVariable);
            insert(ht, n2);
        }

        // RETURN TO NORMAL
        tempVariable = temp2D[j-1][i];
        temp2D[j-1][i] = temp2D[j][i];
        temp2D[j][i] = tempVariable;
        count = 0;

        tempVariable = temp2D[j][i-1];
        temp2D[j][i-1] = temp2D[j][i];
        temp2D[j][i] = tempVariable;

        for(int x = 0; x < edge; x++){
            for(int y = 0; y < edge; y++){
                tempLinear[count] = temp2D[x][y];
                count++;
            }
        }
        if(isHTMember(ht, tempLinear, node->boardSize) == 0){
            n3 = initialiseNode(tempLinear, node, node->boardSize, tempVariable);
            insert(ht, n3);
        }

        // RETURN TO NORMAL
        tempVariable = temp2D[j][i-1];
        temp2D[j][i-1] = temp2D[j][i];
        temp2D[j][i] = tempVariable;
        count = 0;

        tempVariable = temp2D[j][i+1];
        temp2D[j][i+1] = temp2D[j][i];
        temp2D[j][i] = tempVariable;
        for(int x = 0; x < edge; x++){
            for(int y = 0; y < edge; y++){
                tempLinear[count] = temp2D[x][y];
                count++;
            }
        }
        if(isHTMember(ht, tempLinear, node->boardSize) == 0){
            n4 = initialiseNode(tempLinear, node, node->boardSize, tempVariable);
            insert(ht, n4);

        }
        //pop();
        if(n1 != NULL){
            push(n1);
        }
        if(n2 != NULL){
            push(n2);
        }
        if(n3 != NULL){
            push(n3);
        }
        if(n4 != NULL){
            push(n4);
        }
    }
        // CASE 2 - 0 is touching top edge
    else if(j == 0){
        // CASE 2.1 - 0 is touches the right edge as well
        if(i == 0){
            tempVariable = temp2D[j+1][i];
            temp2D[j+1][i] = temp2D[j][i];
            temp2D[j][i] = tempVariable;
            count = 0;

            for(int x = 0; x < edge; x++){
                for(int y = 0; y < edge; y++){
                    tempLinear[count] = temp2D[x][y];
                    count++;
                }
            }
            if(isHTMember(ht, tempLinear, node->boardSize) == 0){
                n1 = initialiseNode(tempLinear, node, node->boardSize, tempVariable);
                insert(ht, n1);
            }

            // RETURN TO NORMAL
            tempVariable = temp2D[j+1][i];
            temp2D[j+1][i] = temp2D[j][i];
            temp2D[j][i] = tempVariable;

            tempVariable = temp2D[j][i+1];
            temp2D[j][i+1] = temp2D[j][i];
            temp2D[j][i] = tempVariable;
            count = 0;

            for(int x = 0; x < edge; x++){
                for(int y = 0; y < edge; y++){
                    tempLinear[count] = temp2D[x][y];
                    count++;
                }
            }
            if(isHTMember(ht, tempLinear, node->boardSize) == 0){
                n2 = initialiseNode(tempLinear, node, node->boardSize, tempVariable);
                insert(ht, n2);
            }

            //pop();
            if(n1 != NULL){
                push(n1);
            }
            if(n2 != NULL){
                push(n2);
            }
        }

            // CASE 2.2 - 0 is touching right edge
        else if(i == (edge - 1)){
            tempVariable = temp2D[j][i-1];
            temp2D[j][i-1] = temp2D[j][i];
            temp2D[j][i] = tempVariable;
            count = 0;

            for(int x = 0; x < edge; x++){
                for(int y = 0; y < edge; y++){
                    tempLinear[count] = temp2D[x][y];
                    count++;
                }
            }
            if(isHTMember(ht, tempLinear, node->boardSize) == 0){
                n1 = initialiseNode(tempLinear, node, node->boardSize, tempVariable);
                insert(ht, n1);
            }

            //RESET TO NORMAL
            tempVariable = temp2D[j][i-1];
            temp2D[j][i-1] = temp2D[j][i];
            temp2D[j][i] = tempVariable;
            count = 0;

            tempVariable = temp2D[j+1][i];
            temp2D[j+1][i] = temp2D[j][i];
            temp2D[j][i] = tempVariable;

            for(int x = 0; x < edge; x++){
                for(int y = 0; y < edge; y++){
                    tempLinear[count] = temp2D[x][y];
                    count++;
                }
            }
            if(isHTMember(ht, tempLinear, node->boardSize) == 0){
                n2 = initialiseNode(tempLinear, node, node->boardSize, tempVariable);
                insert(ht, n2);
            }

            //pop();
            if(n1 != NULL){
                push(n1);
            }
            if(n2 != NULL){
                push(n2);
            }
        }
        else{
            tempVariable = temp2D[j][i-1];
            temp2D[j][i-1] = temp2D[j][i];
            temp2D[j][i] = tempVariable;
            count = 0;

            for(int x = 0; x < edge; x++){
                for(int y = 0; y < edge; y++){
                    tempLinear[count] = temp2D[x][y];
                    count++;
                }
            }
            if(isHTMember(ht, tempLinear, node->boardSize) == 0){
                n1 = initialiseNode(tempLinear, node, node->boardSize, tempVariable);
                insert(ht, n1);
            }

            //RESET TO NORMAL
            tempVariable = temp2D[j][i-1];
            temp2D[j][i-1] = temp2D[j][i];
            temp2D[j][i] = tempVariable;
            count = 0;

            tempVariable = temp2D[j+1][i];
            temp2D[j+1][i] = temp2D[j][i];
            temp2D[j][i] = tempVariable;

            for(int x = 0; x < edge; x++){
                for(int y = 0; y < edge; y++){
                    tempLinear[count] = temp2D[x][y];
                    count++;
                }
            }
            if(isHTMember(ht, tempLinear, node->boardSize) == 0){
                n2 = initialiseNode(tempLinear, node, node->boardSize, tempVariable);
                insert(ht, n2);
            }

            //RETURN TO NORMAL
            tempVariable = temp2D[j+1][i];
            temp2D[j+1][i] = temp2D[j][i];
            temp2D[j][i] = tempVariable;
            count = 0;

            tempVariable = temp2D[j][i+1];
            temp2D[j][i+1] = temp2D[j][i];
            temp2D[j][i] = tempVariable;
            for(int x = 0; x < edge; x++){
                for(int y = 0; y < edge; y++){
                    tempLinear[count] = temp2D[x][y];
                    count++;
                }
            }
            if(isHTMember(ht, tempLinear, node->boardSize) == 0){
                n3 = initialiseNode(tempLinear, node, node->boardSize, tempVariable);
                insert(ht, n3);
            }

            //pop();
            if(n1 != NULL){
                push(n1);
            }
            if(n2 != NULL){
                push(n2);
            }
            if(n3 != NULL){
                push(n3);
            }
        }
    }

        // CASE 3 - 0 is touching the bottom edge
    else if(j == (edge - 1)){
        // CASE 3.1 - 0 is touching left edge
        if(i == 0){
            tempVariable = temp2D[j-1][i];
            temp2D[j-1][i] = temp2D[j][i];
            temp2D[j][i] = tempVariable;
            count = 0;

            for(int x = 0; x < edge; x++){
                for(int y = 0; y < edge; y++){
                    tempLinear[count] = temp2D[x][y];
                    count++;
                }
            }
            if(isHTMember(ht, tempLinear, node->boardSize) == 0){
                n1 = initialiseNode(tempLinear, node, node->boardSize, tempVariable);
                insert(ht, n1);
            }

            // RETURN TO NORMAL
            tempVariable = temp2D[j-1][i];
            temp2D[j-1][i] = temp2D[j][i];
            temp2D[j][i] = tempVariable;

            tempVariable = temp2D[j][i+1];
            temp2D[j][i+1] = temp2D[j][i];
            temp2D[j][i] = tempVariable;
            count = 0;

            for(int x = 0; x < edge; x++){
                for(int y = 0; y < edge; y++){
                    tempLinear[count] = temp2D[x][y];
                    count++;
                }
            }
            if(isHTMember(ht, tempLinear, node->boardSize) == 0){
                n2 = initialiseNode(tempLinear, node, node->boardSize, tempVariable);
                insert(ht, n2);
            }

            //pop();
            if(n1 != NULL){
                push(n1);
            }
            if(n2 != NULL){
                push(n2);
            }
        }

            // CASE 3.2 - 0 is touching the right edge.
        else if(i == (edge - 1)){
            tempVariable = temp2D[j][i-1];
            temp2D[j][i-1] = temp2D[j][i];
            temp2D[j][i] = tempVariable;
            count = 0;

            for(int x = 0; x < edge; x++){
                for(int y = 0; y < edge; y++){
                    tempLinear[count] = temp2D[x][y];
                    count++;
                }
            }
            if(isHTMember(ht, tempLinear, node->boardSize) == 0){
                n1 = initialiseNode(tempLinear, node, node->boardSize, tempVariable);
                insert(ht, n1);
            }

            //RESET TO NORMAL
            tempVariable = temp2D[j][i-1];
            temp2D[j][i-1] = temp2D[j][i];
            temp2D[j][i] = tempVariable;
            count = 0;

            tempVariable = temp2D[j-1][i];
            temp2D[j-1][i] = temp2D[j][i];
            temp2D[j][i] = tempVariable;

            for(int x = 0; x < edge; x++){
                for(int y = 0; y < edge; y++){
                    tempLinear[count] = temp2D[x][y];
                    count++;
                }
            }
            if(isHTMember(ht, tempLinear, node->boardSize) == 0){
                n2 = initialiseNode(tempLinear, node, node->boardSize, tempVariable);
                insert(ht, n2);
            }

            //pop();
            if(n1 != NULL){
                push(n1);
            }
            if(n2 != NULL){
                push(n2);
            }
        }
        else{
            tempVariable = temp2D[j][i-1];
            temp2D[j][i-1] = temp2D[j][i];
            temp2D[j][i] = tempVariable;
            count = 0;

            for(int x = 0; x < edge; x++){
                for(int y = 0; y < edge; y++){
                    tempLinear[count] = temp2D[x][y];
                    count++;
                }
            }
            if(isHTMember(ht, tempLinear, node->boardSize) == 0){
                n1 = initialiseNode(tempLinear, node, node->boardSize, tempVariable);
                insert(ht, n1);
            }

            //RESET TO NORMAL
            tempVariable = temp2D[j][i-1];
            temp2D[j][i-1] = temp2D[j][i];
            temp2D[j][i] = tempVariable;
            count = 0;

            tempVariable = temp2D[j-1][i];
            temp2D[j-1][i] = temp2D[j][i];
            temp2D[j][i] = tempVariable;

            for(int x = 0; x < edge; x++){
                for(int y = 0; y < edge; y++){
                    tempLinear[count] = temp2D[x][y];
                    count++;
                }
            }
            if(isHTMember(ht, tempLinear, node->boardSize) == 0){
                n2 = initialiseNode(tempLinear, node, node->boardSize, tempVariable);
                insert(ht, n2);
            }

            //RETURN TO NORMAL
            tempVariable = temp2D[j-1][i];
            temp2D[j-1][i] = temp2D[j][i];
            temp2D[j][i] = tempVariable;
            count = 0;

            tempVariable = temp2D[j][i+1];
            temp2D[j][i+1] = temp2D[j][i];
            temp2D[j][i] = tempVariable;
            for(int x = 0; x < edge; x++){
                for(int y = 0; y < edge; y++){
                    tempLinear[count] = temp2D[x][y];
                    count++;
                }
            }
            if(isHTMember(ht, tempLinear, node->boardSize) == 0){
                n3 = initialiseNode(tempLinear, node, node->boardSize, tempVariable);
                insert(ht, n3);
            }
            //pop();
            if(n1 != NULL){
                push(n1);
            }
            if(n2 != NULL){
                push(n2);
            }
            if(n3 != NULL){
                push(n3);
            }
        }
    }
        // Case 4 - 0 i not touching up or down, but is touching the left edge
    else if(i == 0){
        tempVariable = temp2D[j-1][i];
        temp2D[j-1][i] = temp2D[j][i];
        temp2D[j][i] = tempVariable;
        count = 0;

        for(int x = 0; x < edge; x++){
            for(int y = 0; y < edge; y++){
                tempLinear[count] = temp2D[x][y];
                count++;
            }
        }
        if(isHTMember(ht, tempLinear, node->boardSize) == 0){
            n1 = initialiseNode(tempLinear, node, node->boardSize, tempVariable);
            insert(ht, n1);
        }

        // RETURN TO NORMAL
        tempVariable = temp2D[j-1][i];
        temp2D[j-1][i] = temp2D[j][i];
        temp2D[j][i] = tempVariable;

        tempVariable = temp2D[j][i+1];
        temp2D[j][i+1] = temp2D[j][i];
        temp2D[j][i] = tempVariable;
        count = 0;

        for(int x = 0; x < edge; x++){
            for(int y = 0; y < edge; y++){
                tempLinear[count] = temp2D[x][y];
                count++;
            }
        }
        if(isHTMember(ht, tempLinear, node->boardSize) == 0){
            n2 = initialiseNode(tempLinear, node, node->boardSize, tempVariable);
            insert(ht, n2);
        }

        // RETURN TO NORMAL
        tempVariable = temp2D[j][i+1];
        temp2D[j][i+1] = temp2D[j][i];
        temp2D[j][i] = tempVariable;

        tempVariable = temp2D[j+1][i];
        temp2D[j+1][i] = temp2D[j][i];
        temp2D[j][i] = tempVariable;
        count = 0;

        for(int x = 0; x < edge; x++){
            for(int y = 0; y < edge; y++){
                tempLinear[count] = temp2D[x][y];
                count++;
            }
        }
        if(isHTMember(ht, tempLinear, node->boardSize) == 0){
            n3 = initialiseNode(tempLinear, node, node->boardSize, tempVariable);
            insert(ht, n3);
        }
        //pop();
        if(n1 != NULL){
            push(n1);
        }
        if(n2 != NULL){
            push(n2);
        }
        if(n3 != NULL){
            push(n3);
        }
    }

        //CASE 5 - 0 is not touching up or down, but only the right edge
    else if(i == (edge - 1)){
        tempVariable = temp2D[j][i-1];
        temp2D[j][i-1] = temp2D[j][i];
        temp2D[j][i] = tempVariable;
        count = 0;

        for(int x = 0; x < edge; x++){
            for(int y = 0; y < edge; y++){
                tempLinear[count] = temp2D[x][y];
                count++;
            }
        }
        if(isHTMember(ht, tempLinear, node->boardSize) == 0){
            n1 = initialiseNode(tempLinear, node, node->boardSize, tempVariable);
            insert(ht, n1);
        }

        //RESET TO NORMAL
        tempVariable = temp2D[j][i-1];
        temp2D[j][i-1] = temp2D[j][i];
        temp2D[j][i] = tempVariable;
        count = 0;

        tempVariable = temp2D[j-1][i];
        temp2D[j-1][i] = temp2D[j][i];
        temp2D[j][i] = tempVariable;

        for(int x = 0; x < edge; x++){
            for(int y = 0; y < edge; y++){
                tempLinear[count] = temp2D[x][y];
                count++;
            }
        }
        if(isHTMember(ht, tempLinear, node->boardSize) == 0){
            n2 = initialiseNode(tempLinear, node, node->boardSize, tempVariable);
            insert(ht, n2);
        }

        // RETURN TO NORMAL
        tempVariable = temp2D[j-1][i];
        temp2D[j-1][i] = temp2D[j][i];
        temp2D[j][i] = tempVariable;
        count = 0;

        tempVariable = temp2D[j+1][i];
        temp2D[j+1][i] = temp2D[j][i];
        temp2D[j][i] = tempVariable;

        for(int x = 0; x < edge; x++){
            for(int y = 0; y < edge; y++){
                tempLinear[count] = temp2D[x][y];
                count++;
            }
        }
        if(isHTMember(ht, tempLinear, node->boardSize) == 0){
            n3 = initialiseNode(tempLinear, node, node->boardSize, tempVariable);
            insert(ht, n3);
        }
        //pop();
        if(n1 != NULL){
            push(n1);
        }
        if(n2 != NULL){
            push(n2);
        }
        if(n3 != NULL){
            push(n3);
        }
    }
}

void Solution(Node *node, FILE *fp_out){
    int solution[numberOfSteps];
    int count = 0;

    while (node->parent != NULL) {
        solution[count] = node->move;
        count++;
        node = node->parent;
    }
    fprintf(fp_out, "#moves\n");
    for(int i = numberOfSteps - 1; i >= 0; i--){
        fprintf(fp_out, "%d ", solution[i]);
    }
    fprintf(fp_out, "\n");

}

void initialiseSolutionArray(Node *node){
    Node *temp = node;
    while (temp->parent != NULL){
        numberOfSteps++;
        temp = temp->parent;
    }
}

int findThePath(Node *node, OpenHashTable *ht, FILE *fp_out){
    int test = 0;
    for(int i = 0; i < node->boardSize - 1; i++){
        if(node->board[i] != (i + 1)){
            test = 1;
        }
    }
    if(node->board[node->boardSize - 1] != 0){
        test = 1;
    }
    if(test == 0){
        initialiseSolutionArray(node);
        Solution(node, fp_out);
        return 0;
    }
    else{
        pop();
        createChildren(node, ht);
        findThePath(peek(), ht, fp_out);
    }
}

int isSolvable(int *arr, int size){
    int rows = sqrt(size);
    int inversions = 0;
    int locationOfZero = 0;
    for(int i = 0; i < size - 1; i++){
        for(int j = i + 1; j < size; j++){
            if(arr[i] != 0 && arr[j] != 0){
                if(arr[i] > arr[j]){
                    inversions++;
                }
            }
            else{
                locationOfZero = i;
            }
        }
    }
    locationOfZero = locationOfZero / rows;
    if(rows % 2 == 1){
        if(inversions % 2 != 0){
            return 1;
        }
    }
    if(rows % 2 == 0){
        if(((rows-1) - locationOfZero) % 2 == 0){
            if(inversions % 2 == 0){
                return 0;
            }
            else{
                return 1;
            }
        }
        if(((rows-1)- locationOfZero) % 2 == 1){
            if(inversions % 2 == 1){
                return 0;
            }
            else{
                return 1;
            }
        }
    }
    return 0;
}

int main(int argc, char **argv){
    FILE *fp_in,*fp_out;

    fp_in = fopen(argv[1], "r");
    tryOpen(fp_in);

    fp_out = fopen(argv[2], "w");
    tryOpen(fp_out);

    char *line = NULL;
    size_t lineBuffSize = 0;
    ssize_t lineSize;
    int k;

    getline(&line,&lineBuffSize,fp_in);//ignore the first line in file, which is a comment
    fscanf(fp_in,"%d\n",&k);//read size of the board
    getline(&line,&lineBuffSize,fp_in);//ignore the second line in file, which is a comment
    int size = k * k;
    int initial_board[size];
    for(int i = 0; i < size; i++)
        fscanf(fp_in,"%d ",&initial_board[i]);

    fclose(fp_in);
    if(isSolvable(initial_board, size) == 0) {
        OpenHashTable *ht = initializeHashTable(5000, size);
        Node *initialState = initialiseFirstNode(initial_board, NULL, size, 0, 0);
        push(initialState);
        insert(ht, initialState);

        findThePath(initialState, ht, fp_out);
        
        for(int i = 0; i < 5000; i++) {
            free(ht->table[i]);
        }
    }
    else{
        fprintf(fp_out, "#moves\nno solution");
    }

    fclose(fp_out);
    return 0;
}
