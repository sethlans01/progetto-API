#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

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
void RBFree(Node* x);
void RBInsert(RBTree T, int stationID, short carNumber, int cars[], int maxPower);
void RBInsertFixup(RBTree T, Node* z);
void RBLeftRotate(RBTree T, Node* x);
void RBPrint(Node* x);
void RBRightRotate(RBTree T, Node* x);
void RBTransplant(RBTree T, Node* u, Node* v);

/*
 * SECTION FOR FIND ROUTE FUNCTIONS DECLARATION
 */
char findBackwardsRoute(int source, int destination);
char findForwardRoute(int source, int destination);
void countStations(Node* x, int s, int d);
void countStationsBack(Node* x, int s, int d);
void findReachableStations(Node* x,int s, int d, int* reachable, int* distances);
void findReachableStationsBack(Node* x,int s, int d, int* reachable, int* distances);

// Global variables
RBTree highway;
int intermediateStations;
int ris;
int reach;

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

void RBFree(Node* x){

    if(x == NULL) {
        return;
    }
    RBFree(x -> left);
    RBFree(x -> right);
    free(x);

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

    // Count intermediate stations, i.e. all the stations that are between source and destination
    intermediateStations = 0;
    countStationsBack(*highway, source, destination);

    // Create two arrays: one to keep track of the intermediate stations that are reachable and one to keep track of
    // their biggest car
    int reachable[intermediateStations];
    int distances[intermediateStations];

    // Initialize the two arrays
    ris = 0;
    reach = maxDistance;
    findReachableStationsBack(*highway, source, destination, reachable, distances);

    // Check if the destination is in the array
    if(reachable[ris-1] != destination){
        return NO;
    } else {
        printf("Portal found\n");
        return OK;
    }

    //TODO: finishh dis

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

    // Count intermediate stations, i.e. all the stations that are between source and destination
    intermediateStations = 0;
    countStations(*highway, source, destination);

    // Create two arrays: one to keep track of the intermediate stations that are reachable and one to keep track of
    // their biggest car
    int reachable[intermediateStations];
    int distances[intermediateStations];

    // Initialize the two arrays
    ris = 0;
    reach = maxDistance;
    findReachableStations(*highway, source, destination, reachable, distances);

    // Check if the destination is in the array
    if(reachable[ris-1] != destination){
        return NO;
    }

    // Do the magic trick
    int solution[100];
    int pos = 0;
    char sourceUsed = NO;
    int newDest = destination;
    solution[pos] = newDest;
    pos++;

    while(sourceUsed == NO){
        // Find the smallest station to reach the destination
        for(int i = 0; i < intermediateStations; i++){
            if(distances[i] >= newDest){
                solution[pos] = reachable[i];
                newDest = reachable[i];
                if(reachable[i] == source){
                    sourceUsed = OK;
                }
                pos++;
                i = ris;
            }
        }
    }

    for(int i = pos-1; i > 0; i--){
        printf("%d ", solution[i]);
    }
    printf("%d\n", destination);

    return OK;
}

void countStations(Node* x, int s, int d){
    if(x != NULL){
        countStations(x->left, s, d);
        if(x->stationID >= s && x->stationID <= d){
            intermediateStations++;
        }
        countStations(x->right, s, d);
    }
}

void countStationsBack(Node* x, int s, int d){
    if(x != NULL){
        countStationsBack(x->left, s, d);
        if(x->stationID <= s && x->stationID >= d){
            intermediateStations++;
        }
        countStationsBack(x->right, s, d);
    }
}

void findReachableStations(Node* x,int s, int d, int* reachable, int* distances){
    if(x != NULL){
        findReachableStations(x->left, s, d, reachable, distances);
        int id = x->stationID;
        if(id >= s && id <= d && id <= reach){
            reachable[ris] = id;
            distances[ris] = x->maxPower + id;
            if(distances[ris] > reach){
                reach = distances[ris];
            }
            ris++;
        }
        findReachableStations(x->right, s, d, reachable, distances);
    }
}

void findReachableStationsBack(Node* x,int s, int d, int* reachable, int* distances){
    if(x != NULL){
        findReachableStationsBack(x->right, s, d, reachable, distances);
        int id = x->stationID;
        if(id <= s && id >= d && id >= reach){
            reachable[ris] = id;
            distances[ris] = id - x->maxPower;
            if(distances[ris] < reach){
                reach = distances[ris];
            }
            ris++;
        }
        findReachableStationsBack(x->left, s, d, reachable, distances);
    }
}
