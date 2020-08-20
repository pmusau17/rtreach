// Patrick Musau
// Vehicle Bicycle model header

#ifndef BICYCLE_H_
#define BICYCLE_H_

#include "main.h"
#include "geometry.h"
#include <stdbool.h>



// run reachability for a given wall timer (or iterations if negative)
bool runReachability(REAL* start, REAL simTime, REAL wallTimeMs, REAL startMs);
REAL getSimulatedSafeTime(REAL start[4],REAL heading_input, REAL throttle);

#endif