#include <stdio.h>
int main() {
    int x;
    int y = x + 1; // Use of uninitialized variable
    printf("%d\n", y);
    return 0;
}
