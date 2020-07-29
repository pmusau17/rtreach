These are my notes as I seek to understand how this tool even works. At the end of this I hope this is a useful guide for anyone who wishes to use this tool in real-time reachability. 

# On x86 systems this is what gets executed when you run make 

```
$ gcc -std=gnu99 -O3 - Wall facelift.c geometry.c interval.c simulate.c util.c \ 
   dynamics_converging_oscillator.c dynamics_harmonic_oscillator.c dynamics_vanderpol.c \
   dyanmics_pendulum.c dynamics_pendulum_nonlinear.c pendulum.c -lm -o rtreach -DNONLINEAR_PENDULUM
```

# The core of RTREACH are the following files

- [facelift.c](facelift.c)
- [geometry.c](geometry.c)
- [simulate.c](simulate.c)
- [util.c](util.c)



