#include <Arduino.h>
#include "functions.h"
#include "global-variables.h"

void pollIRSensors() {

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
    wonLevel = false; // TODO replace with Ginny's IR sensor polling logic
    ballFellIntoBackboard = false; // poll sensor at bottom of the game
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
