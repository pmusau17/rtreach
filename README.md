# F1Tenth Rtreach

### compile the code like this: 

```
gcc -std=gnu99 -O3 -Wall  face_lift_bicycle_model.c geometry.c interval.c simulate_bicycle.c util.c  dynamics_bicycle_model.c  bicycle_main.c bicycle_model.c  -lm -o bicycle -DBICYCLE_MODEL_NONLINEAR
```

### Run a two second simulation using the following command

```
./bicycle 100 0.0 0.0 0.0 0.0 16.0 0.2666
```


### I need to refactor this code...

