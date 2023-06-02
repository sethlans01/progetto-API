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
    int car_number;
    int* cars;
};

struct AdjacencyList{
    struct Node* root;
    int* adjacent_nodes;
};

struct Graph{
    int number_of_nodes;
    struct AdjacencyList** adjacencyLists;
};

typedef struct Node* node;
typedef struct AdjacencyList* adjacencyList;
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

/*
 * SECTION FOR GRAPH GENERAL MANAGEMENT FUNCTION DECLARATION
 */

/*
 * SECTION FOR GRAPH SPECIFIC OPERATIONS
 */
int findStation(graph highway, int station);
int findCar(node station, int car);
int* initializeAdjacencyList(graph highway, node new_node);
graph recalculateEdges(graph highway);
graph removeStationFromAdjacencyLists(graph highway, int station);

/*
 * SECTION FOR TODOS:
 * TODO: Write the data structure for the car list of each station
 * TODO: Write the data structure for the list of station in the highway
 * Implement the main loop of the app, i.e. the part that reads the file and calls the adequate functions [DONE]
 * TODO: Implement aggiungi-stazione: finish initializeAdjacencyList
 * Implement demolisci-stazione [DONE]
 * Implement aggiungi-auto [DONE]
 * Implement rottama-auto [DONE]
 * TODO: Implement pianifica-percorso
 *
 */

int main() {

    // Declaration and initialization of the graph
    graph highway = (struct Graph*) malloc(sizeof(struct Graph));
    highway -> number_of_nodes = 0;
    highway -> adjacencyLists = NULL;

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

    // Check if the station is present
    if(findStation(highway, distance) == -1){
        //If the station is not present add the station

        // Create the new node of the graph
        node new_node = (node) malloc(sizeof(struct Node));
        new_node->cars = (int*) malloc(car_number * sizeof(int));

        new_node -> station_id = distance;
        new_node -> car_number = car_number;
        if(car_number == 0){
            // If the player wants to build a station with 0 cars inside make the list null
            new_node -> cars = NULL;
        } else {
            // Otherwise build a max heap from the array
            int* cars = buildMaxHeap(cars_to_add, car_number);
            new_node -> cars = cars;
        }

        // Create a new adjacency list for this node
        adjacencyList node_adjacency_list = (adjacencyList) malloc(sizeof(struct AdjacencyList));
        node_adjacency_list -> root = new_node;

        // Add the adjacency list to the graph
        if(highway -> number_of_nodes == 0){
            // Initialize the list
            node_adjacency_list -> adjacent_nodes = NULL;
            // If the graph has no elements add the list using malloc
            highway -> adjacencyLists = (struct AdjacencyList**) malloc(sizeof(adjacencyList));
            highway -> adjacencyLists[0] = node_adjacency_list;
        } else {
            // Initialize the list: create the edges that are exiting the new node, but only do it if this station has cars
            if(car_number != 0){
                node_adjacency_list -> adjacent_nodes = initializeAdjacencyList(highway, new_node);
            }
            // If the graph has already at least one element add the list using reallocate
            highway -> adjacencyLists = (struct AdjacencyList**) realloc(highway -> adjacencyLists,(highway->number_of_nodes + 1) * sizeof(struct AdjacencyList*));
            highway -> adjacencyLists[highway -> number_of_nodes] = node_adjacency_list;
            // Modify the adjacency lists of the other nodes of the graph: create the edges that are entering the new node
            highway = recalculateEdges(highway);
        }
        highway -> number_of_nodes++;

        printf("aggiunta\n");
    } else {
        // If the station is already present don't add it and return
        printf("non aggiunta\n");
    }

}

void removeStation(graph highway, int distance){

    // Check if the station is present
    int position = findStation(highway, distance);
    if(position == -1){
        // If the station is not present print the message and return
        printf("non demolita\n");
    } else {
        // If the station is present, start deleting the things from the structure
        // Free the adjacency list of the station
        free(highway -> adjacencyLists[position] -> adjacent_nodes);
        free(highway -> adjacencyLists[position]);

        // Shift the remaining elements in the adjacencyLists array to fill the gap left by the removed adjacency list
        for (int i = position; i < highway -> number_of_nodes - 1; i++) {
            highway -> adjacencyLists[i] = highway -> adjacencyLists[i + 1];
        }

        // Update the number_of_nodes field of the Graph structure
        highway -> number_of_nodes--;

        // Resize the adjacencyLists array to reflect the updated number_of_nodes
        highway -> adjacencyLists = (struct AdjacencyList**)realloc(highway -> adjacencyLists, highway -> number_of_nodes * sizeof(struct AdjacencyList*));

        // Remove the station from the adjacency lists of the other stations
        highway = removeStationFromAdjacencyLists(highway, distance);

        printf("demolita\n");
    }

}

void addCar(graph highway, int station, int car_power){

    // Check if the station exists
    int position = findStation(highway, station);
    if(position == -1){
        // The station does not exist, so don't add the car
        printf("non aggiunta\n");
    } else {
        // The station exists, so check if the car exists in that station

        // Find the adjacency list where the root is the station
        int list_position = 0;
        for(int i = 0; i < highway -> number_of_nodes; i++){
            if(highway -> adjacencyLists[i] -> root -> station_id == station){
                list_position = i;
                break;
            }
        }

        int car_position = findCar(highway -> adjacencyLists[list_position] -> root, car_power);
        if(car_position == -1){
            // The car is not present in the list, so add it to the list
            int* cars = highway -> adjacencyLists[list_position] -> root -> cars;
            cars = (int*) realloc(cars, (highway -> adjacencyLists[list_position] -> root -> car_number + 1) * sizeof(int));
            cars[highway -> adjacencyLists[list_position] -> root -> car_number] = car_power;
            highway->adjacencyLists[list_position]->root->cars = cars;

            // Update the car number in the station
            highway -> adjacencyLists[list_position] -> root -> car_number++;

            //Modify graph's edges and change the edges exiting from the modified node
            highway -> adjacencyLists[list_position] -> adjacent_nodes = initializeAdjacencyList(highway, highway -> adjacencyLists[list_position] -> root);

            printf("aggiunta\n");
        } else {
            // The car is already present so don't add it and return
            printf("non aggiunta\n");
        }
    }

}

void removeCar(graph highway, int station, int car){

    // Check that the station exists
    if(findStation(highway, station) == -1){
        printf("non rottamata\n");
    } else {
        // Check if the car exists in the station
        // Find the adjacency list where the root is the station
        int list_position = 0;
        for(int i = 0; i < highway -> number_of_nodes; i++){
            if(highway -> adjacencyLists[i] -> root -> station_id == station){
                list_position = i;
                break;
            }
        }

        int car_position = findCar(highway -> adjacencyLists[list_position] -> root, car);
        if(car_position == -1){
            printf("non rottamata\n");
        } else {

            // Remove the car from the array of cars
            // Shift the remaining elements in the cars array to fill the gap left by the removed car
            int* cars = highway -> adjacencyLists[list_position] -> root -> cars;
            for(int i = car_position; i < highway -> adjacencyLists[list_position] -> root -> car_number - 1; i++){
                cars[i] = cars[i + 1];
            }
            // Update the car number in the station
            highway -> adjacencyLists[list_position] -> root -> car_number--;
            // Resize the cars array to reflect the updated car_number
            cars = (int*) malloc(highway -> adjacencyLists[list_position] -> root -> car_number * sizeof(int));
            memccpy(cars, highway -> adjacencyLists[list_position] -> root -> cars, highway -> adjacencyLists[list_position] -> root -> car_number * sizeof(int), sizeof(int));
            highway -> adjacencyLists[list_position] -> root -> cars = cars;
            // Modify graph's edges and change the edges exiting from the modified node
            highway -> adjacencyLists[list_position] -> adjacent_nodes = initializeAdjacencyList(highway, highway -> adjacencyLists[list_position] -> root);

            printf("rottamata\n");

        }
    }

}

//TODO: [IMPL] Implement this
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
        //TODO: [INFO] this for may be wrong, in case look at the pseudo-code to fix it (page 48)
        maxHeapify(array, size, i);
    }

    return array;
}

/*
 * @returns -1 if the station is not present in the graph, otherwise returns the position of the station in the struct
 */
int findStation(graph highway, int station){

    // If the graph is empty return false
    if(highway -> number_of_nodes == 0){
        return -1;
    }

    // Otherwise look for the station
    for(int i = 0; i < highway -> number_of_nodes; i++){
        adjacencyList currentList = highway -> adjacencyLists[i];
        if(currentList -> root -> station_id == station){
            return i;
        }
    }

    // If the program arrives here it means that the station does not exist
    return -1;
}

/*
 * @returns -1 if the car is not present in the station, otherwise returns the position of the car in the struct
 */
int findCar(node station, int car){
    // If there are no cars in the station return false
    if(station -> car_number == 0){
        return -1;
    }

    // Otherwise try to find the car
    for(int i = 0; i < station -> car_number; i++){
        if(station -> cars[i] == car){
            return i;
        }
    }

    // If the code arrives here there it means that the car has not been found in the station
    return -1;
}

//TODO: [IMPL] Implement this
int* initializeAdjacencyList(graph highway, node new_node){
    return NULL;
}

//TODO: [IMPL] Implement this
graph recalculateEdges(graph highway){
    return highway;
}

//TODO: [IMPL] Implement this
graph removeStationFromAdjacencyLists(graph highway, int station){
    return highway;
}

