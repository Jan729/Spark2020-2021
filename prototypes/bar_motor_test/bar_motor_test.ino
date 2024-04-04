#include <AccelStepper.h>

#define CEILING 776000           //steps to reach top of playing area
#define FLOOR 17800             //revs to reach bottom of the playing area
#define BALL_RETURN_HEIGHT 0 //lowest height of bar, where bar will pick up ball
#define MAX_BAR_TILT 7600      // maximum slope of bar, aka barPosLeft - barPosRight
#define MAX_STEPS_PER_SEC 4000
#define INITIAL_STEPS_PER_SEC 2000
#define MAX_STEPS_PER_S_SQUARED 3000
#define STEPS_PER_CALL 500 // 1000 steps = 1cm in height
#define STEPS_PER_REV 200
#define LOOK_AHEAD_STEPS 100
#define BAR_DOWN_DELAY_S 5

#define STEP_L 3
#define DIR_L 4
#define STEP_R 5
#define DIR_R 6

#define JOYSTICK_L_UP 7
#define JOYSTICK_L_DOWN 8
#define JOYSTICK_R_UP 9
#define JOYSTICK_R_DOWN 10

int leftBarInput;
int rightBarInput;
int barTilt = 0;
unsigned long lastBarTime;
AccelStepper motorR = AccelStepper(AccelStepper::DRIVER, STEP_R, DIR_R);
AccelStepper motorL = AccelStepper(AccelStepper::DRIVER, STEP_L, DIR_L);

void setup()
{

  long startPos = CEILING/2;
  motorR.setCurrentPosition(startPos);
  motorL.setCurrentPosition(startPos);
  
  motorR.setMaxSpeed(MAX_STEPS_PER_SEC);
  motorL.setMaxSpeed(MAX_STEPS_PER_SEC);

  motorR.setSpeed(INITIAL_STEPS_PER_SEC);
  motorL.setSpeed(INITIAL_STEPS_PER_SEC);

  motorR.setAcceleration(MAX_STEPS_PER_S_SQUARED);
  motorL.setAcceleration(MAX_STEPS_PER_S_SQUARED);

  pinMode(JOYSTICK_R_DOWN, INPUT_PULLUP);
  pinMode(JOYSTICK_R_UP, INPUT_PULLUP);
  pinMode(JOYSTICK_L_DOWN, INPUT_PULLUP);
  pinMode(JOYSTICK_L_UP, INPUT_PULLUP);
}

void loop()
{   
   moveBar();
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

    if (playerIsIdle()) {
      //  moveBarDown();
    }
}

bool playerIsIdle(){
  unsigned long currentTime = millis();
  unsigned long passingTime = currentTime  - lastBarTime;
  return passingTime/1000 > BAR_DOWN_DELAY_S;
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