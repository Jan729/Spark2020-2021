#include <Arduino.h>
#include "functions.h"
#include "global-variables.h"
#include <math.h>

void updateMux() {
  int parentChannel = floor((level-1)/8);
  int childChannel = (level-1)%8;

  digitalWrite(PARENT_A, bitRead(parentChannel, 0));
  digitalWrite(PARENT_B, bitRead(parentChannel, 1));
  digitalWrite(PARENT_C, bitRead(parentChannel, 2));

  digitalWrite(CHILD_S1, bitRead(childChannel, 0));
  digitalWrite(CHILD_S2, bitRead(childChannel, 1));
  digitalWrite(CHILD_S3, bitRead(childChannel, 2));

}

void pollMux() {

  #if IS_WOKWI_TEST
    bool wonLevelButton = digitalRead(TEST_WIN_LEVEL_BUTTON) == LOW;
    bool loseButton = digitalRead(TEST_LOSE_GAME_BUTTON) == LOW;

    static bool prevWonLevelState = false;
    static bool prevBallAtBottomState = false;

    if (wonLevelButton && !prevWonLevelState) {
      wonLevel = true;
      prevWonLevelState = true;
    } else if (!wonLevelButton) {
      prevWonLevelState = false;
    }

    if (loseButton && !prevBallAtBottomState) {
      ballFellIntoBackboard = true;
      prevBallAtBottomState = true;
    } else if (!loseButton) {
      prevBallAtBottomState = false;
    }
  #else
    int sensorValue = analogRead(MUX_OUT_ANALOG);
    if (sensorValue > 500) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(10);
      digitalWrite(LED_BUILTIN, LOW);
      delay(10);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(10);
      digitalWrite(LED_BUILTIN, LOW);
      delay(10);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(10);
      digitalWrite(LED_BUILTIN, LOW);
      wonLevel = true;
      ballFellIntoBackboard = false; // poll sensor at bottom of the game
    }

    // TODO: Add logic for checking if ball fell into backboard
  #endif
}

bool beamBroken() // FIXME this code is outdated. replace with ginny's mux circuit reading code
{

  // bool beamBroke = false;

  // variables will change: // 1 = unbroked, 0 = broke 
  // int sensorState;
    
  // if(targetSensorPin < 16)
  // {
  //   sensorState = mcp1.digitalRead(targetSensorPin%16);
  // }
  
  // else if(16 <= targetSensorPin < 32)
  // {
  //   sensorState = mcp2.digitalRead(targetSensorPin%16);
  // }
  
  // else
  // {
  //   sensorState = mcp3.digitalRead(targetSensorPin%16);
  // }
  

  //note that the array index may need to be changed
  //depending on the actual value of the sensorpin
  //this implementation assumes all IRs are using contiguous
  //pins starting at 0
  //so will probably need an offset (if pins start at 4 for example)
  
  // targetIRBuffer.addValue(!beamBroken*5);
  //fiddle with the number 5 s.t. when beam is broken
  //it affects the average more

  //maybe change value from 1 to 0 or something else
  // if (targetIRBuffer.getAverage() > 1) {
  //   return 1;
  // }
  return 0; //!beamBroken; //this way 1 = broken and 0 = unbroken
}
