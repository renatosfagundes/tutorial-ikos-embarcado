#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define NUM_DOORS 4

typedef struct {
    bool locked;
    uint8_t sensor_value;
} door_t;

door_t doors[NUM_DOORS];

void lock_all_doors() {
    for (int i = 0; i <= NUM_DOORS; i++) { // Off-by-one bug (should be < NUM_DOORS)
        doors[i].locked = true;
    }
}

void unlock_door(int idx) {
    if (idx >= 0 && idx < NUM_DOORS) {
        doors[idx].locked = false;
    }
}

void read_sensors(uint8_t* values, int len) {
    for (int i = 0; i < len; i++) {
        doors[i].sensor_value = values[i]; // No bounds check on i
    }
}

int main() {
    uint8_t sensor_data[NUM_DOORS] = {10, 20, 30, 40};
    lock_all_doors();
    unlock_door(2);
    read_sensors(sensor_data, NUM_DOORS + 1); // Intentional overflow
    printf("Door 2 locked: %d, sensor: %d\n", doors[2].locked, doors[2].sensor_value);
    return 0;
}