// Patrick Musau
// 8-2014
// Dynamics header file for real-time reachability

#ifndef DYNAMICS_BICYCLE_H_
#define DYNAMICS_BICYCLE_H_

#include <stdbool.h>

#include "geometry.h"

/**
 * Get the bounds on the derivative in a region of space at a range of times
 */
double get_derivative_bounds_bicycle(HyperRectangle* rect, int faceIndex,REAL heading_input, REAL throttle);

#endif