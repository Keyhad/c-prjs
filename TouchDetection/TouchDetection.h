#ifndef TOUCH_DETECTION
#define TOUCH_DETECTION

// Definitions
//------------
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

/* The number of rows and columns, acceptable values are <50 for each or <75 in total */
#define ROW_COUNT           11
#define COLUMN_COUNT        5

/* The maximum number of supported touches acceptable values are 2 (other??) */
#define MAX_TOUCH_COUNT     2

/* Information about one single touch
 * (_x, _y) is the center of the press calculated as the vector sum 
 * of contributing sensors (X, Y) = ((x0*f0 + x1*f1 + ...)/(f0+f1+...),
 *                                   (y0*f0 + y1*f1 + ...)/(f0+f1+...)) 
 * where x0,x1,... are all the sensors considered to be part of the touch
 */
typedef struct TOUCH {
    uint16_t _x;
    uint16_t _y;
    uint16_t _force;
    uint8_t _count;
} TOUCH;

TOUCH _positions [MAX_TOUCH_COUNT + 1];

/* Fill in the array _positions by iterating getForce for all sensors 
 *  pHandle - a handle for usage in the call to getForce 
 *  return - void, _positions should have updated values
 */
void detectTouches(void* pHandle);
void detectTouches(void* pHandle);

#endif