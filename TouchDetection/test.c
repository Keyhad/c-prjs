#include <stdio.h>
#include <string.h>
#include "TouchDetection.h"

// help data & functions
uint8_t test_forces[] = {1, 2, 0, 0, 3, 4, 5, 6, 7, 8, 9, 0, 1, 1, 2, 3, 4, 5, 6, 7, 8, 0, 1, 2, 3, 4, 0, 0, 0, 0};

uint8_t getForce(void* pHandle, int col, int row)
{
    return ((int*)pHandle)[row * COLUMN_COUNT + col];
}

/*
* unit test #1
*/
void test_detectTouches() 
{
    detectTouches(test_forces);
    
    printf("#1-1 %s\n", _positions[0]._force ? "PASS" : "FAIL");
    printf("#1-2 %s\n", _positions[1]._force ? "PASS" : "FAIL");
}

void main() 
{
    printf("Touch detection tests\n");

    test_detectTouches();
}

