#include <Arduino.h>
#include "functions.h"
#include "global-variables.h"

void updateLights(int lastHole, int newHole){
  digitalWrite(lastHole, LOW);
  digitalWrite(newHole, HIGH);
}

void flashAllTargetLEDs() {
    for(int i = 15; i <=0 ; i--) {
     mcp3.digitalWrite(i, HIGH);
     delay(100);
     mcp3.digitalWrite(i, LOW);
    }
  
  for(int i = 15; i <=0 ; i--) {
     mcp2.digitalWrite(i, HIGH);
     delay(100);
     mcp2.digitalWrite(i, LOW);
    }
  
  for(int i = 15; i <=0 ; i--) {
     mcp1.digitalWrite(i, HIGH);
     delay(100);
     mcp1.digitalWrite(i, LOW);
    }
}
