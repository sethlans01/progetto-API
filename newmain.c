#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <limits.h>

#define COMMAND_LENGTH 18
#define MAX_CARS 512
#define MAX_INPUT 100000
#define NEW_STATION "aggiungi-stazione"
#define REMOVE_STATION "demolisci-stazione"
#define NEW_CAR "aggiungi-auto"
#define DESTROY_CAR "rottama-auto"
#define FIND_ROUTE "pianifica-percorso"
#define RED 'r'
#define BLACK 'b'
#define OK 'O'
#define NO 'N'
#define FORWARD 'F'
#define BACKWARDS 'B'
#define MAX_QUEUE_SIZE 100
#define WHITE 'w'
#define GRAY 'g'

//TODO: remove [DEBUG]

/*
 * SECTION FOR THE DECLARATION OF DATA STRUCTURES
 */
typedef struct Node{
    char color;
    struct Node *parent;
    struct Node *right;
    struct Node *left;
    int stationID;
    short carNumber;
    int cars[MAX_CARS];
    int maxPower;
} Node;

typedef Node** RBTree;

typedef struct Q{
    int data[MAX_QUEUE_SIZE];
    int head;
    int tail;
} Q;

typedef Q* Queue;

/*
 * SECTION FOR COMMANDS FUNCTIONS DECLARATION
 */
void addStation(int station, short car_number, int *cars_to_add, int maxPower);
void removeStation(int station);
void addCar(int station, int car);
void removeCar(int station, int car);
void findRoute(int source, int destination);

/*
 * SECTION FOR RB-TREE FUNCTIONS DECLARATION
 */
Node* findNode(RBTree T, int stationID);
Node* newNode(int stationID, short carNumber, int cars[], int maxPower);
Node* treeMinimum(Node* x);
void RBDelete(RBTree T, Node* z);
void RBDeleteFixup(RBTree T, Node* x);
void RBInsert(RBTree T, int stationID, short carNumber, int cars[], int maxPower);
void RBInsertFixup(RBTree T, Node* z);
void RBLeftRotate(RBTree T, Node* x);
void RBPrint(Node* x);
void RBRightRotate(RBTree T, Node* x);
void RBTransplant(RBTree T, Node* u, Node* v);

/*
 * SECTION FOR QUEUE FUNCTIONS DECLARATION
 */
char isEmpty(Queue queue);
char isFull(Queue queue);
int dequeue(Queue queue);
void enqueue(Queue queue, int value);

/*
 * SECTION FOR FIND ROUTE FUNCTIONS DECLARATION
 */
char findBackwardsRoute(int source, int destination);
char findForwardRoute(int source, int destination);
int bfs(RBTree adjacencyList, int source, int destination);
void ascendingAdd(Node* x, RBTree list, int source, int destination);
void bfsFor(Node* x, int maxDistanceReachable, int source, short distance, Queue queue);
void colorBFS(Node* x, int source);
void initializeAdjacencyList(RBTree adjacencyList, int source, int destination, char direction);


// Global variables
RBTree highway;

int main() {
    int exit = 0;

    // Initialize the highway with null
    highway = malloc(sizeof(Node*));
    *highway = NULL;

    char command[COMMAND_LENGTH];

    // Read from stdin the command to execute
    while (scanf("%s", command) != EOF) {
        // Depending on the content of the string read the rest of the input line and call the adequate function
        if (strcmp(command, NEW_STATION) == 0) {
            int distance;
            short car_number;
            int cars_to_add[MAX_CARS];
            short number_of_cars = 0;
            char line_to_read[MAX_INPUT];
            int maxPower = 0;
            if (scanf("%d", &distance) == EOF) exit = 1;
            if (scanf("%hd", &car_number) == EOF) exit = 1;

            // Read the car list
            // Read the rest of the line from stdin
            if (fgets(line_to_read, MAX_INPUT, stdin) == NULL) exit = 1;
            // Read the first integer from the string read
            char *number = strtok(line_to_read, " ");
            for (int i = 0; i < car_number; i++) {
                // Parse the integer and put it in the array
                int newCar = atoi(number);
                cars_to_add[number_of_cars] = newCar;
                number_of_cars++;
                if(newCar > maxPower){
                    maxPower = newCar;
                }
                // Read the next integer from the line
                number = strtok(NULL, " ");
            }

            // Call the function to add a station
            addStation(distance, car_number, cars_to_add, maxPower);
        } else if (strcmp(command, REMOVE_STATION) == 0) {
            int distance;
            if (scanf("%d", &distance) == EOF) exit = 1;

            // Call the function to delete a station
            removeStation(distance);
        } else if (strcmp(command, NEW_CAR) == 0) {
            int station;
            int car_power;
            if (scanf("%d", &station) == EOF) exit = 1;
            if (scanf("%d", &car_power) == EOF) exit = 1;

            // Call the function to add a car to the data structure
            addCar(station, car_power);
        } else if (strcmp(command, DESTROY_CAR) == 0) {
            int station;
            int car;
            if (scanf("%d", &station) == EOF) exit = 1;
            if (scanf("%d", &car) == EOF) exit = 1;

            // Call the function to remove the car from the data structure
            removeCar(station, car);
        } else if (strcmp(command, FIND_ROUTE) == 0) {
            int source;
            int destination;
            if (scanf("%d", &source) == EOF) exit = 1;
            if (scanf("%d", &destination) == EOF) exit = 1;

            // Call the function that calculates the route
            findRoute(source, destination);
        } else {
            break;
        }

        // If arrived at the end of the file terminate the program
        if (exit == 1) {
            break;
        }
    }

    return 0;
}

/*
 * SECTION FOR COMMANDS FUNCTION IMPLEMENTATION
 */

void addStation(int station, short car_number, int *cars_to_add, int maxPower){
    Node* result = findNode(highway, station);
    if(result == NULL){
        RBInsert(highway, station, car_number, cars_to_add, maxPower);
        printf("aggiunta\n");
    } else {
        printf("non aggiunta\n");
    }
}

void removeStation(int station){
    Node* result = findNode(highway, station);
    if(result == NULL){
        printf("non demolita\n");
    } else {
        RBDelete(highway, result);
        printf("demolita\n");
    }
}

void addCar(int station, int car){
    Node* result = findNode(highway, station);
    if(result == NULL){
        printf("non aggiunta\n");
    } else {
        int carsPresent = result -> carNumber;
        if(carsPresent == 512){
            printf("non aggiunta\n");
            return;
        }
        result -> cars[carsPresent] = car;
        (result -> carNumber)++;
        if(car > result->maxPower){
            result->maxPower = car;
        }
        printf("aggiunta\n");
    }
}

void removeCar(int station, int car){
    Node* result = findNode(highway, station);
    if(result == NULL){
        printf("non rottamata\n");
        return;
    } else {
        int carsPresent = result -> carNumber;
        if(carsPresent == 0){
            printf("non rottamata\n");
            return;
        }
        // Find the car
        int position = -1;
        int* cars = result -> cars;
        for(int i = 0; i < carsPresent; i++){
            if(cars[i] == car){
                position = i;
                break;
            }
        }

        if(position == -1){
            // The car is not present
            printf("non rottamata\n");
            return;
        } else {
            // Remove the car
            result -> cars[position] = result -> cars[(result -> carNumber) - 1];
            (result -> carNumber)--;
            if(car == result->maxPower){
                // Find the new max
                cars = result -> cars;
                int newMax = 0;
                for(int i = 0; i < result -> carNumber; i++){
                    if(cars[i] > newMax){
                        newMax = cars[i];
                    }
                }

            }
            printf("rottamata\n");

        }
    }
}

void findRoute(int source, int destination){

    char result;

    // If source and destination are the same, print source and return
    if(source == destination){
        printf("%d\n", source);
        return;
    }

    // Look at the direction
    if(destination > source){   // Forward
        result = findForwardRoute(source, destination);
    } else {                    // Backwards
        result = findBackwardsRoute(source, destination);
    }

    if(result == NO){
        printf("nessun percorso\n");
    }
}

/*
 * SECTION FOR RB-TREE FUNCTIONS IMPLEMENTATION
 */
Node* findNode(RBTree T, int stationID){
        Node* x = (*T);
        while(x != NULL && x -> stationID != stationID){
            if(stationID < x -> stationID){
                x = x -> left;
            } else {
                x = x -> right;
            }
        }
        return x;
}

Node* newNode(int stationID, short carNumber, int cars[], int maxPower){

    Node* new = malloc(sizeof(Node));
    new -> color = RED;
    new -> parent = NULL;
    new -> right = NULL;
    new -> left = NULL;
    new -> stationID = stationID;
    new -> carNumber = carNumber;
    memcpy(new -> cars, cars, carNumber * sizeof(int));
    new -> maxPower = maxPower;

    return new;

}

Node* treeMinimum(Node* x){
    while(x -> left != NULL){
        x = x -> left;
    }
    return x;
}

void RBDelete(RBTree T, Node* z){

    assert(*T && z);
    Node *x = NULL;
    Node *y = z;
    char y_orig_c = y->color;
    if (z->left == NULL) {
        x = z->right;
        RBTransplant(T, z, z->right);
    } else if (z->right == NULL) {
        x = z->left;
        RBTransplant(T, z, z->left);
    } else {
        y = treeMinimum(z->right);
        y_orig_c = y->color;
        x = y->right;
        if (y->parent == z && x)
            x->parent = y;
        else {
            RBTransplant(T, y, y->right);
            y->right = z->right;
            if(y->right)
                y->right->parent = y;
        }
        RBTransplant(T, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    if (y_orig_c == BLACK)
        RBDeleteFixup(T, x);
    free(z);

}

void RBDeleteFixup(RBTree T, Node* x){

    if(*T == NULL || x == NULL)
        return;

    while(x != (*T) && x->color == BLACK){
        if(x==x->parent->left){
            Node* w = x->parent->right;

            if(w == NULL)
                break;

            if(w->color==RED){
                w->color = BLACK;
                x->parent->color = BLACK;
                RBLeftRotate(T,x->parent);
                w = x->parent->right;
            }

            if(w != NULL && (w -> left == NULL || w->left->color==BLACK) && (w->right == NULL || w->right->color == BLACK)){
                w->color = RED;
                x = x->parent;
            } else {
                if( w != NULL && (w->right == NULL || w->right->color == BLACK)){
                    w->left->color = BLACK;
                    w->color = RED;
                    RBRightRotate(T,w);
                    w = x->parent->right;
                }

                if(w!=NULL)
                    w->color = x->parent->color;
                x->parent->color = BLACK;
                if(w != NULL){
                w->right->color = BLACK;
                RBLeftRotate(T,x->parent);}
                x = (*T);
            }
        }  else {
            Node* w = x->parent->left;

            if(w == NULL)
                break;

            if(w->color==RED){
                w->color = BLACK;
                x->parent->color = BLACK;
                RBRightRotate(T,x->parent);
                w = x->parent->left;
            }

            if(w != NULL && (w->right == NULL || w->right->color==BLACK) && (w->left == NULL || w->left->color == BLACK)){
                w->color = RED;
                x = x->parent;
            } else {
                if(w != NULL && (w->left == NULL || w->left->color == BLACK)){
                    w->right->color = BLACK;
                    w->color = RED;
                    RBLeftRotate(T,w);
                    w = x->parent->left;
                }

                if(w != NULL)
                    w->color = x->parent->color;
                x->parent->color = BLACK;
                if(w != NULL){
                    w->left->color = BLACK;
                RBRightRotate(T,x->parent);}
                x = (*T);
            }
        }
    }
    x->color = BLACK;

}

void RBInsert(RBTree T, int stationID, short carNumber, int cars[], int maxPower){

    Node* z = newNode(stationID, carNumber, cars, maxPower);
    Node* y = NULL;
    Node* x = (*T);

    while(x != NULL){
        y = x;
        if(z -> stationID < x -> stationID){
            x = x -> left;
        } else {
            x = x -> right;
        }
    }

    z -> parent = y;

    if(y == NULL){
        assert((*T) == NULL);
        (*T) = z;
    } else if(z -> stationID < y -> stationID){
        y -> left = z;
    } else {
        y -> right = z;
    }

    z -> left = NULL;
    z -> right = NULL;
    z -> color = RED;

    RBInsertFixup(T, z);

}

void RBInsertFixup(RBTree T, Node* z){

    assert(T && z);
    while (z != (*T) && z -> parent -> color == RED) {
        assert(z->parent->parent);
        if (z->parent == z->parent->parent->left) {
            Node *y = z->parent->parent->right;
            if (y != NULL && y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    RBLeftRotate(T, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                RBRightRotate(T, z->parent->parent);
            }
        } else {
            assert(z->parent == z->parent->parent->right);
            Node *y = z->parent->parent->left;
            if (y != NULL && y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    RBRightRotate(T, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                RBLeftRotate(T, z->parent->parent);
            }
        }
    }
    (*T)->color = BLACK;
}

void RBLeftRotate(RBTree T, Node* x){
    Node *y = x->right;
    x->right = y->left;
    if (y->left != NULL)
        y->left->parent = x;

    y->parent = x->parent;
    if (x->parent == NULL) {
        (*T) = y;
    } else if (x == x->parent->left)
        x->parent->left = y;
    else {
        x->parent->right = y;
    }

    y->left = x;
    x->parent = y;

}

void RBPrint(Node* x){
    if(x != NULL){
        RBPrint(x -> left);
        printf("COLOR: %c, ID: %d\n", x -> color, x -> stationID);
        RBPrint(x -> right);
    }
}

void RBRightRotate(RBTree T, Node* x){
    Node *y = x->left;
    x->left = y->right;
    if (y->right != NULL)
        y->right->parent = x;

    y->parent = x->parent;
    if (x->parent == NULL) {
        (*T) = y;
    } else if (x == x->parent->left)
        x->parent->left = y;
    else {
        x->parent->right = y;
    }

    y->right = x;
    x->parent = y;
}

void RBTransplant(RBTree T, Node* u, Node* v){
    if(u -> parent == NULL){
        (*T) = v;
    } else if(u == u -> parent -> left){
        u -> parent -> left = v;
    } else {
        u -> parent -> right = v;
    }
    if(v != NULL){
        v -> parent = u -> parent;
    }
}

/*
 * SECTION FOR QUEUE FUNCTIONS IMPLEMENTATION
 */

char isEmpty(Queue queue) {
    if(queue->head == -1 && queue->tail == -1) {
        return OK;
    } else {
        return NO;
    }
}

char isFull(Queue queue) {
    if((queue->tail + 1) % MAX_QUEUE_SIZE == queue->head){
        return OK;
    } else {
        return NO;
    }
}

int dequeue(Queue queue) {
    if (isEmpty(queue) == OK) { //TODO: remove this
        printf("[DEBUG] Queue is empty. Cannot dequeue.\n");
        return -1;
    }

    int dequeuedValue = queue->data[queue->head];

    if (queue->head == queue->tail) {
        // Queue becomes empty after dequeue
        queue->head = -1;
        queue->tail = -1;
    } else {
        queue->head = (queue->head + 1) % MAX_QUEUE_SIZE;
    }

    return dequeuedValue;
}

void enqueue(Queue queue, int value) {
    if (isFull(queue) == OK) { //TODO: remove this
        printf("[DEBUG] Queue is full. Cannot enqueue %d.\n", value);
        return;
    }

    if (isEmpty(queue) == OK) {
        queue->head = 0;
        queue->tail = 0;
    } else {
        queue->tail = (queue->tail + 1) % MAX_QUEUE_SIZE;
    }

    queue->data[queue->tail] = value;
}

/*
 * SECTION FOR FIND ROUTE FUNCTIONS IMPLEMENTATION
 */
char findBackwardsRoute(int source, int destination){

    // Check if you can go directly from source to destination
    Node* sourceNode = findNode(highway, source);
    int maxDistance = source - (sourceNode -> maxPower);
    if(maxDistance <= destination){
        printf("%d %d\n", source, destination);
        return OK;
    }

    //TODO: FINISH DIS
    return NO;
}

char findForwardRoute(int source, int destination){

    // Check if you can go directly from source to destination
    Node* sourceNode = findNode(highway, source);
    int maxDistance = source + (sourceNode -> maxPower);
    if(maxDistance >= destination){
        printf("%d %d\n", source, destination);
        return OK;
    }

    // Create adjacency list
    RBTree adjacencyList = malloc(sizeof(Node*));
    *adjacencyList = NULL;
    initializeAdjacencyList(adjacencyList, source, destination, FORWARD);

    int steps = bfs(adjacencyList, source, destination);

    //TODO: FINISH DIS

    printf("Steps: %d\n", steps);

    //TODO: change dis to NO
    return OK;
}

int bfs(RBTree adjacencyList, int source, int destination){
    //Print the adjacency list
    RBPrint(*adjacencyList);

    // Create the queue
    Queue queue = (Queue) malloc(sizeof(Q));
    queue -> head = -1;
    queue -> tail = -1;

    // Color each node white and set their distance to infinite, then color the source node as gray and set its distance to 0
    // Note: the distance is saved in the attribute carNumber and the color is saved in the color attribute
    colorBFS(*adjacencyList, source);

    // Enqueue the source node
    enqueue(queue, source);

    while(isEmpty(queue) == NO){
        // Remove an element from the queue
        int u = dequeue(queue);
        Node* uNode = findNode(adjacencyList, u);

        bfsFor(uNode, uNode->maxPower, u, uNode -> carNumber, queue);

        uNode -> color = BLACK;
    }

    Node* destinationNode = findNode(adjacencyList, destination);

    if(destinationNode == NULL){
        return 0;
    }
    return destinationNode -> carNumber;
}

void ascendingAdd(Node* x, RBTree list, int source, int destination){
    if(x != NULL && ((x -> stationID) >= source)){
        ascendingAdd(x->left, list, source, destination);
        if(x->stationID == destination){
            return;
        }
        RBInsert(list, x->stationID, 0, NULL, ((x->stationID) + (x->maxPower)));
        ascendingAdd(x->right, list, source, destination);
    }
}

void bfsFor(Node* x, int maxDistanceReachable, int source, short distance, Queue queue){
    if(x != NULL && ((x -> stationID) >= source)){
        bfsFor(x->left, maxDistanceReachable, source, distance, queue);
        if(x->stationID > maxDistanceReachable){
            return;
        }
        if(x -> color == WHITE){
            x -> color = GRAY;
            x -> carNumber = distance + 1;
            enqueue(queue, x -> stationID);
        }
        bfsFor(x->right, maxDistanceReachable, source, distance, queue);
    }
}

void colorBFS(Node* x, int source){
    if(x != NULL){
        colorBFS(x->left, source);
        if(x->stationID != source){
            x -> color = WHITE;
            x -> carNumber = SHRT_MAX;
        } else {
            x -> color = GRAY;
        }
        colorBFS(x->right, source);
    }
}

void initializeAdjacencyList(RBTree adjacencyList, int source, int destination, char direction){
    if(direction == FORWARD){
        // Find source node
        Node* sourceNode = findNode(highway, source);
        ascendingAdd(sourceNode, adjacencyList, source, destination);
        RBInsert(adjacencyList, destination, 0, NULL, 0);
    }
    //TODO: FINISH DIS with BACKWARDS
}

