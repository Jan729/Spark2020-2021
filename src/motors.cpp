#include <Arduino.h>
#include "functions.h"
#include "global-variables.h"
#include <AccelStepper.h>

  // FIXME calibrate bar if powered off display and bar is in an unknown position
  // rn the bar position is hardcoded during code upload
void calibrateBarPosition() {
  long hardcodedStartPos = CEILING/2;
  motorR.setCurrentPosition(hardcodedStartPos);
  motorL.setCurrentPosition(hardcodedStartPos);
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
   long barPosR = motorR.currentPosition();
   long barPosL = motorL.currentPosition();
   barTilt = barPosL - barPosR;

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
   long barPosR = motorR.currentPosition();
   long barPosL = motorL.currentPosition();

    barTilt = barPosL - barPosR;

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

    if (checkPassingTime()) {
      moveBarDown();
    }
}

void resetBarAndBall()
{
  // TODO hardcode the bar to dump any leftover ball into backboard

  motorL.moveTo(BALL_RETURN_HEIGHT);
  motorR.moveTo(BALL_RETURN_HEIGHT);

  resetBall();

  motorL.moveTo(FLOOR);
  motorR.moveTo(FLOOR);
}

void resetBall() { 
  // IMPLEMENT ME
  Serial.println("reset ball");
}

void moveBarDown() {
  long barPosR = motorR.currentPosition();
  long barPosL = motorL.currentPosition(); 
  lastBarTime = millis();
  if (barPosR-STEPS_PER_CALL > FLOOR) {
      motorR.moveTo(barPosR-STEPS_PER_CALL);
  }

  if (barPosL-STEPS_PER_CALL > FLOOR) {
       motorL.moveTo(barPosL-STEPS_PER_CALL);
  }
}
