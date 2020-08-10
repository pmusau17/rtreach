# On Mixed Facelifting 

Thao Dang's Thesis has been massive in helping me decipher how facelifting works. It can be found [here](http://citeseerx.ist.psu.edu/viewdoc/download;jsessionid=37FB8AC6EF9877F0600CCEC69475A688?doi=10.1.1.2.4227&rep=rep1&type=pdf). 

First a quote that helped me get through this: 
- "Young man, in mathematics you don't understand things. You just get used to them."  -John von Neumann

### Let's get into it


#### Functions 

- void make_neighborhood_rect(HyperRectangle * out, int f, HyperRectangle* bloatedRect, HyperRectangle* originalRect, REAL nebWidth)
    - HyperRectangle out: During the facelifting algorithm, implemented here a new HyperRectangle is created each time we do a single facelifitng operation. This HyperRectangle is the Rectangle that we edit
    - HyperRectangle bloatedRect: bloated rectangle is a copy of the original HyperRectangle passed into the facelifting operation.
    - HyperRectangle originalRect: **(What's this?)**
    - double nebWidth: initially this value is zero during the operation. (K cool what??).
    - The first thing this function does is assign the pointer to the out to the bloatedRect pointer
    - If the face index is odd its a minimum (and i'm not so sure I understand entirely what that means)
    - Anyway if it's a min whatever dimension (f corresponds to) it sets the min and max of that dimension to min of the original rect
    - Otherwise it sets that dimension both to max
    - if neb is negtative the min dimension increases by nebWidth, else the max dimension increases by nebWidth


- void generateSplitRectangle(HyperRectangle* rectToSplit, HyperRectangle* out, int iteratorVal, int splitDimensions[NUM_DIMS])
    - HyperRectangle rectToSplit
    - HyperRectangle out 
    - int iteratorVal
    - int[] splitDimesnions: Initial thought on this is that its a mask 

- face_lifting_iterative_improvement(int startMs, LiftingSettings* settings)
    - This function is the one that is used within the pendulum.c
    - This function will quit if the stepSize gets below 0.00000001
    - There is a restartedComputation option in the settings but for the pendulum it doesn't exist. The settings for the pendulum don't include a restart option so we can ignore that. 
    - It then gets the reachTime fro the reachTime. Let us remember that reachTime is defined as the time we are computing the reachability for. In the pendulum example this parameter is computed as 1.25 times the timeToSafe. It then gets divided by 10. So really its (1.25*timeToSafe). The initial stepSize is 0.1*reachTime.


- lift_single_rect(HyperRectangle * rect, REAL stepSize, timeRemaining)
    - HyperRectangle * rect: Initial HyperRectangle passed to the face lifting algorithm, stepSize is initially listed above, timeRemaining is the reachtime. Initially timeRemaining and reachtime are assigned the same value. 
    - This function returns timeElapsed which is a double
    - we start with the rectangle we passed in and define a double array nebWidth[NUM_FACES] (Remembers NUM_FACES = 2* NUM_DIMS)
    - Initially we estimate the nebWidth based on the derivative in the center of the rectangle that we care about. 
    - Initially we se it all to zero



### Alright I'm going to try and make this as concrete as possible

I'm going to use the call ./rtreach 100 -0.1 0.7 0.0 0.0. 

This state a recoverable state slightly outside of the ellipse. Through euler simualation we determine that the after 0.100 seconds the state will return to the ellipse. Thus we also perform reachability analysis from this state. So the starting hyperRectangle is the following.
[-0.100000,-0.100000]
[0.700000,0.700000]
[0.000000,0.000000]
[0.000000,0.000000]

Since the state was simulated safe after 0.100 seconds, we now run the reachability analysis for 1.25*0.10 so for 0.125 with a stepSize of 0.012500.



Reminder: 

- The get_derivative_bounds(&faceNebRect, f) function is an implementation of the derivatives of the model one dimension at a time. It allows for the evaluation of the derivative with respect to the faces of an orthogonal polyhedron. Depending on which face number it is. It will either give you the minimum or the maximum of the interval. So if its even it returns the min if its odd it returns the maximum. The justification for why this works like that is on page 94 of Thao Dang's Thesis.



