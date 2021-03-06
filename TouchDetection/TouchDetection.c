#include <stdlib.h>
#include <string.h>
#include "TouchDetection.h"
/* 
 * Purpose: 
 * Detect presses and accumulate/summarize the force for each press
 *
 * Detail: 
 * Find a cluster of activated sensors and fill in the information in the _positions array
 *   Example 1:
 *   0 0 0 0
 *   3 0 5 0
 *   0 0 0 0
 *   Result: _positions = {{ 0.0, 1.0, 3.0, 1}, { 2.0, 1.0, 5.0, 1}};
 *
 *   Example 2:
 *   1 4 0 0
 *   5 0 0 4
 *   0 0 0 4
 *   Result: _positions = {{ 0.4, 0.5, 10.0, 3}, { 3.0, 1.5, 8.0, 2}};
 *  
 * The definition of a cluster is adjecent positions with non-zero force, only diagonal adjecent
 * cells are optional to include or not. (The best is to include them)
 * To allow for center of presses to be between row and columns the use of 8.8 fix point numbers 
 * are encouraged. The use of float for _x and _y are an acceptable simplification if fix point is
 * not available. 8.8 fix point can be implemented by the simplest method described here:
 * http://x86asm.net/articles/fixed-point-arithmetic-and-tricks/#Basic-Arithmetic-Operations
 * 
 * If there are more than MAX_TOUCH_COUNT of clusters then the sum of the forces for the remaining
 * sensors should be stored in "_positions[MAX_TOUCH_COUNT]._force".
 * 
 */


// External functions
//-------------------

/*
 * Get the force from the sensor at given position.
 *  pHandle - a handle to the abstract sensor
 *  col - The column [0, COLUMN_COUNT-1]
 *  row - The row [0, ROWCOUNT - 1]
 *  return - 8-bit value representing the force
 */
uint8_t getForce(void* pHandle, int col, int row);

/*
dimension iof a cluster
*/
#define CLUSTER_ROW_COUNT     2
#define CLUSTER_COLUMN_COUNT  2

#define MAX_CLUSTERS    (ROW_COUNT - CLUSTER_ROW_COUNT + 1) * \
                        (COLUMN_COUNT - CLUSTER_COLUMN_COUNT + 1)

typedef struct S_Cluster {
    int x;
    int y;
    int forces[CLUSTER_ROW_COUNT * CLUSTER_COLUMN_COUNT];
    int force;
    int count;
} T_Cluster;


/* Fill in the array _positions by iterating getForce for all sensors 
 *  pHandle - a handle for usage in the call to getForce 
 *  return - void, _positions should have updated values
 */
void detectTouches(void* pHandle) 
{
    int touchTable[ROW_COUNT][COLUMN_COUNT];

    // scan for forces and initialize touch table
    for (int yi = 0; yi < ROW_COUNT; yi++) 
    {
        for (int xi = 0; xi < COLUMN_COUNT; xi++) 
        {
            touchTable[yi][xi] = getForce(pHandle, xi, yi);
        }
    }

    // find clusters values
    T_Cluster clusters[MAX_CLUSTERS];
    for (int ci = 0; ci < MAX_CLUSTERS; ci++) {
        T_Cluster *cluster = &clusters[ci];
        cluster->x = ci % COLUMN_COUNT;
        cluster->y = ci / ROW_COUNT;
        
        int f0 = touchTable[clusters->y][cluster->x];
        int f1 = touchTable[clusters->y][cluster->x + 1];
        int f2 = touchTable[clusters->y + 1][cluster->x];
        int f3 = touchTable[clusters->y + 1][cluster->x + 1];

        cluster->force = f0 + f1 + f2 + f3;
        cluster->count = (f0 ? 1 : 0) +
                         (f1 ? 1 : 0) +
                         (f2 ? 1 : 0) +
                         (f3 ? 1 : 0);
        cluster->forces[0] = f0;
        cluster->forces[1] = f1;
        cluster->forces[2] = f2;
        cluster->forces[3] = f3;
    }

    // sort clusters by force value
    for (int ci = 0; ci < MAX_CLUSTERS; ci++) {
        for (int cii = MAX_CLUSTERS - 1; cii > ci; cii--) {
            if (clusters[cii].force > clusters[cii - 1].force) {
                // swap the record
                T_Cluster temp;
                memcpy(&temp, &clusters[cii - 1], sizeof(T_Cluster));
                memcpy(&clusters[cii - 1], &clusters[cii], sizeof(T_Cluster));
                memcpy(&clusters[cii], &temp, sizeof(T_Cluster));
            }
        }
    }

    // reset _positions to zero
    memset(_positions, 0, sizeof(TOUCH) * (MAX_TOUCH_COUNT + 1));

    for (int ci = 0; ci < MAX_TOUCH_COUNT; ci++) 
    {
        T_Cluster *cluster = &clusters[ci];
        if (clusters->force > 0) {
            // X = (x0*f0 + x1*f1 + ...)/(f0+f1+...)
            _positions[ci]._x = 0;
            _positions[ci]._x += cluster->x * cluster->forces[0];
            _positions[ci]._x += (cluster->x + 1) * cluster->forces[1];
            _positions[ci]._x += cluster->x * cluster->forces[2];
            _positions[ci]._x += (cluster->x + 1) * cluster->forces[3];
            _positions[ci]._x /= cluster->force;
            // Y = (y0*f0 + y1*f1 + ...)/(f0+f1+...)
            _positions[ci]._y = 0;
            _positions[ci]._y += cluster->y * cluster->forces[0];
            _positions[ci]._y += cluster->y * cluster->forces[1];
            _positions[ci]._y += (cluster->y + 1) * cluster->forces[2];
            _positions[ci]._y += (cluster->y + 1) * cluster->forces[3];
            _positions[ci]._y /= cluster->force;

            _positions[ci]._force = clusters[ci].force;
            _positions[ci]._count = clusters[ci].count;
        }
    }

    for (int ci = 2; ci < MAX_CLUSTERS; ci++) 
    {
        // put sum of the other forces here
        _positions[MAX_TOUCH_COUNT]._force += clusters[ci].force;
    }
}