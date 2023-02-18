#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>


char *read_from_file(const char *filename)
{
    long int size = 0;
    FILE *file = fopen(filename, "r");
    
    if(!file) {
        fputs("File error.\n", stderr);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);

    char *result = (char *) malloc(size);
    if(!result) {
        fputs("Memory error.\n", stderr);
        fclose(file);
        return NULL;
    }

    if(fread(result, 1, size, file) != size) {
        fputs("Read error.\n", stderr);
        fclose(file);
        return NULL;
    }

    fclose(file);
    return result;
}

int randint(int n) {
  if ((n - 1) == RAND_MAX) {
    return rand();
  } else {
    // Supporting larger values for n would requires an even more
    // elaborate implementation that combines multiple calls to rand()
    assert (n <= RAND_MAX);

    // Chop off all of the values that would cause skew...
    int end = RAND_MAX / n; // truncate skew
    assert (end > 0);
    end *= n;

    // ... and ignore results from rand() that fall above that limit.
    // (Worst case the loop condition should succeed 50% of the time,
    // so we can expect to bail out of this loop pretty quickly.)
    int r;
    while ((r = rand()) >= end);

    return r % n;
  }
}

int main(int argc, char **argv) 
{
    srand(time(NULL));
    int n = atoi(argv[2]);
    int m = atoi(argv[3]);
    int rand1, aux;
    if(argc < 2) {
        fputs("Need an argument.\n", stderr);
        return -1;
    }

    char *result = read_from_file(argv[1]);

    if(!result) return -1;
    int randomnumber;
    for(int j=0; j<n; j++){
        randomnumber = rand() % RAND_MAX;
        while (randomnumber > strlen(result)){
            randomnumber = rand() % RAND_MAX;
        }
        aux = randomnumber+m;
        printf(">");
        for (int i=randomnumber;i<aux;i++){
            printf("%c", result[i]);
        }
        printf("< \n");
    }
    
    free(result);

    return 0;
} 