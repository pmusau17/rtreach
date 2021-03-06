% makefile for real time reachability for Matlab

% change which dynamics are used by changing flag -D
mex -v -O main.c face_lift.c geometry.c interval.c ...
    dynamics_converging_oscillator.c ...
    dynamics_harmonic_oscillator.c ...
    dynamics_pendulum.c ... % linear pendulum
    dynamics_pendulum_nonlinear.c ... % nonlinear pendulum
    pendulum.c ... 
    dynamics_vanderpol.c ... % van der pol oscillator
    simulate.c util.c ...
    -DLINEAR_PENDULUM ...
    -output rtreach
    
    % use this for nonlinear pendulum
    %-DNONLINEAR_PENDULUM ...
    % use this for linear pendulum
    %-DLINEAR_PENDULUM ...