#include "functions.h"

void multiLED(int LED1, int LED2, int LED3){
  int count = 0;

  if(LED3 < 0) count++;
  if(LED2 < 0) count++;

  if(count == 0){
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
  }else if(count == 1){
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
  }else{
    digitalWrite(LED1, HIGH);
  }
}