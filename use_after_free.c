#include <stdio.h>
#include <stdlib.h>
int main() {
    int *ptr = malloc(sizeof(int));
    *ptr = 10;
    free(ptr);
    *ptr = 20; // Use after free
    printf("%d\n", *ptr);
    return 0;
}
