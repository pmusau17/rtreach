// Stanley Bak
// 4-2014

#include <stdio.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>

#include "main.h"
#include "face_lift.h"
#include "util.h"

// make a face's neighborhood of a given width
void make_neighborhood_rect(HyperRectangle* out, int f,
		HyperRectangle* bloatedRect, HyperRectangle* originalRect, REAL nebWidth)
{
	*out = *bloatedRect;

	/*printf("out: ");
	print(out);
	printf("\n\n");

	printf("bloatedRect: ");
	print(bloatedRect);
	printf("\n");

	printf("originalRect: ");
	print(originalRect);
	printf("\n");*/



	bool isMin = (f % 2) == 0;
	int d = f / 2;

	// flatten
	if (isMin)
	{
		out->dims[d].min = originalRect->dims[d].min;
		out->dims[d].max = originalRect->dims[d].min;
	}
	else
	{
		out->dims[d].min = originalRect->dims[d].max;
		out->dims[d].max = originalRect->dims[d].max;
	}

	// swap if nebWidth was negative
	if (nebWidth < 0)
		out->dims[d].min += nebWidth;
	else
		out->dims[d].max += nebWidth;
}

// necessary to guarantee loop termination
const REAL MAX_DER = 99999;
const REAL MIN_DER = -99999;

// do a single face lifting operation
// et (error tracker) is set if you want to track the sources of errors, can be null
// returns time elapsed
REAL lift_single_rect(HyperRectangle* rect, REAL stepSize, REAL timeRemaining)
{
  #ifdef DEBUG
	static int debugNumCalls = 0;
	++debugNumCalls;
	//HyperRectangle debug_initialRect = *rect;
  #endif

	////////////////////////////////////////////////
	// estimate the widths of the neighborhoods   //
	// construct bloated rect (for neighborhoods) //

	HyperRectangle bloatedRect = *rect;
	REAL nebWidth[NUM_FACES];

	// initially, estimate nebWidth based on the derivative in the center of the rectangle we care about

	for (int f = 0; f < NUM_FACES; ++f)
		nebWidth[f] = 0;

	bool needRecompute = true;
	REAL minNebCrossTime;
	REAL ders[NUM_FACES];
	
	// Printing Patrick
	printf("rect: ");
	print(rect);
	printf("\n\n");


	while (needRecompute)
	{
		needRecompute = false;
		minNebCrossTime = DBL_MAX;

		for (int f = 0; f < NUM_FACES; ++f)
		{
			int dim = f / 2;
			bool isMin = (f % 2) == 0;

			HyperRectangle faceNebRect;

			// make candidate neighborhood
			make_neighborhood_rect(&faceNebRect, f, &bloatedRect, rect, nebWidth[f]);
			
			printf("faceNebRect: %d :",f);
			print(&faceNebRect);
			printf("\n");

			// test derivative inside neighborhood

			// the projection of the derivative on the outward norm e_i- is -fi(x) and fi(x) for e_i
			REAL der = get_derivative_bounds(&faceNebRect, f);


			// so we cap the derivative at 999999 and min at the negative of that.
			if (der > MAX_DER) {
				der = MAX_DER;
                        }
			else if (der < MIN_DER) {
				der = MIN_DER;
                        }

			REAL prevNebWidth = nebWidth[f];
			REAL newNebWidth = der * stepSize; // the projection of the derivative is tempered by the stepSize
			
			// if it is a negative facing face the derivative is negative if it is less than 0.
			// if it is a positive facing face the derivative has to be positive to grow and outward.
			
			bool grewOutward = (isMin && newNebWidth < 0) || (!isMin && newNebWidth > 0); 

			// check if the previous nebWidth grewOutward.

			bool prevGrewOutward = (isMin && prevNebWidth < 0) || (!isMin && prevNebWidth > 0);

			// prevent flipping from outward face to inward face
			if (!grewOutward && prevGrewOutward)
			{
				newNebWidth = 0;
				der = 0;
			}

			// if flipping from inward to outward
			if (!prevGrewOutward && grewOutward) {
				needRecompute = true;
                        }

			// 2nd condition to recompute, der doubled (which means neb width is twice what it was before)
			if (fabs(newNebWidth) > 2 * fabs(prevNebWidth)) {
				needRecompute = true;
                        }

			// adjust bloated rect only if we are requiring a later recomputation
			if (needRecompute)
			{
				nebWidth[f] = newNebWidth;

				if (isMin && nebWidth[f] < 0)
					bloatedRect.dims[dim].min = rect->dims[dim].min + nebWidth[f];
				else if (!isMin && nebWidth[f] > 0)
					bloatedRect.dims[dim].max = rect->dims[dim].max + nebWidth[f];
			}
			else
			{
				// might be the last iteration, compute min time to cross face

				// clamp derivative if it changed direction
				// this means along the face it's inward, but in the neighborhood it's outward
				if (der < 0 && prevNebWidth > 0) {
					der = 0;
                                }
				else if (der > 0 && prevNebWidth < 0) {
					der = 0;
                                }

				if (der != 0)
				{
					REAL crossTime = prevNebWidth / der;

					if (crossTime < minNebCrossTime) {
						minNebCrossTime = crossTime;
                                        }
				}

				ders[f] = der;
			}
		}
	}

	if (minNebCrossTime * 2 < stepSize)
	{
		//printf(": minNebCrossTime = %f, stepSize = %f\n", minNebCrossTime, stepSize);
		//printf(": debugNumCalls = %i\n", debugNumCalls);

		error_exit("minNebCrossTime is less than half of step size.");
	}

	//printf("\n");

	////////////////////////////////////////
	// lift each face by the minimum time //

	REAL timeToElapse = minNebCrossTime;

	// subtract a tiny amount time due to multiplication / division rounding
	timeToElapse = timeToElapse * 99999 / 100000;

	if (timeRemaining < timeToElapse) {
		timeToElapse = timeRemaining;
        }

	// do the lifting
	for (int d = 0; d < NUM_DIMS; ++d)
	{
		rect->dims[d].min += ders[2*d] * timeToElapse;
		rect->dims[d].max += ders[2*d+1] * timeToElapse;
	}

	if (!hyperrectangle_contains(&bloatedRect, rect, true))
	{
		//printf("error occurred when debugNumCalls = %d\n", debugNumCalls);
		//printf("rect = ");
		//println(&debug_initialRect);

		error_exit("lifted rect is outside of bloated rect");
	}

	return timeToElapse;
}

// generate a split rectangle
void generateSplitRectangle(HyperRectangle* rectToSplit, HyperRectangle* out,
		int iteratorVal, int splitDimensions[NUM_DIMS])
{
	for (int dimIndex = 0; dimIndex < NUM_DIMS; ++dimIndex)
	{
		int mask = splitDimensions[dimIndex];
		int splitNum = iteratorVal % mask;
		iteratorVal /= mask;

		REAL width = interval_width(&rectToSplit->dims[dimIndex]) / splitDimensions[dimIndex];

		// assign the current dimension
		out->dims[dimIndex].min = rectToSplit->dims[dimIndex].min + splitNum * width;
		out->dims[dimIndex].max = out->dims[dimIndex].min + width;
	}
}

///////////////// below is from header file ///////////////

bool face_lifting_iterative_improvement(int startMs, LiftingSettings* settings)
{
	bool rv = false;
	bool lastIterationSafe = false;

	set_error_print_params(settings);

	// Get the settings from the facelifting settings
	REAL stepSize = settings->initialStepSize;

	int iter = 0; // number of iterations

	while (true)
	{
		iter++;
		bool safe = true; // until proven otherwise

		// if we've cut the step size way too far where floating point error may be a factor
		if (stepSize < 0.0000001)
		{
			DEBUG_PRINT("Quitting from step size too small: stepSize: %0.10f at iteration: %d\n\r", stepSize, iter);
			rv = false;
			break;
		}

		if (settings->restartedComputation) {
			settings->restartedComputation();
                }

		// This function gets the reachtime passed from the settings 
		// In the case of the pendulum example it is 1.25*TimeToSafe
		REAL timeRemaining = settings->reachTime;

		// Get the initial set from which to perform reachability analysis.
		HyperRectangle trackedRect = settings->init;

		// Debugging for Patrick
		printf("iter: %d, stepSize: %f with ",iter,stepSize);
		print(&trackedRect);
		printf("\n");
		

		// Create a new hyperrectangle
		HyperRectangle hull;

		// compute reachability up to split time
		while (safe && timeRemaining > 0)
		{
			// reachedAtIntermediateTime is a function that makes sure that the current state satisfies 
			// the constraints. 
			if (settings->reachedAtIntermediateTime) {
				hull = trackedRect;
                        }

			// debug changed so error tracker is always passed in (see note)
			REAL timeElapsed = lift_single_rect(&trackedRect, stepSize, timeRemaining);

			// if we're not even close to the desired step size
			if (hyperrectange_max_width(&trackedRect) > settings->maxRectWidthBeforeError)
			{
				DEBUG_PRINT("maxRectWidthBeforeError exceeded at time %f, rect = ",
						settings->reachTime - timeRemaining);
				#if DEBUG
				println(&trackedRect);
				#endif
				// step size is too large, make it smaller and recompute
				safe = false;
			}
			else if (settings->reachedAtIntermediateTime)
			{
				hyperrectangle_grow_to_convex_hull(&hull, &trackedRect);

				safe = safe && settings->reachedAtIntermediateTime(&hull);
			}

			if (timeElapsed == timeRemaining && settings->reachedAtFinalTime)
				safe = safe && settings->reachedAtFinalTime(&trackedRect);

			timeRemaining -= timeElapsed;
		}

		int now = milliseconds();
		int elapsedTotal = now - startMs;

		DEBUG_PRINT("%dms: stepSize = %f\n",	elapsedTotal, stepSize);

		if (settings->maxRuntimeMilliseconds > 0)
		{
			int remaining = settings->maxRuntimeMilliseconds - elapsedTotal;

			if (remaining < 0)
			{
				// we've exceeded our time, use the result from the last iteration
				// note in a real system you would have an interrupt or something to cut off computation
				DEBUG_PRINT("Quitting from runtime maxed out\n\r");

				rv = lastIterationSafe;
				break;
			}

			if (safe)
			{
				// already safe with current step size, break
				DEBUG_PRINT("Found safe, stopping\n\r");
				rv = true;
				break;
			}
		}
		else
		{
			// runtime was negative, split a fixed number of times
			if (settings->maxRuntimeMilliseconds++ == 0)
			{
				DEBUG_PRINT("Splitting\n\r");
				rv = safe;
				break;
			}
		}

		lastIterationSafe = safe;

		// apply error-reducing strategy
		stepSize /= 2;
	}

	DEBUG_PRINT("iterations at quit: %d\n\r", iter);

	return rv;
}
