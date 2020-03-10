#ifndef TOUCH_DETECTION
#define TOUCH_DETECTION
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

// Definitions
//------------

typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

/* The number of rows and columns, acceptable values are <50 for each or <75 in total */
const int ROW_COUNT = 11;
const int COLUMN_COUNT = 5;

/* The maximum number of supported touches acceptable values are 2 (other??) */
const int MAX_TOUCH_COUNT = 2;

/* Information about one single touch
 * (_x, _y) is the center of the press calculated as the vector sum 
 * of contributing sensors (X, Y) = ((x0*f0 + x1*f1 + ...)/(f0+f1+...),
 *                                   (y0*f0 + y1*f1 + ...)/(f0+f1+...)) 
 * where x0,x1,... are all the sensors considered to be part of the touch
 */
typedef struct S_Touch {
    uint16_t _x;
    uint16_t _y;
    uint16_t _force;
    uint8_t _count;
} T_Touch;

T_Touch _positions [MAX_TOUCH_COUNT + 1] = 0;

/* Fill in the array _positions by iterating getForce for all sensors 
 *  pHandle - a handle for usage in the call to getForce 
 *  return - void, _positions should have updated values
 */
void detectTouches(void* pHandle);

#endif