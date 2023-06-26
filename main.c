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
 * SECTION FOR COMMANDS FUNCTION DECLARATION
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
    printf("aggiungi auto\n");
}

void removeCar(int station, int car){
    printf("rottama auto\n");
}

void findRoute(int source, int destination){
    printf("pianifica percorso\n");
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
