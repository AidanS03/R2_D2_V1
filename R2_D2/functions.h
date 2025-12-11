#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define trig_R 22
#define echo_R 23
#define trig_L 12
#define echo_L 13

#define pGain 2
#define iGain 0.25
#define dGain 0.2

#define STEERING_GAIN 1 

float getDist(char side);
void processPID(float PIDres);

#endif
