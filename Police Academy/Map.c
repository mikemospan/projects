// Implementation of the Map ADT

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Map.h"

struct map {
    int **roads;            // Matrix of edges
    char **cityName;        // Array of city names
    int numCities;          // Number of vertices
    int numRoads;           // Number of edges
};

/**
 * Creates a new map with the given number of cities
 * Assumes that numCities is positive
 */
Map MapNew(int numCities) {
    // Malloc the map and check for errors
    Map m = malloc(sizeof(struct map));

    if (m == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    // Initialise all the struct's components and check for errors
    m->numCities = numCities;
    m->numRoads = 0;
    m->roads = malloc(numCities * sizeof(int *));
    m->cityName = calloc(m->numCities, sizeof(char *));

    if (m->cityName == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    if (m->roads == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }
    
    // Allocate memory for the adjacency matrix and intialise
    // all the cells to start at 0 (also checks for errors)
    for (int i = 0; i < numCities; i++) {
        m->roads[i] = calloc(numCities, sizeof(int));

        if (m->roads[i] == NULL) {
            fprintf(stderr, "error: out of memory\n");
            exit(EXIT_FAILURE);
        }
    }

    return m;
}

/**
 * Frees all memory associated with the given map
 */
void MapFree(Map m) {
    // Frees the components in the array of strings and matrix
    for (int i = 0; i < m->numCities; i++) {
        free(m->roads[i]);
        free(m->cityName[i]);
    }

    // Frees the array, matrix and map
    free(m->roads);
    free(m->cityName);
    free(m);
}

/**
 * Returns the number of cities on the given map
 */
int MapNumCities(Map m) {
    return m->numCities;
}

/**
 * Returns the number of roads on the given map
 */
int MapNumRoads(Map m) {
    return m->numRoads;
}

/**
 * Inserts a road between two cities with the given length
 * Does nothing if there is already a road between the two cities
 * Assumes that the cities are valid and are not the same
 * Assumes that the length of the road is positive
 */
void MapInsertRoad(Map m, int city1, int city2, int length) {
    // If the road doesn't exist, create it and increase numRoads
    if (m->roads[city1][city2] == 0) {
        m->roads[city1][city2] = length;
        m->roads[city2][city1] = length;
        m->numRoads++;
    }
}

/**
 * Sets the name of the given city
 */
void MapSetName(Map m, int city, char *name) {
    // Allocates enough memory for the string and checks for errors
    char *copy = malloc((strlen(name) + 1) * sizeof(char));

    if (copy == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }

    // Copies the string into the cityName array component
    m->cityName[city] = strcpy(copy, name);
}

/**
 * Returns the name of the given city
 */
char *MapGetName(Map m, int city) {
    return m->cityName[city];
}

/**
 * Checks if there is a road between the two given cities
 * Returns the length of the road if there is a road, and 0 otherwise
 */
int MapContainsRoad(Map m, int city1, int city2) {
    return m->roads[city1][city2];
}

/**
 * Returns the number of roads connected to the given city and stores
 * them in the given roads array. The `from` field should be equal to
 * `city` for all the roads in the array.
 * Assumes that the roads array is large enough to store all the roads
 */
int MapGetRoadsFrom(Map m, int city, Road roads[]) {
    // Initialises the number of roads in the city to 0
    int roadsInCity = 0;

    // For every city, check if there is a road and if there is
    // add that road to the roads array and increase the roads counter
    for (int i = 0; i < m->numCities; i++) {
        if (m->roads[city][i] != 0) {
            roads[roadsInCity].from = city;
            roads[roadsInCity].to = i;
            roads[roadsInCity].length = m->roads[city][i];
            roadsInCity++;
        }
    }
    
    return roadsInCity;
}

/**
 * Displays the map
 * !!! DO NOT EDIT THIS FUNCTION !!!
 * This function will work once the other functions are working
 */
void MapShow(Map m) {
    printf("Number of cities: %d\n", MapNumCities(m));
    printf("Number of roads: %d\n", MapNumRoads(m));
    
    Road *roads = malloc(MapNumRoads(m) * sizeof(Road));
    if (roads == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);    
    }
    
    for (int i = 0; i < MapNumCities(m); i++) {
        printf("[%d] %s has roads to:", i, MapGetName(m, i));
        int numRoads = MapGetRoadsFrom(m, i, roads);
        for (int j = 0; j < numRoads; j++) {
            if (j > 0) {
                printf(",");
            }
            printf(" [%d] %s (%d)", roads[j].to, MapGetName(m, roads[j].to),
                   roads[j].length);
        }
        printf("\n");
    }
    
    free(roads);
}

