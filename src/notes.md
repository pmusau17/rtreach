# On Rtreach

These are my notes as I seek to understand how this tool works. At the end of this, I hope this is a useful guide for anyone who wishes to use this tool for real-time reachability. 



## Definitions/Reference

- **REAL**: double

### On x86 systems this is what gets executed when you run make 

```
gcc -std=gnu99 -O3 - Wall facelift.c geometry.c interval.c simulate.c util.c \ 
   dynamics_converging_oscillator.c dynamics_harmonic_oscillator.c dynamics_vanderpol.c \
   dyanmics_pendulum.c dynamics_pendulum_nonlinear.c pendulum.c -lm -o rtreach -DNONLINEAR_PENDULUM
```

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

The [dynamics_pendulum_nonlinear.h](dynamics_pendulum_nonlinear.h) file specifies the number of dimensions. In the case of the inverted pendulum which is an example considered in this repository there are 4 states, therefore 4 dimensions. The real time reachability algorithm uses boxes to represent the states. The technique makes use of mixed face-lifiting and in mixed face lifting they dynamics along each face are over-approximated by the maximum derivative along that face. 

Thus is the geometry file you will find a definition that specifies that the number of faces is two times the number of dimensions. Thus the file defines 

NUM_FACES = (2* NUM_DIMS)

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

The DYNAMICS_PENDULUM_NONLINEAR does...(add this).

### dynamics.h

This header file defines one function: 
- double get_derivative_bounds(HyperRectangle* rect, int faceIndex)













