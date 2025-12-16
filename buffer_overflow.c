#include <stdio.h>
int main() {
    int arr[3];
    arr[5] = 42; // Buffer overflow
    printf("%d\n", arr[5]);
    return 0;
}
