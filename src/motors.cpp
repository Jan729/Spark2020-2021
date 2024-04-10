#include <Arduino.h>
#include "functions.h"
#include "global-variables.h"
#include <AccelStepper.h>
  
  // TODO check if the bar position variables "drift" and become 
  // inaccurate over time
void calibrateBarPosition() {
  // FIXME hardcode the bar's position before you run it the first time
  // then test the EEPROM code
  // you can't test the EEPROM code with the simulator
  long hardcodedStartPosR = FLOOR;
  long hardcodedStartPosL = FLOOR;
  barTilt = hardcodedStartPosL - hardcodedStartPosR;
  motorR.setCurrentPosition(hardcodedStartPosR);
  motorL.setCurrentPosition(hardcodedStartPosL);

  // setBarPositionsFromEEPROM();
  Serial.print(barPosL);
  lastBarSaveTime = millis();
}

void setupBarMotors() {
  motorR.setMaxSpeed(MAX_STEPS_PER_SEC);
  motorL.setMaxSpeed(MAX_STEPS_PER_SEC);

  motorR.setSpeed(INITIAL_STEPS_PER_SEC);
  motorL.setSpeed(INITIAL_STEPS_PER_SEC);

  motorR.setAcceleration(MAX_STEPS_PER_S_SQUARED);
  motorL.setAcceleration(MAX_STEPS_PER_S_SQUARED);
}

void pollLeftJoystick() {
  if (digitalRead(JOYSTICK_L_UP) == LOW) {
    leftBarInput = 1;
    lastBarTime = millis();
  } else if (digitalRead(JOYSTICK_L_DOWN) == LOW) {
    leftBarInput = -1;
    lastBarTime = millis();
  } else {
    leftBarInput = 0;
    motorL.setSpeed(0);
  }
}

void pollRightJoystick() {
  if (digitalRead(JOYSTICK_R_UP) == LOW) {
    rightBarInput = 1;
    lastBarTime = millis();
  } else if (digitalRead(JOYSTICK_R_DOWN) == LOW) {
    rightBarInput = -1;
    lastBarTime = millis();
  } else {
    rightBarInput = 0;
    motorR.setSpeed(0);
  }
}

void moveBarLeft() {
    if (leftBarInput > 0 && barPosL+STEPS_PER_CALL < CEILING && barTilt < MAX_BAR_TILT)
    { //move left side of bar UP
        bool changingDirections = motorL.distanceToGo() < 0;
        long positionCorrection = changingDirections ? LOOK_AHEAD_STEPS : 0;
        motorL.moveTo(barPosL+STEPS_PER_CALL+positionCorrection);
    }
    else if (leftBarInput < 0 && barPosL-STEPS_PER_CALL > FLOOR && barTilt > -MAX_BAR_TILT)
    { //move left side of bar DOWN
        bool changingDirections = motorL.distanceToGo() > 0;
        long positionCorrection = changingDirections ? LOOK_AHEAD_STEPS : 0;
        motorL.moveTo(barPosL-STEPS_PER_CALL-positionCorrection);
    }
}

void moveBarRight() {
    if (rightBarInput > 0 && barPosR+STEPS_PER_CALL < CEILING && barTilt > -MAX_BAR_TILT)
    { //move right side of bar UP
      bool changingDirections = motorR.distanceToGo() < 0;
      long positionCorrection = changingDirections ? LOOK_AHEAD_STEPS : 0;
      motorR.moveTo(barPosR+STEPS_PER_CALL+positionCorrection);
    }
    else if (rightBarInput < 0 && barPosR-STEPS_PER_CALL > FLOOR && barTilt < MAX_BAR_TILT)
    { //move right side of bar DOWN
      bool changingDirections = motorR.distanceToGo() > 0;
      long positionCorrection = changingDirections ? LOOK_AHEAD_STEPS : 0;
      motorR.moveTo(barPosR-STEPS_PER_CALL-positionCorrection);
    }
}

void moveBar()
{
    barPosR = motorR.currentPosition();
    barPosL = motorL.currentPosition();
    barTilt = barPosL - barPosR;
    bool rightBarMoving = motorR.distanceToGo() > LOOK_AHEAD_STEPS || motorR.distanceToGo() < -LOOK_AHEAD_STEPS;
    bool leftBarMoving = motorL.distanceToGo() > LOOK_AHEAD_STEPS  || motorL.distanceToGo() < -LOOK_AHEAD_STEPS;

    if (rightBarMoving) {
        motorR.run();
    } else {
        pollRightJoystick();
        moveBarRight();
    }

    if (leftBarMoving) {
        motorL.run();
    } else {
        pollLeftJoystick();
        moveBarLeft();
    }

    if (playerIsIdle()) {
      moveBarDown();
    }

}

void moveBarBlocking(long leftPos, long rightPos) {
  motorL.moveTo(leftPos);
  motorR.moveTo(rightPos);

  while(motorR.run() || motorL.run()) {
    motorR.run();
    motorL.run();
  }
  // WARNING If the power disconnects in the middle of the while loop, the position
  // stored in EEPROM will be wrong
}

void resetBarAndBall()
{
  #if IS_WOKWI_TEST
  Serial.println("resetting bar and ball...");
  #endif

  // TODO hardcode the bar to dump any leftover ball into backboard

  moveBarBlocking(BALL_RETURN_HEIGHT, BALL_RETURN_HEIGHT);

  resetBall();

  moveBarBlocking(FLOOR, FLOOR);

  #if IS_WOKWI_TEST
  Serial.println("done resetting bar and ball");
  #endif
}

void resetBall() { 
  // IMPLEMENT ME
  #if IS_WOKWI_TEST
  Serial.println("loading ball...");
  #endif
}

void moveBarDown() {
  barPosR = motorR.currentPosition();
  barPosL = motorL.currentPosition(); 
  lastBarTime = millis();
  if (barPosR-STEPS_PER_CALL > FLOOR) {
    barPosR = barPosR-STEPS_PER_CALL;
  }

  if (barPosL-STEPS_PER_CALL > FLOOR) {
    barPosL = barPosL-STEPS_PER_CALL;
  }

  moveBarBlocking(barPosL, barPosR);
}
