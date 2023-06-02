#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define COMMAND_LENGTH 18
#define MAX_CARS 512
#define MAX_INPUT 100000
#define NEW_STATION "aggiungi-stazione"
#define REMOVE_STATION "demolisci-stazione"
#define NEW_CAR "aggiungi-auto"
#define DESTROY_CAR "rottama-auto"
#define FIND_ROUTE "pianifica-percorso"

/*
 * SECTION FOR THE DECLARATION OF DATA STRUCTURES
 */
struct Node{
    int station_id;
    int* cars;
    int number_of_cars;
};

struct AdjacencyList{
    struct Node* root;
    int* edges;
    int number_of_edges;
};

struct Graph{
    struct AdjacencyList** adjacency_matrix;
    int number_of_nodes;
};

typedef struct Node* node;
typedef struct AdjacencyList* adjacency_list;
typedef struct Graph* graph;

/*
 * SECTION FOR COMMANDS FUNCTION DECLARATION
 */
void addStation(graph highway, int distance, int car_number, int* cars_to_add);
void removeStation(graph highway, int distance);
void addCar(graph highway, int station, int car_power);
void removeCar(graph highway, int station, int car);
void findRoute(int source, int destination);

/*
 * SECTION FOR MAX-HEAP GENERAL MANAGEMENT FUNCTION DECLARATION
 */
void maxHeapify(int* array, int size, int i);
int* buildMaxHeap(int* array, int size);


int main() {

    // Declaration and initialization of the graph
    graph highway = (struct Graph*) malloc(sizeof(struct Graph));
    highway -> number_of_nodes = 0;
    highway -> adjacency_matrix = NULL;

    char command[COMMAND_LENGTH];

    // Read from stdin the command to execute
    while(scanf("%s", command) != EOF){
        // Depending on the content of the string read the rest of the input line and call the adequate function
        if(strcmp(command, NEW_STATION) == 0){
            int distance;
            int car_number;
            int cars_to_add[MAX_CARS];
            int number_of_cars = 0;
            char line_to_read[MAX_INPUT];
            scanf("%d", &distance);
            scanf("%d", &car_number);

            // Read the car list
            // Read the rest of the line from stdin
            fgets(line_to_read, MAX_INPUT, stdin);
            // Read the first integer from the string read
            char* number = strtok(line_to_read, " ");
            for(int i = 0; i < car_number; i++){
                // Parse the integer and put it in the array
                cars_to_add[number_of_cars] = atoi(number);
                number_of_cars++;
                // Read the next integer from the line
                number = strtok(NULL, " ");
            }

            // Call the function to add a station
            addStation(highway, distance, car_number, cars_to_add);
        } else if (strcmp(command, REMOVE_STATION) == 0){
            int distance;
            scanf("%d", &distance);

            // Call the function to delete a station
            removeStation(highway, distance);
        } else if (strcmp(command, NEW_CAR) == 0){
            int station;
            int car_power;
            scanf("%d", &station);
            scanf("%d", &car_power);

            // Call the function to add a car to the data structure
            addCar(highway, station, car_power);
        } else if (strcmp(command, DESTROY_CAR) == 0){
            int station;
            int car;
            scanf("%d", &station);
            scanf("%d", &car);

            // Call the function to remove the car from the data structure
            removeCar(highway, station, car);
        } else if (strcmp(command, FIND_ROUTE) == 0){
            int source;
            int destination;
            scanf("%d", &source);
            scanf("%d", &destination);

            // Call the function that calculates the route
            findRoute(source, destination);
        }
    }

    return 0;
}

void addStation(graph highway, int distance, int car_number, int* cars_to_add){
    printf("Added station\n");
}

void removeStation(graph highway, int distance){
    printf("Removed station\n");
}

void addCar(graph highway, int station, int car_power){
    printf("Added car\n");
}

void removeCar(graph highway, int station, int car){
    printf("Removed car\n");
}

void findRoute(int source, int destination){
    printf("Found route\n");
}

void maxHeapify(int* array, int size, int i){
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    int max = i;

    if (l < size && array[l] > array[max]) {
        max = l;
    }

    if (r < size && array[r] > array[max]) {
        max = r;
    }

    if (max < size && array[max] > array[i]) {
        //TODO: [INFO] this condition in the if may be wrong
        // In case modify it to max != i
        int temp = array[i];
        array[i] = array[max];
        array[max] = temp;
        maxHeapify(array, size, max);
    }
}

int* buildMaxHeap(int* array, int size){

    for(int i = size/2 - 1; i >= 0; i--){
        maxHeapify(array, size, i);
    }

    return array;
}

