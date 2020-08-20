#include "bicycle_model.h"
#include "main.h"
#include "face_lift.h"
#include "util.h"
#include "simulate_bicycle.h"
#include <stdio.h>



// declaration 
bool face_lifting_iterative_improvement_bicycle(int startMs, LiftingSettings* settings, REAL heading_input, REAL throttle);

// function that stops simulation after two seconds
bool shouldStop(REAL state[NUM_DIMS], REAL simTime, void* p)
{
	bool rv = false;
    REAL maxTime = 2.0f;
    // stop if the maximum simulation time 
	if (simTime >= maxTime)
	{
		rv = true;

		REAL* stopTime = (REAL*)p;
		*stopTime = -1;
	}

	return rv;
}

// This function enumerates all of the corners of the current HyperRectangle and 
// returns whether or not any of the points lies outside of the ellipsoid
bool finalState(HyperRectangle* rect)
{
	REAL maxPotential = 0.0;

	DEBUG_PRINT("--->  potential of final state = %f\n", maxPotential);

	return maxPotential > 1;
}

// Simulation 
REAL getSimulatedSafeTime(REAL start[4],REAL heading_input,REAL throttle)
{
	REAL stepSize = 0.02f;
	REAL rv = 0.0f;

	simulate_bicycle(start, heading_input,throttle,stepSize, shouldStop, (void*)&rv); // TODO: look here

	//DEBUG_PRINT("time until simulation reaches safe state = %f\n", rv);

	return rv;
}

// called on states reached during the computation
bool intermediateState(HyperRectangle* r)
{
	bool allowed = true;
	//const REAL FIFTEEN_DEGREES_IN_RADIANS = 0.2618;

	// check hyper_rectangle enters the set x: [1,2], y: [1,2] for now

    println(r);
	if (r->dims[0].min > 1 && r->dims[0].max < 2) // position limits
		allowed = false;
	else if (r->dims[1].min > 1 || r->dims[1].max < 2) // velocity limits
		allowed = false;

	return allowed;
}




bool runReachability_bicycle(REAL* start, REAL simTime, REAL wallTimeMs, REAL startMs,REAL heading_input, REAL throttle)
{
	LiftingSettings set;
	printf("Starting reachability computation from the following state:\n");
	for (int d = 0; d < NUM_DIMS; ++d)
	{
		set.init.dims[d].min = start[d];
		set.init.dims[d].max = start[d];
		printf("[%f,%f]\n",set.init.dims[d].min,set.init.dims[d].max);
	}

	set.reachTime = simTime;
	set.maxRuntimeMilliseconds = wallTimeMs;

	REAL iss = set.reachTime;
//	iss = iss / 10.0f; // problem with division?
	iss = iss * 0.10f;

	DEBUG_PRINT("\n\rsimTime: %f\n\rreachTime: %f\n\r\n\r", simTime, set.reachTime);
	
	

	set.initialStepSize = iss; //set.reachTime / 10.0f;
	set.maxRectWidthBeforeError = 100;

	set.reachedAtFinalTime = finalState;
	set.reachedAtIntermediateTime = intermediateState;
	set.restartedComputation = 0; //restartedComputation;

	// debugging for patrick
	printf("Beginning Reachability Analysis >>>> initialStepSize: %f, reachTime: %f\n\n",set.initialStepSize,set.reachTime);

	return face_lifting_iterative_improvement_bicycle(startMs, &set,heading_input, throttle);
}




