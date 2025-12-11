#include "functions.h"

float getDist(char side){
  float duration, distance;
  
  if (side == 'R'){
    digitalWrite(trig_R, LOW);
    delayMicroseconds(2);
    digitalWrite(trig_R, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig_R, LOW);
  
    duration = pulseIn(echo_R, HIGH);
    distance = (duration*.0343)/2;
    
    return distance;
  }else if(side == 'L'){
    digitalWrite(trig_L, LOW);
    delayMicroseconds(2);
    digitalWrite(trig_L, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig_L, LOW);
  
    duration = pulseIn(echo_L, HIGH);
    distance = (duration*.0343)/2;
    
    return distance;
  }
}

void processPID(float PIDres){
  float forwardSpeed = 100;
  float turn = PIDres * STEERING_GAIN;

  float leftSpeed = forwardSpeed - turn;
  float rightSpeed = forwardSpeed + turn;

  leftSpeed = constrain(leftSpeed, -255, 255);
  rightSpeed = constrain(rightSpeed, -255, 255);

  int leftPWM = abs(leftSpeed);
  int rightPWM = abs(rightSpeed);
  
  // Set left motor direction
  if (leftSpeed <= 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }

  // Set right motor direction
  if (rightSpeed <= 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }

  // Set speeds
  analogWrite(ENABLE_A, leftPWM);
  analogWrite(ENABLE_B, rightPWM);

  Serial.printf("Left speed: %f Right speed: %f\n", leftSpeed, rightSpeed);
}
