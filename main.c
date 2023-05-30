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
    struct Node** adjacent_nodes;
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
void addCar(int station, int car_power);
void removeCar(int station, int car);
void findRoute(int source, int destination);

/*
 * SECTION FOR MAX-HEAP GENERAL MANAGEMENT FUNCTION DECLARATION
 */
int* buildMaxHeap(int* array, int size);

/*
 * SECTION FOR GRAPH GENERAL MANAGEMENT FUNCTION DECLARATION
 */

/*
 * SECTION FOR GRAPH SPECIFIC OPERATIONS
 */
int findStation(graph highway, int station);
struct Node** initializeAdjacencyList(graph highway, node new_node);

/*
 * SECTION FOR TODOS:
 * TODO: Write the data structure for the car list of each station
 * TODO: Write the data structure for the list of station in the highway
 * Implement the main loop of the app, i.e. the part that reads the file and calls the adequate functions [DONE]
 * TODO: Implement aggiungi-stazione: finish initializeAdjacencyList
 * Implement demolisci-stazione [DONE]
 * TODO: Implement aggiungi-auto
 * TODO: Implement rottama-auto
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
            addCar(station, car_power);
        } else if (strcmp(command, DESTROY_CAR) == 0){
            int station;
            int car;
            scanf("%d", &station);
            scanf("%d", &car);

            // Call the function to remove the car from the data structure
            removeCar(station, car);
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
        node new_node = (node) malloc(sizeof(node));
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
        adjacencyList node_adjacency_list = (adjacencyList) malloc(sizeof(adjacencyList));
        node_adjacency_list -> root = new_node;
        // Initialize the list
        node_adjacency_list -> adjacent_nodes = initializeAdjacencyList(highway, new_node);

        // Add the adjacency list to the graph
        if(highway -> number_of_nodes == 0){
            // If the graph has no elements add the list using malloc
            highway -> adjacencyLists = (struct AdjacencyList**) malloc(sizeof(adjacencyList));
            highway -> adjacencyLists[0] = node_adjacency_list;
        } else {
            // If the graph has already at least one element add the list using reallocate
            highway -> adjacencyLists = (struct AdjacencyList**) realloc(highway -> adjacencyLists,(highway->number_of_nodes + 1) * sizeof(struct AdjacencyList*));
            highway -> adjacencyLists[highway -> number_of_nodes] = node_adjacency_list;
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

        printf("demolita\n");
    }

}

//TODO: [IMPL] Implement this
void addCar(int station, int car_power){
    printf("Added car\n");
}

//TODO: [IMPL] Implement this
void removeCar(int station, int car){
    printf("Removed car\n");
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

//TODO: [IMPL] Implement this
struct Node** initializeAdjacencyList(graph highway, node new_node){
    return NULL;
}
