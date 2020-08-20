#include "bicycle_model.h"
#include "main.h"
#include "face_lift.h"
#include "util.h"
#include "simulate.h"
#include <stdio.h>




// function that stops simulation 
bool shouldStop(REAL state[NUM_DIMS], REAL simTime, void* p)
{
	bool rv = false;
    return rv;
}

// This function enumerates all of the corners of the current HyperRectangle and 
// returns whether or not any of the points lies outside of the ellipsoid
bool finalState(HyperRectangle* rect)
{
	REAL points[16][4] =
	{
		{rect->dims[0].min, rect->dims[1].min, rect->dims[2].min, rect->dims[3].min},
		{rect->dims[0].min, rect->dims[1].min, rect->dims[2].min, rect->dims[3].max},
		{rect->dims[0].min, rect->dims[1].min, rect->dims[2].max, rect->dims[3].min},
		{rect->dims[0].min, rect->dims[1].min, rect->dims[2].max, rect->dims[3].max},

		{rect->dims[0].min, rect->dims[1].max, rect->dims[2].min, rect->dims[3].min},
		{rect->dims[0].min, rect->dims[1].max, rect->dims[2].min, rect->dims[3].max},
		{rect->dims[0].min, rect->dims[1].max, rect->dims[2].max, rect->dims[3].min},
		{rect->dims[0].min, rect->dims[1].max, rect->dims[2].max, rect->dims[3].max},

		{rect->dims[0].max, rect->dims[1].min, rect->dims[2].min, rect->dims[3].min},
		{rect->dims[0].max, rect->dims[1].min, rect->dims[2].min, rect->dims[3].max},
		{rect->dims[0].max, rect->dims[1].min, rect->dims[2].max, rect->dims[3].min},
		{rect->dims[0].max, rect->dims[1].min, rect->dims[2].max, rect->dims[3].max},

		{rect->dims[0].max, rect->dims[1].max, rect->dims[2].min, rect->dims[3].min},
		{rect->dims[0].max, rect->dims[1].max, rect->dims[2].min, rect->dims[3].max},
		{rect->dims[0].max, rect->dims[1].max, rect->dims[2].max, rect->dims[3].min},
		{rect->dims[0].max, rect->dims[1].max, rect->dims[2].max, rect->dims[3].max},
	};

	REAL maxPotential = potential(points[0][0], points[0][1], points[0][2], points[0][3]);

	for (int i = 1; i < 16; ++i)
	{
		REAL p = potential(points[i][0], points[i][1], points[i][2], points[i][3]);

		if (p > maxPotential)
			maxPotential = p;
	}

	DEBUG_PRINT("--->  potential of final state = %f\n", maxPotential);

	return maxPotential < 1;
}

// Simulation 
REAL getSimulatedSafeTime(REAL start[4])
{
	REAL stepSize = 0.02f;
	REAL rv = 0.0f;

	simulate(start, stepSize, shouldStop, (void*)&rv); // TODO: look here

	//DEBUG_PRINT("time until simulation reaches safe state = %f\n", rv);

	return rv;
}

// called on states reached during the computation
bool intermediateState(HyperRectangle* r)
{
	bool allowed = true;
	const REAL FIFTEEN_DEGREES_IN_RADIANS = 0.2618;

	// check if the constraints are satisfied

	if (r->dims[0].min < -1 || r->dims[0].max > 1) // position limits
		allowed = false;
	else if (r->dims[1].min < -1 || r->dims[1].max > 1) // velocity limits
		allowed = false;
	else if (r->dims[2].min < -FIFTEEN_DEGREES_IN_RADIANS || r->dims[2].max > FIFTEEN_DEGREES_IN_RADIANS)
		allowed = false;

	return allowed;
}




bool runReachability(REAL* start, REAL simTime, REAL wallTimeMs, REAL startMs)
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

	return face_lifting_iterative_improvement_bicycle(startMs, &set);
}




