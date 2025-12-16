#include <stdio.h>
int main() {
    int *ptr = NULL;
    *ptr = 5; // Null pointer dereference
    printf("%d\n", *ptr);
    return 0;
}
