#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

// Auxillary function which deletes an element from array of ints
void delete(int* arr, int size, int t) {
    int num, check = 0;
    for (size_t i = 0; i < size; i++) {
        if (arr[i] == t) {
            num = i; // Find index of this element
            check = 1;
        }
    }if (check){
        for (size_t i = num; i < size - 1; i++) {
        arr[i] = arr[i+1]; // Move all elements after this element
        }
    }
}

// Auxillary function which checks if an element is in array (for ints)
int isIn(int* arr, int t, int size){
    for (size_t i = 0; i < size; i++) {
        if (arr[i] == t){
            return 1;
        }
    }
    return 0;
}

// Struct for the table for the algorithm
struct tableElement{
    int num; // Number a.k.a name for this particular city
    int distFromStart; // Distance from sart city
    int depSize; // Size of the array of previous cities
    int departure[100]; // Num of cities with shortest path from start city which lead to this city
};

void treeModifier(struct tableElement* invM, struct tableElement el, int start, struct tableElement* table) {
    for (size_t i = 0; i < el.depSize; i++) { // Go through each previous city for each city
        if (!(isIn(invM[el.departure[i]].departure, el.num, invM[el.departure[i]].depSize))) { //Check if the city is not already in the list
            invM[el.departure[i]].departure[invM[el.departure[i]].depSize] = el.num; // Assign to each previous city this particular city as next
            invM[el.departure[i]].depSize++;
        }
        if (invM[el.departure[i]].num != start){
            treeModifier(invM, table[el.departure[i]], start, table); // Run function for each previous city if it is not the base case
        }
    }
}

void countNumberOfPaths(struct tableElement el, int* cnt, int end, struct tableElement* invM) { // Recursive function used to count the number of paths and writes it to the gobal variable
    if (el.num == end) {
        ++*cnt;
        return;
    }
    for (size_t i = 0; i < el.depSize; i++) {
        countNumberOfPaths(invM[el.departure[i]], cnt, end, invM);
    }
}

void printArr(int arr[], int len, int* cnt, FILE *output){ // Function which takes list of cities and prints the correct form of path from first city to last
    fprintf(output, "%d. ", *cnt);
    for (size_t i = 0; i < len - 1; i++) {
        fprintf(output, "%d->", arr[i]);
    }
    fprintf(output, "%d\n", arr[len-1]);
}

// Out is the recursive function which forms the paths of or tree
// I takes the "node" adds it to a temrary path and runs itself for each of "childs"
// Global variable num is used for printing the number of current path and is increased each iteration
void out(struct tableElement el, int paths[], int len, int* num, int end, FILE* output, struct tableElement* invM) {
    paths[len] = el.num;
    len++;
    if(el.num == end){
        printArr(paths, len, num, output); // Print the path if this element is the destination
        ++*num;
    } else{
        for (size_t i = 0; i < el.depSize; i++) {
            out(invM[el.departure[i]], paths, len, num, end, output, invM);
        }
    }
}

int main() {
    FILE *input;
    FILE *output;
    input = fopen("input.txt", "r");
    output = fopen("AlexanderKrivonosovOutput.txt", "w");
    int numOfCities, start, end, flag;
    flag = fscanf(input, "%d %d %d\n", &numOfCities, &start, &end);
    if (flag == 0 || flag == 1 || flag == 2){
        fprintf(output, "Structure of the input is invalid"); // Check if the input is not 3 numbers
        return 1;
    }
    if (numOfCities > 50 || numOfCities < 5){
        fprintf(output, "Number of cities is out of range");
        return 1;
    }else if (start >= numOfCities){
        fprintf(output, "Chosen initial city does not exist");
        return 1;
    }else if(end >= numOfCities){
        fprintf(output, "Chosen destination city does not exist");
        return 1;
    }
    int dots[51][51];
    for (size_t i = 0; i < numOfCities; i++) { // Fill the matrix
        for (int j = 0; j < numOfCities; j++) {
            flag = fscanf(input, "%d", &dots[i][j]);
            if (flag != 1){ // Check if it is not a number
                char c = fgetc(input);
                if ((int)c != 42){ // check if it is not an asterisk
                    fprintf(output, "Structure of the input is invalid");
                    return 1;
                }else if(c == '\n'){
                    fprintf(output, "Matrix size does not suit to the number of cities"); // Check if the matrix has redundant elements
                    return 1;
                }
                dots[i][j] = -1; // If it is an asterisk make it equal to minus one
            }
            if (dots[i][j] > 20) {
                fprintf(output, "The distance between some cities is out of range");
                return 1;
            }
        }
    }
    if (fgetc(input) != EOF) {
        fprintf(output, "Matrix size does not suit to the number of cities");
        return 1;
    }

    int unvisited[51]; // List of unvisited cities

    struct tableElement table[51]; // Array of cities
    for (size_t i = 0; i < numOfCities; i++) {
        if (i == start) {
            table[i].distFromStart = 0;
        } else {
            table[i].distFromStart = INT_MAX;
        }
        table[i].num = i;
        table[i].departure[0] = i;
        table[i].depSize = 1;
        unvisited[i] = table[i].num; // Fill thae array of unvisited
    }

    //Here the main algorithm goes
    int unvisitedCount = numOfCities;
    int currentMinValue; // Auxillary variable for finding of minimum among unvisited cities
    int currentMin; // Auxillary variable for finding of minimum among unvisited cities
    for (size_t i = 0; i < numOfCities; i++) {
        currentMinValue = INT_MAX;
        for (size_t j = 0; j < numOfCities; j++) { // Search for minimum dist. from initial sity among unvisited
            if ((table[j].distFromStart < currentMinValue) && isIn(unvisited, table[j].num, unvisitedCount)) {
                currentMinValue = table[j].distFromStart;
                currentMin = table[j].num;
            }
        }
        for (size_t j = 0; j < numOfCities; j++) {
            if (isIn(unvisited, table[j].num, unvisitedCount) && table[j].num != currentMin) { // Go through all cities which can be reached from this city
                if ((dots[currentMin][j] > 0) && (dots[currentMin][j] + table[currentMin].distFromStart < table[j].distFromStart)) { // Sum the distance to this city and one of the following and check if it is less than exiting one
                    table[j].distFromStart = dots[currentMin][j] + table[currentMin].distFromStart; // If so, update dist.
                    table[j].departure[0] = table[currentMin].num; // Reset the list of shortest paths
                    table[j].depSize = 1; // Reset it's length
                } else if ((dots[currentMin][j] > 0) && (dots[currentMin][j] + table[currentMin].distFromStart == table[j].distFromStart)) { // If it is equal to minimal
                    table[j].departure[table[j].depSize] = table[currentMin].num; // Add this city to list
                    table[j].depSize++; // Increase it's length
                }
            }
        }
        delete(unvisited, unvisitedCount, currentMin); // Decrease the number of unvisited cities
        unvisitedCount--;
    }
    //End of the main algorithm

    if (table[end].distFromStart == INT_MAX) {
        fprintf(output, "Initial and destination cities are not connected\n"); // If distance to our destination city was not chenged, then our cities are not connected
        return 1;
    }

    // Now we have the table of cities with minimal dist from start and list of cities we came from. It is the same as having a tree of cities:
    //            end
    //          /  |  \
    //     city  city  city
    //      |     |     |  \
    //   start  city  city start
    //           |      |
    //         start  start
    // This is an example of the tree we've got. The leafs are always the initial city, and the root is always end. To output it we should reverse the tree

    //Start of the output algorithm
    struct tableElement invM[51]; // Array with reverse tree
    for (size_t i = 0; i < numOfCities; i++){
        invM[i].num = i;
        invM[i].depSize = 0;
    }

    treeModifier(invM, table[end], start, table); // Apply the transformation

    int cnt = 0;
    countNumberOfPaths(invM[start], &cnt, end, invM);

    int num = 1;
    int paths[100];
    fprintf(output, "The shortest path is %d\n", table[end].distFromStart);
    fprintf(output, "The number of shortest paths is %d\n", cnt);
    out(invM[start], paths, 0, &num, end, output, invM);
    // End of the output algorithm

    fclose(input);
    fclose(output);
    return 0;
}
