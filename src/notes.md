# On Rtreach

These are my notes as I seek to understand how this tool works. Mostly I'm just looking to understand how this works with respect to the pendulum. At the end of this, I hope this is a useful guide for anyone who wishes to use this tool for real-time reachability. 

# Brief description of the pendulum problem 

The control objective is to move the cart from one position to another along the track with the pendulum still standing at the upright position #920; =0. The DC motor has only limited power and the track length has finite length, thus there exists certain sates of the physical system from which the pendulum cannot be steered back to the upright position. 
## Definitions/Reference

This [thesis](http://citeseerx.ist.psu.edu/viewdoc/download;jsessionid=37FB8AC6EF9877F0600CCEC69475A688?doi=10.1.1.2.4227&rep=rep1&type=pdf) will prove to be very helpful.

- **REAL**: double
- **AVR-GCC**: AVR-GCC is a compiler that takes C language high level code and creates binary source code that can be uploaded into an AVR micro contoller. 
- In C if you want indicate that the compiler should treat a decimal as a single precision floating pointer number you use f.
- **reach-time**:  the time we are computing reachability for.
- **runtime**: the wall time the method is allowed to run.
- **Neighborhood**: We define the neighborhood of a polyhedron F, as the polyhedron obtained by pushing outward each face e of F by an amount ve. The details are provided in the above thesis on page 89. 

### On x86 systems this is what gets executed when you run make 

```
gcc -std=gnu99 -O3 - Wall  main.c facelift.c geometry.c interval.c simulate.c util.c \ 
   dynamics_converging_oscillator.c dynamics_harmonic_oscillator.c dynamics_vanderpol.c \
   dyanmics_pendulum.c dynamics_pendulum_nonlinear.c pendulum.c -lm -o rtreach -DNONLINEAR_PENDULUM
```

## Entrypoint 

The main entrypoint for rtreach is [main.c](main.c). In this file we import the header file for the pendulum. 

It allows for MATLAB and if that variable is defined it imports the matrix.h header file. It also allows for ARDUINO (AVR platform). 

Definitions:
- defines state_n = 4 (not sure where this is used, but i'm gonna find out)
- runtimeMS 
- double startState[NUM_DIMS]

Methods
- main
   - reads arguments from the command line, 
      - The first parameter defines runtimeMs(which does?)
      - the rest if the para 
   - This function then uses the function defined in pendulum.c potential to get the LMI potential 
   - It also then runs the isSafe function defined in pendulum.c


## The core of RTREACH is contained in the following files

- [facelift.c](facelift.c)
- [geometry.c](geometry.c)
- [simulate.c](simulate.c)
- [util.c](util.c)

# The header files 

### geometry.h 

This file picks the dynamics header to comiple. 
- If you use the -DLINEAR_PENDULUM flag it selects [dynamics_pendulum.h](dynamics_pendulum.h) file. 
- -DNONLINEAR_PENDULUM gets you [dynamics_pendulum_nonlinear.h](dynamics_pendulum_nonlinear.h). 

Let's consider the non-linear case. 

The [dynamics_pendulum_nonlinear.h](dynamics_pendulum_nonlinear.h) file specifies the number of dimensions. In the case of the inverted pendulum which is an example considered in this repository there are 4 states, therefore 4 dimensions. The real time reachability algorithm uses boxes to represent the states. The technique makes use of mixed face-lifiting and in mixed face lifting the dynamics along each face are over-approximated by the maximum derivative along that face. For a box, there are two faces for every dimension (one for the minimum
face along that dimension and one for the maximum face).


Thus, NUM_FACES = (2* NUM_DIMS)

Definitions in this file:

- **Interval**: C struct with two fields {double min, double max} . 
- **Hyperpoint**: C struct that is a double array of size NUM_DIMS (In the case of the inverted pendulum this a double array of size 4).
- **HyperRectangle**: C struct that is an array of intervals of size NUM_DIMS (For the inverted pendulum it has a shape of (4,2)).

Methods declared in this header file and their explanations (**add them as you figure it out** ): 

- double interval_width(Interval* i);
   - **explanation**:
- bool hyperrectangle_contains(HyperRectangle* outside, HyperRectangle* inside, bool printErrors);
   - **explanation**:
- void hyperrectangle_grow_to_convex_hull(HyperRectangle* grower, HyperRectangle* contained);
   - **explanation**:
- double hyperrectange_max_width(HyperRectangle* rect);
   - **explanation**:
- void hyperrectangle_bloat(HyperRectangle* out, double from[NUM_DIMS], dobule width);
   - **explanation**:

### dynamics_pendulum_nonlinear.h

This file simply defines DYNAMICS_PENDULUM_NONLINEAR and NUM_DIMS.

The DYNAMICS_PENDULUM_NONLINEAR specifies which dynamics file to use. So 
- DYNAMICS_PENDULUM_NONLINEAR corresponds to [dynamics_pendulum_nonlinear.c](dynamics_pendulum_nonlinear.c)
- DYNAMICS_PENDULUM corresponds to [dynamics_pendulum.c](dynamics_pendulum.c)

### dynamics.h

This header file defines one function: 
- double get_derivative_bounds(HyperRectangle* rect, int faceIndex)

### face_lift.h

This file is the header file for the main mixed face-lifting algorithm. 

It defines a struct called LiftingSettings with the following fields:
- init which is a hyper-rectangle
- reachtime which is a double denoting the total reach time
- initialstep size which is the size of the steps to use
- maxRectWidthBeforeError
- maxRuntimeMilliseconds 

The struct also defines pointers to functions:

- bool (*reachedAtIntermediateTime)(HyperRectangle* r);
      - **explanation**:
- bool (*reachedAtFinalTime)(HyperRectangle* r);
      - **explanation**:
- void (*restartedComputation)();
      - **explanation**:

Pointers to functions:
- The idea behind pointers to functions is that it's possible to write a function that takes the address of another function as an argument. This enables the first function to find the second function and run it. Thus enabling the possibility of passing different function addresses to the first function at different times for different purposes.

The function also defines one method that uses the struct defined above:
- bool face_lifting_iterative_improvement(int startMs, LiftingSettings* settings);
   - This function does face lifting with the settings given above. It does not do iterative improvment. It tries to do everything in a single step (which is best for short reach times). It will return true if the set of states is satisfcatory according to the function you provide in LiftingSettings. That function is reachedAtIntermediateTime and this function checks whether the current hyper-rectangle satisifes the constraints.


### pendulum.h

This file has declarations for three methods:
- double potential(double pos, double vel, double theta, double omega);
   - **Explanation**: The inputs to this function are the states of the pendulum and it returns the value of the Lyapunov potential function. This potential function approximates a region known as the recoverable region. The recoverable region is a region of the state space in which a given controller can stabilize the system. from the gain controller computed offline by solving the LMI problem. The solution of this problem yields a gain vector K and a matrix P such that <img src="https://render.githubusercontent.com/render/math?math=X^TPX =1">. As per the following [report](https://apps.dtic.mil/dtic/tr/fulltext/u2/a373286.pdf), this gain value can be use to stabilize the system asymptotically and <img src="https://render.githubusercontent.com/render/math?math=X^TPX =1"> can be used to approximate the ellipsoid of the recoverable region. Thus this function computes the value of this potential function with P defined below: ![P](./images/p.png)
- intermediateState(HyperRectangle *)
   - **explanation**: This function determines if the  HyperRectangle passed to this function satisifes the physical constraints defined in the original problem. If it does it returns true else it returns false.
- int isSafe(int runtimeMs, double state[NUM_DIMS])
   - **Explanation**: Computes the if the current state is safe. The first thing this function does is compute the lyapunov potential as defined above. If the potential is less than equal to 1, then we return 1. If the value of the lmi is greater than one then we simulate it for a maximum of two seconds to see if it goes back into the ellipsod and how far in the future it returns back into the ellipsoid. If the time to safe returned by the simulation is greater than zero it then runs reachability and if that status is safe it returns 3 if not it reutns 2. If the timetosafe is zero, it also returns reachability and if that state is safe it returns 2.
- bool runReachability(double * start, double simTime, double wallTimeMs, double startMs); 
   - **explanation**: This function calls the face_lifting_iterative_improvement(given the startTime, and the settings for the algorithm. It starts with the point specified by start. simTime is synonymous with reachtime. The maximum runtime given for the algorithm is given by wallTimeMs. The initial stepsize is simTime/10 and the maximum widthBefore error is 100. It then defines the functions needed by [face_lift.h](face_lift.h). IntermediateState makes sure that the states satisfy the constraints. FinalState to be completely frank I'm not sure. From an initial pass though it looks like it just enumerates all the possible combinations for HyperRectangles of NUM_DIMS of which there are 2^(NUM_DIMS), so for the pendulum there are 16. Based on these combinations it computes the maximum potential of any of these states and then returns whether or not the max potential of all the states is within the ellipsoid or safe. Take a look here [pendulum.c](pendulum.c)

- getSimulatedSafeTime(REAL start[4])
   - stepSize = 0.02
   - initial stopTime is 0.0 
   - returns the time when the state is safe or the max simulation time. If there is no safe state within the simulation it returns -1

### util.h

Defines two global variables:
- bool intitialized: (which does?)
- long int startSec

Methods: 
- void error_exit(const char* str); 
   - **explanation**
- void set_error_print_params(LiftingSettings* set);
   - **explanation**
- long int milliseconds();
   - **explanation**: computes time passed in milliseconds


### simulate.h

This file define one method called simulate 
- void simulate(REAL point[NUM_DIMS], double stepSize, bool (*shouldStop)(REAL state[NUM_DIMS], double simTime, void* p), void* param);
   - **Explanation**: The header of this function says that it peforms simulation using Euler's method for numerical simulation. For those of us, who don't remember what this means look at the following [article](https://tutorial.math.lamar.edu/classes/de/eulersmethod.aspx). Euler's method is a numerical analysis technique that allows one to approximate solutions to differential equations. It takes the point from which to conduct the simulation and the derivative evaluated at this point to write down the equation of the tangent line. It then uses this to approximate the solution at a given step from the initial point. This process can be repeated to perform a simulation. Pseudocode below: <img src="images/euler.png" alt="euler" width="400"/>. 
   - The derivative computations are defined in terms of intervals in files [dynamics_pendulum.c](dynamics_pendulum.c) and [dynamics_pendulum_nonlinear.c](dynamics_pendulum_nonlinear.c). 
   - It performs simulation for a given simulation time, and stops if this time is over or the state is within the ellipsoid.

- shouldStop(double state[NUM_DIMS], double simTime, void*p)
   - **Explanation**: This function is utilized in simulate. computes the lyapunov function for a given state. This function is used within the simulate function and it is called repetitively during the euler simulation. If rv <1 then it's within the ellipsoid and simulation should stop. The other termination condition is the length of simulation. p looks the pointer that records the time in which the simulation stop time. 

- simulate(double startPoint[NUM_DIMS],double stepSize, bool (*shouldStop)(REAL state[NUM_DIMS], REAL simTime, void* p), void* param)
   - **Explanation**: the simulation uses a hyper-rectangle but with the min and max at the same point so its not really an interval but rather a point. 


### Interval.h

This file contains the functions needed to do interval arithmetic. Here are the operations that are provided in the file. 
I
- Interval new_interval(double min, double max);
   - creates a new interval [min,max]
- Interval new_interval_v(double val);
   - creates a new interval [val,val]
- Interval add_interval(Interval i, Interval j);
   - Adds two intervals [i.min+j.min,i.max+j.max]
- Interval sub_interval(Interval i, Interval j);
   - Subtracts two intervals from each other [i.min-j.min,i.max-j.max]
- Interval mul_interval(Interval i, Interval j);
   - Multiply two intervals together 
- Interval div_interval(Interval i, Interval j);
   - Division of two intervals
- Interval pow_interval(Interval i, int n); 
- Interval sin_interval(Interval i);
- Interval cos_interval(Interval i);

# Dynamics Files 

### dynamics_pendulum_nonlinear.c
![P](./images/non_linear_dynamics.png)

This file defines a function called get_derivative_bounds which computes the derivative in terms of interval arithmetic. 

- get_derivative_bounds(HyperRectange* rect, int faceIndex)
   -  It does so for each dimesnsion at a time. So whatever dimensions is specified by the faceIndex parameter. 
   -  If the index is odd it returns the minimum part of the interval
   -  If the index is even it returns the maximum part
   -  When simulate uses this function it doesn't matter what the faceIndex is because rv.min, rv.max are the same




# Example Execution with the Pendulum Example Code 


One of the examples included in the code starts with the state [-0.1 0.9 0.0 0.0]. The command to run the code with this start state is 
```
./rtreach 100 -0.1 0.9 0.0 0.0
```

This state is unsafe and this can be seen from the value of the lyapunov potential function which is 1.755. From there within the main.c we call the function issafe with the a reachtime of 100ms. Since the lyapunov potential is greater than 1 we use simulation to check whether a future state will re-enter the safe region. This simulation is done via euler simulation and each intermediate state's lyapunov potential is computed via the function shouldStop which is defined in [pendulum.c](pendulum.c). The euler simulation uses a step size of 0.02. Since no succeeding state enters the safe region after 2.00 the algorithm concludes that the system is unsafe. No reachability algorithm is used here, only simulation and successive calls to lyapunov potential function derived by the lmi.

### Example 2, recoverable state (if you use the linear dynamics)
```
./rtreach 100 -0.1 0.6 0.0 0.0
```

The initial state is not within the ellipse since the value of the lyapunov potential function is 1.047692. Since this is the case the next thing that the issafe function is use simulation to see if a future state will return to the ellipse. The simulation uses the feedback controller K which was designed by following the LMI based Simplex Approach. The simulation can execute for a maximum of 2.0s but will stop as soon as a future state is within the ellipsoid. In this case it occurs within 5 euler steps (0.02). This causes the simulation to stop and then we go back to the isSafe function in [pendulum.c](pendulum.c). We can conclude that the current state is safe if we use the safety controller in the future and now we run the runReachability function with the inputs (0.1*1.25) the runtime we specified intially which is 100ms and startms (I have no idea what this is...stay tuned).

runReachability is a functiond defined within [pendulum.c](pendulum.c). That uses the mixed face lifting methods described in the paper to do reachability (fam save me now...). It begins by setting the intial point in the facelifting settings. The first of which is constructing an interval whose min and max are the state from which we are doing reachability. The walltime is what we entered so in our case its 100ms. simTime is (0.1*1.25). It then divides this simtime by 10 and set this is athe initialStepSize with a maxRectWidth of 100. 




### Notes:

- What is the input when the non-linear pendulum model is used? I don't see an input for the linear model its the feedback controller KX with K derived from solving the LMI problem.


















