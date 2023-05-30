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

void addStation(int distance, int car_number, int* cars_to_add);
void removeStation(int distance);
void addCar(int station, int car_power);
void removeCar(int station, int car);
void findRoute(int source, int destination);

/*
 * SECTION FOR TODOS:
 * TODO: Write the data structure for the car list of each station
 * TODO: Write the data structure for the list of station in the highway
 * Implement the main loop of the app, i.e. the part that reads the file and calls the adequate functions [DONE]
 * TODO: Implement aggiungi-stazione
 * TODO: Implement demolisci-stazione
 * TODO: Implement aggiungi-auto
 * TODO: Implement rottama-auto
 * TODO: Implement pianifica-percorso
 *
 */

int main() {
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
            addStation(distance, car_number, cars_to_add);
        } else if (strcmp(command, REMOVE_STATION) == 0){
            int distance;
            scanf("%d", &distance);

            // Call the function to delete a station
            removeStation(distance);
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

//TODO: [IMPL] Implement this
void addStation(int distance, int car_number, int* cars_to_add){
    printf("Added station\n");
}

//TODO: [IMPL] Implement this
void removeStation(int distance){
    printf("Removed station\n");
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
