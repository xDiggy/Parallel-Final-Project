#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void generate_random_array(const char* filename, int size) {
    // Open the file for writing
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    // Declare the array
    fprintf(file, "    int random_%d[] = {", size);

    // Seed the random number generator
    srand(time(NULL));

    // Fill the array with random integers
    for (int i = 0; i < size; i++) {
        fprintf(file, "%d", rand() % 100); // Adjust the range of random numbers as needed
        if (i < size - 1) {
            fprintf(file, ", ");
        }
    }

    // Close the array declaration and the main function
    fprintf(file, "};\n\n");

    // Close the file
    fclose(file);

    printf("File '%s' created successfully.\n", filename);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <array_size>\n", argv[0]);
        return 1;
    }

    int size = atoi(argv[1]);
    if (size <= 0) {
        printf("Invalid array size!\n");
        return 1;
    }

    char newFileName[100];
    sprintf(newFileName, "random_array_%d.c", size);

    generate_random_array(newFileName, size);

    return 0;
}
