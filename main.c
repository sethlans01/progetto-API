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

/*
 * SECTION FOR NEW FUNCTIONS
 */
int findStation(graph highway, int station);
void addExitingEdges(graph highway, int list_position);
void addEnteringEdges(graph highway, int list_position);
void removeExitingEdges(graph highway, int nodeToRemoveFrom, int deleted_station);

void expandEdges(graph highway, int station_to_update, int added_car);

int main() {

    // Declaration and initialization of the graph
    graph highway = (struct Graph*) malloc(sizeof(struct Graph));
    highway -> number_of_nodes = 0;
    highway -> adjacency_matrix = (struct AdjacencyList**) malloc(0);

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

    if(findStation(highway, distance) == -1){
        // The node does not exist, so add it.
        // Allocate memory for a new node.
        node station = (node) malloc(sizeof(struct Node));

        // Initialize the new node.
        station -> station_id = distance;
        if(car_number == 0){
            station -> cars = NULL;
        } else {
            int* car_list = buildMaxHeap(cars_to_add, car_number);
            // Allocate memory for the list of cars and copy the data.
            station->cars = (int*)malloc(sizeof(int) * car_number);
            memcpy(station->cars, car_list, sizeof(int) * car_number);
        }
        station -> number_of_cars = car_number;

        // Allocate memory for a new adjacency list.
        adjacency_list list = (adjacency_list) malloc(sizeof(struct AdjacencyList));

        // Initialize the new adjacency list.
        list -> root = station;
        list -> edges = NULL;
        list -> number_of_edges = 0;

        // Add the list to the adjacency matrix.
        int old_number_of_nodes = highway -> number_of_nodes;
        // Expand the matrix.
        highway -> adjacency_matrix = (struct AdjacencyList**) realloc(highway -> adjacency_matrix, (old_number_of_nodes + 1) *
                                                                                                    sizeof(struct AdjacencyList*));

        // Add the list to the matrix.
        highway -> adjacency_matrix[old_number_of_nodes] = list;

        // Update the number of nodes.
        highway -> number_of_nodes++;

        // Calculate the edges that exit from the new node.
        addExitingEdges(highway, old_number_of_nodes);

        // Calculate the edges that enter the new node.
        addEnteringEdges(highway, old_number_of_nodes);

        printf("aggiunta\n");
    } else {
        // The node already exists, so don't add it.
        printf("non aggiunta\n");
    }

}

void addEnteringEdges(graph highway, int list_position) {

    if(highway -> number_of_nodes == 1){
        // Since there is only one station in the graph (the one we just added), there are not going to be any edges entering that node
        return;
    } else {
        // If there is more than one station in the graph, look if it's possible to create the edge
        for(int i = 0; i < highway -> number_of_nodes; i++){
            if(i != list_position){
                node current_node = highway -> adjacency_matrix[i] -> root;
                node inserted_node = highway -> adjacency_matrix[list_position] -> root;

                if(current_node -> number_of_cars != 0){
                    // Calculate the distance between the two stations
                    int distance = abs(current_node->station_id - inserted_node->station_id);
                    int max_car_power = current_node -> cars[0];
                    if(max_car_power >= distance){
                        adjacency_list current_node_adjacency_list = highway -> adjacency_matrix[i];
                        if(current_node_adjacency_list -> number_of_edges == 0){
                            // Add the station using malloc
                            current_node_adjacency_list -> edges = (int*) malloc(sizeof(int));
                            current_node_adjacency_list -> edges[0] = inserted_node -> station_id;
                        } else {
                            // Add the station using reallocation
                            current_node_adjacency_list -> edges = (int*) realloc(current_node_adjacency_list -> edges,
                                                                                   sizeof(int) * (current_node_adjacency_list -> number_of_edges + 1));
                            current_node_adjacency_list -> edges[current_node_adjacency_list -> number_of_edges] = inserted_node -> station_id;
                        }
                        // Update the number of edges in the list
                        current_node_adjacency_list -> number_of_edges++;
                    }
                }
            }
        }
    }
}

void addExitingEdges(graph highway, int list_position) {

    if(highway -> number_of_nodes == 1){
        // Since there is only one station in the graph (the one we just added), there are not going to be any edges exiting that node
        return;
    } else {
        // If there is more than one station in the graph, look if it's possible to create the edge
        for(int i = 0; i < highway -> number_of_nodes; i++){
            if(i != list_position){

                node current_node = highway -> adjacency_matrix[i] -> root;
                node inserted_node = highway -> adjacency_matrix[list_position] -> root;

                if(inserted_node -> number_of_cars != 0){
                    // Calculate the distance between the two stations
                    int distance = abs(current_node->station_id - inserted_node->station_id);
                    int max_car_power = inserted_node -> cars[0];
                    if(max_car_power >= distance){
                        adjacency_list inserted_node_adjacency_list = highway -> adjacency_matrix[list_position];
                        if(inserted_node_adjacency_list -> number_of_edges == 0){
                            // Add the station using malloc
                            inserted_node_adjacency_list -> edges = (int*) malloc(sizeof(int));
                            inserted_node_adjacency_list -> edges[0] = current_node -> station_id;
                        } else {
                            // Add the station using reallocation
                            inserted_node_adjacency_list -> edges = (int*) realloc(inserted_node_adjacency_list -> edges,
                                                                                   sizeof(int) * (inserted_node_adjacency_list -> number_of_edges + 1));
                            inserted_node_adjacency_list -> edges[inserted_node_adjacency_list -> number_of_edges] = current_node -> station_id;
                        }
                        // Update the number of edges in the list
                        inserted_node_adjacency_list -> number_of_edges++;
                    }
                }
            }
        }
    }

}

void removeStation(graph highway, int distance){

    // Check if the station exists
    int station_position = findStation(highway, distance);
    if(station_position == -1){
        // The station does not exist
        printf("non demolita\n");
        return;
    } else {
        // The station exists, so remove it
        // Deallocate the array of cars from the node
        free(highway -> adjacency_matrix[station_position] -> root -> cars);

        // Deallocate the node
        free(highway -> adjacency_matrix[station_position] -> root);

        // Deallocate the array of the edges
        free(highway -> adjacency_matrix[station_position] -> edges);

        // Deallocate the adjacency list
        free(highway -> adjacency_matrix[station_position]);

        // Shift the array of the adjacency matrix
        for(int i = station_position; i < highway -> number_of_nodes - 1; i++){
            highway -> adjacency_matrix[i] = highway -> adjacency_matrix[i + 1];
        }

        // Update the number of nodes in the graph
        highway -> number_of_nodes--;

        // Resize the adjacency matrix
        highway -> adjacency_matrix = (adjacency_list*) realloc(highway -> adjacency_matrix, sizeof(adjacency_list) * highway -> number_of_nodes);

        // Update the edges
        for(int i = 0; i < highway -> number_of_nodes; i++){
            removeExitingEdges(highway, i, distance);
        }

        printf("demolita\n");

    }

}

void removeExitingEdges(graph highway, int nodeToRemoveFrom, int deleted_station) {
    // Get the edges array of the node
    int* edges = highway -> adjacency_matrix[nodeToRemoveFrom] -> edges;

    // Find the position of the deleted station in the edges array
    int deleted_station_position = -1;
    for(int i = 0; i < highway -> adjacency_matrix[nodeToRemoveFrom] -> number_of_edges; i++){
        if(edges[i] == deleted_station){
            deleted_station_position = i;
            break;
        }
    }

    // If the deleted station is not in the edges array, return
    if(deleted_station_position == -1){
        return;
    }

    // Shift the array
    for(int i = deleted_station_position; i < highway -> adjacency_matrix[nodeToRemoveFrom] -> number_of_edges - 1; i++){
        edges[i] = edges[i + 1];
    }

    // Update the number of edges
    highway -> adjacency_matrix[nodeToRemoveFrom] -> number_of_edges--;

    // Resize the array
    highway -> adjacency_matrix[nodeToRemoveFrom] -> edges = (int*) realloc(highway -> adjacency_matrix[nodeToRemoveFrom] -> edges,
                                                                           sizeof(int) * highway -> adjacency_matrix[nodeToRemoveFrom] -> number_of_edges);
}

void addCar(graph highway, int station, int car_power){

    int hasToUpdateEdges = 0;
    int hasToDeleteEdgesList = 0;

    // Check if the station exists
    int station_position = findStation(highway, station);
    if(station_position == -1){
        // The station does not exist
        printf("non aggiunta\n");
        return;
    } else {
        // The station exists, so add the car
        node current_node = highway -> adjacency_matrix[station_position] -> root;
        if(current_node -> number_of_cars == 0){
            // The station is empty, so update the edges
            hasToUpdateEdges = 1;
            // Add the car using malloc
            current_node -> cars = (int*) malloc(sizeof(int));
            current_node -> cars[0] = car_power;
        } else {
            if(car_power > highway -> adjacency_matrix[station_position] -> root -> cars[0]){
                // The car is more powerful, so update the edges
                hasToDeleteEdgesList = 1;
                hasToUpdateEdges = 1;
            }
            // Add the car using reallocation
            current_node -> cars = (int*) realloc(current_node -> cars, sizeof(int) * (current_node -> number_of_cars + 1));
            current_node -> cars[current_node -> number_of_cars] = car_power;
        }
        // Update the number of cars in the node
        current_node -> number_of_cars++;

        // Build max heap
        int* new_cars = buildMaxHeap(current_node -> cars, current_node -> number_of_cars);
        current_node -> cars = new_cars;

        // Update the edges
        if(hasToUpdateEdges){
            if(hasToDeleteEdgesList){
                // Delete the edges list
                free(highway -> adjacency_matrix[station_position] -> edges);
                highway -> adjacency_matrix[station_position] -> edges = NULL;
                highway -> adjacency_matrix[station_position] -> number_of_edges = 0;
            }
            addExitingEdges(highway, station_position);
        }

        printf("aggiunta\n");
    }

}

void removeCar(graph highway, int station, int car){
    printf("Removed car\n");
}

void findRoute(int source, int destination){
    printf("Found route\n");
}

/*
 * @returns -1 if the station does not exist, otherwise returns the position in the adjacency_matrix in the graph
 */
int findStation(graph highway, int station) {
    // Get number of nodes of the graph
    int node_number = highway -> number_of_nodes;

    // If graph is empty return -1
    if(node_number == 0){
        return -1;
    }

    // Otherwise find the station
    for(int i = 0; i < node_number; i++){
        if(highway -> adjacency_matrix[i] -> root -> station_id == station){
            return i;
        }
    }

    // If code reaches this point the station does not exist
    return -1;
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

