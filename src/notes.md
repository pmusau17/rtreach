# On rtreach

These are my notes as I seek to understand how this tool works. At the end of this, I hope this is a useful guide for anyone who wishes to use this tool for real-time reachability. 



# Definitions/Reference

- **REAL**: double

# On x86 systems this is what gets executed when you run make 

```
gcc -std=gnu99 -O3 - Wall facelift.c geometry.c interval.c simulate.c util.c \ 
   dynamics_converging_oscillator.c dynamics_harmonic_oscillator.c dynamics_vanderpol.c \
   dyanmics_pendulum.c dynamics_pendulum_nonlinear.c pendulum.c -lm -o rtreach -DNONLINEAR_PENDULUM
```

# The core of RTREACH are the following files

- [facelift.c](facelift.c)
- [geometry.c](geometry.c)
- [simulate.c](simulate.c)
- [util.c](util.c)

# The header files 

### geometry.h 

This file picks the dynamics header to comiple. If you use the -DLINEAR_PENDULUM flag it selects [dynamics_pendulum.h](dynamics_pendulum.h) file. -DNONLINEAR_PENDULUM gets you [dynamics_pendulum_nonlinear.h](dynamics_pendulum_nonlinear.h). Let's consider the non-linear case. 

The [dynamics_pendulum_nonlinear.h](dynamics_pendulum_nonlinear.h) file specifies the number of dimensions. In the case of the inverted pendulum which is an example considered in this repository there are 4 states, therefore 4 dimensions. The real time reachability algorithm uses boxes to represent the states. The technique makes use of mixed face-lifiting and in mixed face lifting they dynamics along each face are over-approximated by the maximum derivative along that face. 

Thus is the geometry file you will find a definition that specifies that the number of faces is two times the number of dimensions. 

### dynamics_pendulum_nonlinear.h











