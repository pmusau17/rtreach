// Patrick Musau
// Vehicle Bicycle model header

#ifndef BICYCLE_H_
#define BICYCLE_H_

#include <stdbool.h>
#include "geometry.h"
#include "main.h"

// run reachability for a given wall timer (or iterations if negative)
bool runReachability(REAL* start, REAL simTime, REAL wallTimeMs, REAL startMs);

#endif