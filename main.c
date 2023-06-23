#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

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

// Red-Black Tree management functions
Node* newRBNode(int stationID, short carNumber, const int *cars, int maxPower);
Node* RBMinimum(Node* x);
Node* RBSuccessor(Node* x);
Node* locateNode(Node** T, int stationID);
char isPresent(Node** T, int stationID);
void rotateLeft(Node** T, Node* x);
void rotateRight(Node** T, Node* y);
void RBInsertFixup(Node** T, Node* z);
void RBInsert(Node** T, int stationID, short carNumber, const int *cars, int maxPower);
void RBDeleteFixup(Node** T, Node* x);
void RBDelete(Node** T, Node* z);

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
        }

        // If arrived at the end of the file terminate the program
        if (exit == 1) {
            break;
        }
    }

    printf("FINE\n");

    return 0;
}

void addStation(int station, short car_number, int *cars_to_add, int maxPower){
    char result;
    result = isPresent(highway, station);
    if(result == '0'){ // Station is not present, so add it
        RBInsert(highway, station, car_number, cars_to_add, maxPower);
        printf("aggiunta\n");
    } else {
        // Station is present, so don't add it
        printf("non aggiunta\n");
    }
}

void removeStation(int station){
    Node* result;
    result = locateNode(highway, station);
    if(result == NULL){
        // The station is not present, so don't remove it
        printf("non demolita\n");
    } else {
        // The station is present, so remove it
        RBDelete(highway, result);
        printf("demolita\n");
    }
}

void addCar(int station, int car){
    Node* result;
    result = locateNode(highway, station);
    if(result == NULL){
        // The station is not present, so don't add the car
        printf("non aggiunta\n");
    } else {
        // The station is present, so try to add it
        if(result->carNumber == MAX_CARS){
            // The car parking lot is full, so don't add the new car
            printf("non aggiunta\n");
        } else {
            result->cars[result->carNumber] = car;
            (result->carNumber)++;
            if(car > result->maxPower){
                result->maxPower = car;
            }
            printf("aggiunta\n");
        }
    }
}

void removeCar(int station, int car){
    Node* result;
    result = locateNode(highway, station);
    if(result == NULL){
        // The station is not present, so don't remove the car
        printf("non rottamata\n");
    } else {
        // The station is present, so try to remove it
        short position = -1;
        short maxCars = result->carNumber;
        int *carArray = result -> cars;
        for(short i = 0; i < maxCars; i++){
            if(car == carArray[i]){
                position = i;
                break;
            }
        }
        if(position == -1){
            // The car is not present in the station, so don't remove it
            printf("non rottamata\n");
        } else {
            // Copy the last car in the position of the car that needs to be removed
            result->cars[position] = result->cars[maxCars-1];
            (result->carNumber)--;
            if(car == result->maxPower){
                // Find the new max power car
                int newMax = 0;
                maxCars = result->carNumber;
                carArray = result -> cars;
                for(short i = 0; i < maxCars; i++){
                    if(carArray[i] > newMax){
                        newMax = carArray[i];
                    }
                }
                result->maxPower = newMax;
            }
            printf("rottamata\n");
        }
    }
}

void findRoute(int source, int destination){

}

// Red-Black Tree management functions
Node* newRBNode(int stationID, short carNumber, const int *cars, int maxPower){

    Node *temp = (Node*) malloc(sizeof(Node));
    temp->color     = RED;
    temp->left      = NULL;
    temp->right     = NULL;
    temp->parent    = NULL;
    temp->stationID = stationID;
    temp->carNumber = carNumber;
    temp->maxPower  = maxPower;
    for(int i = 0; i < carNumber; i++){
        temp->cars[i] = cars[i];
    }

    return temp;
}

Node* RBMinimum(Node* x){
    while(x->left != NULL){
        x = x->left;
    }
    return x;
}

Node* RBSuccessor(Node* x){
    Node* y;
    if(x->right != NULL){
        return RBMinimum(x->right);
    }
    y = x->parent;
    while(y != NULL && x == y->right){
        x = y;
        y = y->parent;
    }
    return y;
}

Node* locateNode(Node** T, int stationID){
    if((*T) == NULL || (*T)->stationID == stationID){
        return (*T);
    }
    if(stationID > (*T) -> stationID){
        return locateNode(&((*T)->right), stationID);
    } else {
        return locateNode(&((*T)->left), stationID);
    }
}

char isPresent(Node** T, int stationID){
    if((*T) == NULL){
        return '0';
    }
    if((*T)->stationID == stationID){
        return '1';
    }
    if(stationID > (*T) -> stationID){
        return isPresent(&((*T)->right), stationID);
    } else {
        return isPresent(&((*T)->left), stationID);
    }
}

void rotateLeft(Node** T, Node* x){
    Node *y  = x->right;
    x->right = y->left;
    if (y->left != NULL)
        y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == NULL)
        *T = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left   = x;
    x->parent = y;
}

void rotateRight(Node** T, Node* y){
    Node *x  = y->left;
    y->left  = x->right;
    if (x->right != NULL)
        x->right->parent = y;
    x->parent = y->parent;
    if (y->parent == NULL)
        *T = x;
    else if (y == y->parent->right)
        y->parent->right = x;
    else
        y->parent->left  = x;
    x->right  = y;
    y->parent = x;
}

void RBInsertFixup(Node** T, Node* z){
    if(z == *T){
        (*T)->color = BLACK;
    } else {
        Node* x;
        x = z->parent;
        if(x->color == RED){
            if(x == x->parent->left){
                Node* y;
                y = x->parent->right;
                if(y->color == RED){
                    x->color = BLACK;
                    y->color = BLACK;
                    x->parent->color = RED;
                    RBInsertFixup(T, x->parent);
                } else {
                    if(z == x->right){
                        z = x;
                        rotateLeft(T, z);
                        x = z -> parent;
                    }
                    x->color = BLACK;
                    x->parent->color = RED;
                    rotateRight(T, x->parent);
                }
            } else {
                Node* y;
                y = x->parent->left;
                if(y->color == RED){
                    x->color = BLACK;
                    y->color = BLACK;
                    x->parent->color = RED;
                    RBInsertFixup(T, x->parent);
                } else {
                    if(z == x->left){
                        z = x;
                        rotateRight(T, z);
                        x = z -> parent;
                    }
                    x->color = BLACK;
                    x->parent->color = RED;
                    rotateLeft(T, x->parent);
                }
            }
        }
    }
}

void RBInsert(Node** T, int stationID, short carNumber, const int *cars, int maxPower){

    Node* z = newRBNode(stationID, carNumber, cars, maxPower);
    Node* y =  NULL;
    Node* x = *T;

    // Find where to Insert new node Z into the binary search tree
    while (x != NULL) {
        y = x;
        if (z->stationID < x->stationID)
            x = x->left;
        else
            x = x->right;
    }

    z->parent = y;
    if (y == NULL)
        *T = z;
    else if (z->stationID < y->stationID)
        y->left  = z;
    else
        y->right = z;

    // Init z as a red leaf
    z->left  = NULL;
    z->right = NULL;
    z->color = RED;

    // Ensure the Red-Black property is maintained
    RBInsertFixup(T, z);
}

void RBDeleteFixup(Node** T, Node* x){
    if(x->color == RED || x->parent == NULL){
        x->color = BLACK;
    } else if(x == x->parent->left){
        Node* w;
        w = x->parent->right;
        if(w->color == RED){
            w->color = BLACK;
            x->parent->color = RED;
            rotateLeft(T, x->parent);
            w = x->parent->right;
        }
        if(w->left->color == BLACK && w->right->color == BLACK){
            w -> color = RED;
            RBDeleteFixup(T, x->parent);
        } else {
            if(w->right->color == BLACK){
                w->left->color = BLACK;
                w -> color = RED;
                rotateRight(T, w);
                w = x->parent->right;
            }
            w->color = x->parent->color;
            x->parent->color = BLACK;
            w->right->color = BLACK;
            rotateLeft(T, x->parent);
        }
    } else {
        Node* w;
        w = x->parent->left;
        if(w->color == RED){
            w->color = BLACK;
            x->parent->color = RED;
            rotateRight(T, x->parent);
            w = x->parent->left;
        }
        if(w->right->color == BLACK && w->left->color == BLACK){
            w -> color = RED;
            RBDeleteFixup(T, x->parent);
        } else {
            if(w->left->color == BLACK){
                w->right->color = BLACK;
                w -> color = RED;
                rotateLeft(T, w);
                w = x->parent->left;
            }
            w->color = x->parent->color;
            x->parent->color = BLACK;
            w->left->color = BLACK;
            rotateRight(T, x->parent);
        }
    }
}

void RBDelete(Node** T, Node* z){
    Node *x, *y;
    if(z->left == NULL || z->right == NULL){
        y = z;
    } else {
        y = RBSuccessor(z);
    }
    if(y->left != NULL){
        x = y->left;
    } else {
        x = y->right;
    }
    x->parent = y->parent;
    if(y->parent == NULL){
        (*T) = x;
    } else if(y == y->parent->left){
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }
    if(y != z){
        z -> stationID = y -> stationID;
        z -> carNumber = y -> carNumber;
        memcpy(z->cars, y->cars, z->carNumber * sizeof(int ));
    }
    if(y->color == BLACK){
        RBDeleteFixup(T, x);
    }
    free(y);
}
