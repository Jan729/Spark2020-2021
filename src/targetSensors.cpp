#include <Arduino.h>
#include "functions.h"
#include "global-variables.h"

void pollIRSensors() {

  #if IS_WOKWI_TEST
    bool wonLevel = digitalRead(TEST_WIN_LEVEL_BUTTON) == LOW;
    bool ballAtBottom = digitalRead(TEST_LOSE_GAME_BUTTON) == LOW;
    
    // catch rising edge of test buttons
    if (wonLevel && !wonLevelState) {
      wonLevelState = true;
    } else if (!wonLevel && wonLevelState) {
      wonLevelState = false;
    }
    
    if (ballAtBottom && !ballAtBottomState) {
      ballAtBottomState = true;
    } else if (!ballAtBottom && ballAtBottom) {
      ballAtBottomState = false;
    }
    
    targetBroken = wonLevelState;
    bottomBroken = ballAtBottomState;
  #else
    targetBroken = false; // TODO replace with Ginny's IR sensor polling logic
    bottomBroken = false; // poll sensor at bottom of the game
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
