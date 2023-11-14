#include <Arduino.h>
#include "functions.h"
#include "global-variables.h"

void pollBarJoysticks() {// all buttons are active low
  if (digitalRead(JOYSTICK_R_UP) == LOW) {
    rightBarInput = 1;
  } else if (digitalRead(JOYSTICK_R_DOWN) == LOW) {
    rightBarInput = -1;
  } else {
    rightBarInput = 0;
  }

  if (digitalRead(JOYSTICK_L_UP) == LOW) {
    leftBarInput = 1;
  } else if (digitalRead(JOYSTICK_L_DOWN) == LOW) {
    leftBarInput = -1;
  } else {
    leftBarInput = 0;
  }
}

void moveBar()
{
  bool moveRightUp = rightBarInput == 1;
  bool moveRightDown = rightBarInput == -1;
  bool moveLeftUp = leftBarInput == 1;
  bool moveLeftDown = leftBarInput == -1;

  if (moveRightUp && barPosR > CEILING && barTilt < MAX_BAR_TILT)
  { //move right side of bar UP
    motorR.step(STEP_INCR);
    barPosR-= 1;
  } else if (moveRightDown && barPosR < FLOOR && barTilt > -MAX_BAR_TILT)
  { //move right side of bar DOWN
    motorR.step(-STEP_INCR);
    barPosR-= 1;
  }

  if (moveLeftUp && barPosL > CEILING && barTilt > -MAX_BAR_TILT)
  { //move left side of bar UP
    motorL.step(STEP_INCR);
    barPosL-= 1;
  } else if (moveLeftDown && barPosL < FLOOR && barTilt < MAX_BAR_TILT)
  { //move left side of bar DOWN
    motorL.step(-STEP_INCR);
    barPosL-= 1;
  }

  barTilt = barPosL - barPosR;
}

//lowers bar to ball return area, calls resetBall(), then lifts bar to start of game area
//assumes there won't be a ball on the bar when resetBar() is called
void resetBarAndBall()
{
  #ifdef IS_WOKWI_TEST
    Serial.println("dummy reset bar function. sleeping 1s...");
    delay(1000);
    barPosL = FLOOR;
    barPosR = FLOOR;
    Serial.println("finished resetting bar");
    return;
  #endif

  //if left side of bar is higher, lower it to the same height as right side
  while(barPosL < barPosR) {
    motorL.step(STEP_INCR);
    barPosL+= 1;
  }

   //if right side of bar is higher, lower it to the same height as left side
  while(barPosR < barPosL) {
    motorR.step(STEP_INCR);
    barPosR+= 1;
  }

  //lower both sides of bar to ball return height
  
  while (barPosL < BALL_RETURN_HEIGHT)
  {
  //when bar triggers the bar sensor, reset the bar's position variables  
  if(digitalRead(BAR_SENSOR_PIN) == LOW) {
     barPosL = FLOOR;
     barPosR = FLOOR;
  }

   motorL.step(STEP_INCR);
   motorR.step(STEP_INCR); 
   barPosL+= 1;
   barPosR+= 1;
   Serial.println("left bar pos: " + barPosL);
  }

  resetBall();

  //lift bar to start of playing area
  while (barPosL > FLOOR)
  {
    motorL.step(-STEP_INCR);
    motorR.step(-STEP_INCR);
    barPosL-= 1;
    barPosR-= 1;
  }
}

void resetBall() { // TODO check +ve and -ve direction
    motorBallReturn.step(STEPS_30_DEG);
    delay(BALL_RETURN_DELAY_MS);
    motorBallReturn.step(-STEPS_30_DEG);
  
  Serial.println("reset ball");
}

void moveBarDown() { // TODO: should make the bar go down by a decided amount
  // if it's alr at the bottom, don't move it
}
