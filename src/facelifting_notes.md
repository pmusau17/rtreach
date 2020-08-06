# On Mixed Facelifting 

Thao Dang's Thesis has been massive in helping me decipher how facelifting works. It can be found [here](http://citeseerx.ist.psu.edu/viewdoc/download;jsessionid=37FB8AC6EF9877F0600CCEC69475A688?doi=10.1.1.2.4227&rep=rep1&type=pdf). 

### Let's get into it


#### Functions 

- void make_neighborhood_rect(HyperRectangle * out, int f, HyperRectangle* bloatedRect, HyperRectangle* originalRect, REAL nebWidth)
    - HyperRectangle out: **(What's this?)**
    - HyperRectangle bloatedRect: **(What's this?)**
    - HyperRectangle originalRect: **(What's this?)**
    - double nebWidth: **(What's this?)**
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
    - It then gets the reachTime fro the reachTime. Let us remember that reachTime is defined as the time we are computing the reachability for. In the pendulum example this parameter is computed as 1.25 times the timeToSafe. It then gets divided by 10. So really its (0.10*1.25*timeToSafe). 


- lift_single_rect(HyperRectangle * rect, REAL stepSize, timeRemaining)
    - we start with the rectangle we passed in and define a double array nebWidth[NUM_FACES] (Remembers NUM_FACES = 2* NUM_DIMS)
    - Initially we estimate the nebWidth based on the derivative in the center of the rectangle that we care about. 
    - Initially we se it all to zero

