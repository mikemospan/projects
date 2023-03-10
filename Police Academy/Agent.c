// This program was made by Michael Mospan (z5419614) on 5/08/2022
// This file is an implementation of the Agent ADT

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>         // Included library for INT_MAX  

#include "Agent.h"
#include "Map.h"
#include "Stack.h"          // Included the created Stack ADT

#define UNDEFINED -1        // Undefined for NULL or unknown values

// This struct stores information about an individual agent and can be
// used to store information that the agent needs to remember.
struct agent {
    char *name;
    int startLocation;
    int location;
    int maxStamina;         // Max stamina
    int stamina;            // Current stamina
    int strategy;
    Map map;
    
    int thiefLocation;      // Stores the thief's given location
    int *clvVisited;        // Array of visited cities for CLV strategy
    bool *dfsVisited;       // Array of visited cities for DFS strategy
    StackNode stack;        // A pointer to the head of the stack
};

// Function Prototypes
static Move chooseRandomMove(Agent agent, Map m);
static Move chooseCLVMove(Agent agent, Map m);
static Move chooseCLV(Agent agent, Road legalRoads[], int numLegalRoads);
static Move chooseDFSMove(Agent agent, Map m);
static Move chooseDFS(Agent agent, Road roads[], int numRoads, int length);
static Move chooseLTP(Map m, Agent agent);
static void checkIfReset(Map m, Agent agent, int cityID);
static void resetDFS(Map m, Agent agent, int cityID);
static void dijkstra(Map m, int source, int currStamina, int maxStamina,
                     int *pred, int *staminaCost);
static void visitCity(int i, int source, int nextCity, int stamina[],
                      Road roads[], int staminaCost[], int numTurns[],
                      int pred[], double cost[], int maxStamina);
static int filterRoads(Agent agent, Road roads[], int numRoads,
                       Road legalRoads[]);

/**
 * Creates a new agent
 */
Agent AgentNew(int start, int stamina, int strategy, Map m, char *name) {
    if (start >= MapNumCities(m)) {
        fprintf(stderr, "error: starting city (%d) is invalid\n", start);
        exit(EXIT_FAILURE);
    }
    
    Agent agent = malloc(sizeof(struct agent));
    if (agent == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }
    
    agent->startLocation = start;
    agent->location = start;
    agent->maxStamina = stamina;
    agent->stamina = stamina;
    agent->strategy = strategy;
    agent->map = m;
    agent->name = strdup(name);

    // Starts the thief's location as undefined (unknown)
    agent->thiefLocation = UNDEFINED;
    
    // Allocates and initialises memory for the CLV strategy visited array
    agent->clvVisited = calloc(MapNumCities(m), sizeof(int));
    agent->clvVisited[agent->startLocation] = 1;
    
    // Allocates and initialises memory for the DFS strategy visited array
    agent->dfsVisited = calloc(MapNumCities(m), sizeof(bool));
    agent->dfsVisited[agent->startLocation] = true;
    agent->stack = NULL;
    
    return agent;
}

/**
 * Frees all memory associated with the agent
 * NOTE: You should not free the map because the map is owned by the
 *       main program, and the main program will free it
 */
void AgentFree(Agent agent) {
    free(agent->clvVisited);
    free(agent->dfsVisited);
    free(agent->name);
    StackFree(agent->stack);
    free(agent);
}

////////////////////////////////////////////////////////////////////////
// Making moves

/**
 * Calculates the agent's next move
 */
Move AgentGetNextMove(Agent agent, Map m) {
    switch (agent->strategy) {
        case STATIONARY:             return (Move){agent->location, 0};
        case RANDOM:                 return chooseRandomMove(agent, m);
        case CHEAPEST_LEAST_VISITED: return chooseCLVMove(agent, m);
        case DFS:                    return chooseDFSMove(agent, m);
        default:
            printf("error: strategy not implemented yet\n");
            exit(EXIT_FAILURE);
    }
}

static Move chooseRandomMove(Agent agent, Map m) {
    Move move;
    Road *roads = malloc(MapNumCities(m) * sizeof(Road));
    Road *legalRoads = malloc(MapNumCities(m) * sizeof(Road));

    // Get all roads to adjacent cities
    int numRoads = MapGetRoadsFrom(m, agent->location, roads);
    // Filter out roads that the agent does not have enough stamina for
    int numLegalRoads = filterRoads(agent, roads, numRoads, legalRoads);

    // If the thief's location is known and there is a legal road
    if (agent->thiefLocation != UNDEFINED && numLegalRoads > 0) {
        move = chooseLTP(m, agent);
    }
    else if (numLegalRoads > 0) {
        // Sort the roads using insertion sort
        for (int i = 1; i < numLegalRoads; i++) {
            Road r = legalRoads[i];
            int j = i;
            while (j > 0 && r.to < legalRoads[j - 1].to) {
                legalRoads[j] = legalRoads[j - 1];
                j--;
            }
            legalRoads[j] = r;
        }
        
        // nextMove is randomly chosen from the legal roads
        int cityID = rand() % numLegalRoads;
        move = (Move){legalRoads[cityID].to, legalRoads[cityID].length};
    } else {
        // The agent must stay in the same location
        move = (Move){agent->location, 0};
    }
    
    free(legalRoads);
    free(roads);
    return move;
}

static Move chooseCLVMove(Agent agent, Map m) {
    // Create the move, roads and legal roads arrays
    Move move;
    Road *roads = malloc(MapNumCities(m) * sizeof(Road));
    Road *legalRoads = malloc(MapNumCities(m) * sizeof(Road));
    
    // Get all roads to adjacent cities
    int numRoads = MapGetRoadsFrom(m, agent->location, roads);
    // Filter out roads that the agent does not have enough stamina for
    int numLegalRoads = filterRoads(agent, roads, numRoads, legalRoads);

    if (agent->thiefLocation != UNDEFINED && numLegalRoads > 0) {
        // Update move using chooseLTP if the thief's location is known
        move = chooseLTP(m, agent);
    }
    else if (numLegalRoads > 0) {
        // Update move using chooseCLV if the thief's location is unknown
        move = chooseCLV(agent, legalRoads, numLegalRoads);
    }
    else {
        // Don't move
        move = (Move){agent->location, 0};
    }
    
    // Increment the CLV visited array
    agent->clvVisited[move.to]++;

    // Free all arrays and return the move
    free(legalRoads);
    free(roads);
    return move;
}

// Function for Cheap Least Visited Strategy
static Move chooseCLV(Agent agent, Road legalRoads[], int numLegalRoads) {
    // Initialise the checked stats for the first legal city
    int numVisited = agent->clvVisited[legalRoads[0].to];
    int staminaCost = legalRoads[0].length;
    int cityID = 0;
    Move move;

    // For every next legal road
    for (int i = 1; i < numLegalRoads; i++) {
        // If the city has been visited less update the stats
        if (numVisited > agent->clvVisited[legalRoads[i].to]) {
            numVisited = agent->clvVisited[legalRoads[i].to];
            staminaCost = legalRoads[i].length;
            cityID = i;
        }
        // If the city has been visited the same amount,
        // check which city has the highest stamina cost
        else if (numVisited == agent->clvVisited[legalRoads[i].to]) {
            if (staminaCost > legalRoads[i].length) {
                staminaCost = legalRoads[i].length;
                cityID = i;
            }
        }
    }

    // Move to the next location
    move = (Move){legalRoads[cityID].to, legalRoads[cityID].length};
    return move;
}

static Move chooseDFSMove(Agent agent, Map m) {
    // Create the move, roads and legal roads arrays
    Move move;
    Road *roads = malloc(MapNumCities(m) * sizeof(Road));
    int numRoads = MapGetRoadsFrom(m, agent->location, roads);
    int length = 0;
    
    if (agent->thiefLocation != UNDEFINED) {
        // Update the move using and chooseLTP and reset the
        // DFS counters if the thief's location is known
        move = chooseLTP(m, agent);
        resetDFS(m, agent, move.to);
    }
    else {
        // Update move using chooseDFS if the thief's location is unknown
        move = chooseDFS(agent, roads, numRoads, length);
    }

    // Check if we need to reset the DFS path
    checkIfReset(m, agent, move.to);

    // Free all the adjacent road and returns the move
    free(roads);
    return move;
}

// Function for Depth First Search Strategy
static Move chooseDFS(Agent agent, Road roads[], int numRoads, int length) {
    Move move;
    bool cityIDFound = false;
    int cityID;

    // Search through the adjacent roads for the first unvisited city
    for (int i = 0; i < numRoads; i++) {
        if (!agent->dfsVisited[roads[i].to]) {
            cityID = roads[i].to;
            length = roads[i].length;
            cityIDFound = true;
            break;
        }
    }

    // If no unvisited city was found, pop a city from the stack
    if (!cityIDFound) {
        struct stackNode from = StackPop(&(agent->stack));
        cityID = from.cityID;
        length = from.cityLength;
    }
    
    if (agent->stamina < length) {
        // Since we don't have enough stamina, push the popped city
        // back onto the stack
        if (!cityIDFound) {
            StackPush(&(agent->stack), cityID, length);
        }
        move = (Move){agent->location, length};
    }
    else {
        // If an unvisited city was found, our current location onto
        // the stack
        if (cityIDFound) {
            StackPush(&(agent->stack), agent->location, length);
        }
        move = (Move){cityID, length};
        agent->dfsVisited[cityID] = true;
    }

    return move;
}

// Function for checking if the DFS needs to be reset
static void checkIfReset(Map m, Agent agent, int cityID) {
    // Assumes visited is true, then scans for any discrepancies
    int allVisited = true;
    for (int i = 0; i < MapNumCities(m); i++) {
        if (!agent->dfsVisited[i]) {
            allVisited = false;
        }
    }

    // If the agent has visited every city, reset the visited array
    if (allVisited) {
        resetDFS(m, agent, cityID);
    }
}

// Function that resets the DFS and marks the current city as visited
static void resetDFS(Map m, Agent agent, int cityID) {
    for (int i = 0; i < MapNumCities(m); i++) {
        agent->dfsVisited[i] = false;
    }

    agent->dfsVisited[cityID] = true;
}

// Function for finding the Least Turns Path
static Move chooseLTP(Map m, Agent agent) {
    Move move;
    int *pred = malloc(MapNumCities(m) * sizeof(int));
    int *staminaCost = malloc(MapNumCities(m) * sizeof(int));

    // Run dijkstra's algorithm which modifies the pred and staminaCost arrays
    dijkstra(m, agent->location, agent->stamina,
             agent->maxStamina, pred, staminaCost);

    // Starts the cityID at the thief's location and goes down the
    // pred array until we are 1 away from our current location
    int cityID = agent->thiefLocation;
    while (pred[cityID] != agent->location) {
        cityID = pred[cityID];
    }

    // If there is no stamina,
    // don't move and set the city as undefined (unknown)
    if (agent->stamina < staminaCost[cityID]) {
        move = (Move){agent->location, 0};
        cityID = UNDEFINED;
    }
    else {
        move = (Move){cityID, staminaCost[cityID]};
    }
    
    // If we have reached the thief's location, set
    // the thief's location back to undefined (unknown)
    if (cityID == agent->thiefLocation) {
        agent->thiefLocation = UNDEFINED;
    }

    // Frees the used arrays and returns the move
    free(pred);
    free(staminaCost);
    return move;
}

// Function that uses dijkstra's algorithm to find a path with
// the least number of turns
static void dijkstra(Map m, int source, int currStamina,
                     int maxStamina, int *pred, int *staminaCost) {
    // Initialises the numCities and declares
    // numTurns, stamina, visited, cost and roads array
    int numCities = MapNumCities(m);
    int *numTurns = malloc(numCities * sizeof(int));
    int *stamina = malloc(numCities * sizeof(int));
    bool *visited = malloc(numCities * sizeof(bool));
    double *cost = malloc(numCities * sizeof(double));
    Road *roads = malloc(numCities * sizeof(Road));

    // Initialises the arrays and allocates memory for the road array
    for (int i = 0; i < numCities; i++) {
        numTurns[i] = 0;
        pred[i] = UNDEFINED;
        stamina[i] = UNDEFINED;
        staminaCost[i] = UNDEFINED;
        visited[i] = false;
        cost[i] = INT_MAX;
    }
    
    // Sets the array's source values
    numTurns[source] = 0;
    stamina[source] = currStamina;
    staminaCost[source] = 0;
    cost[source] = -1;

    for (int count = 0; count < numCities - 1; count++) {
        int currCity;
        int minCost = INT_MAX;

        // Finds an unvisited city with the smallest cost
        for (int i = 0; i < numCities; i++) {
            if (!visited[i] && (cost[i] < minCost)) {
                minCost = cost[i];
                currCity = i;
            }
        }

        // Creates an array of adjacent roads to currCity
        int numRoads = MapGetRoadsFrom(m, currCity, roads);

        // Goes through every adjacent road checking if they are visited
        for (int i = 0; i < numRoads; i++) {
            int nextCity = roads[i].to;

            // If we haven't visited the next checked city
            if (!visited[nextCity]) {
                visitCity(i, source, nextCity, stamina, roads,
                          staminaCost, numTurns, pred, cost, maxStamina);
            }
        }

        // Marks the city as visited
        visited[currCity] = true;
    }
    
    // Frees all the used arrays
    free(roads);
    free(stamina);
    free(visited);
    free(numTurns);
    free(cost);
}

// "Visits" the city by calculating its number of turns and
// filling in the arrays
static void visitCity(int i, int source, int nextCity, int stamina[],
                      Road roads[], int staminaCost[], int numTurns[],
                      int pred[], double cost[], int maxStamina) {
    // Initialises the number of turns and current stamina
    int addTurns = 1;
    int currStamina;
    // Checks if we need to wait, if we do add a turn and update accordingly
    if (stamina[roads[i].from] < roads[i].length) {
        currStamina = maxStamina - roads[i].length;
        addTurns++;
    }
    else {
        currStamina = stamina[roads[i].from] - roads[i].length;
    }

    // Create a cost variable that keeps track of turns and stamina
    double newCost = (numTurns[roads[i].from] + addTurns)
                     - ((double)currStamina) / maxStamina;
    
    // Update the arrays if our newly found cost is less than the previous
    if (cost[nextCity] > newCost) {
        cost[nextCity] = newCost;
        pred[nextCity] = roads[i].from;
        stamina[nextCity] = currStamina;
        staminaCost[nextCity] = roads[i].length;
        numTurns[nextCity] = numTurns[roads[i].from] + addTurns;
    }
}

// Takes an array with all the possible roads and puts the ones the agent
// has enough stamina for into the legalRoads array
// Returns the number of legal roads
static int filterRoads(Agent agent, Road roads[], int numRoads,
                       Road legalRoads[]) {
    int numLegalRoads = 0;
    for (int i = 0; i < numRoads; i++) {
        if (roads[i].length <= agent->stamina) {
            legalRoads[numLegalRoads++] = roads[i];
        }
    }
    return numLegalRoads;
}

/**
 * Executes a given move by updating the agent's internal state
 */
void AgentMakeNextMove(Agent agent, Move move) {
    if (move.to == agent->location) {
        agent->stamina = agent->maxStamina;
    } else {
        agent->stamina -= move.staminaCost;
    }
    agent->location = move.to;
    
    // TODO: You may need to add to this to handle different strategies
}

////////////////////////////////////////////////////////////////////////
// Gets information about the agent
// NOTE: It is expected that these functions do not need to be modified

/**
 * Gets the name of the agent
 */
char *AgentName(Agent agent) {
    return agent->name;
}

/**
 * Gets the current location of the agent
 */
int AgentLocation(Agent agent) {
    return agent->location;
}

/**
 * Gets the amount of stamina the agent currently has
 */
int AgentStamina(Agent agent) {
    return agent->stamina;
}

////////////////////////////////////////////////////////////////////////
// Learning information

/**
 * Tells the agent where the thief is
 */
void AgentGainInfo(Agent agent, int thiefLocation) {
    agent->thiefLocation = thiefLocation;
}

////////////////////////////////////////////////////////////////////////
// Displaying state

/**
 * Prints information about the agent (for debugging purposes)
 */
void AgentShow(Agent agent) {
    // TODO: You can implement this function however you want
    //       You can leave this function blank if you want
}

////////////////////////////////////////////////////////////////////////
