/***************************

  Moves the stepper motors for the bar, given a value from user input (ie distance sensors)
  
  IMPORTANT: If you don't have a driver connected to your arduino and directly plugged the stepper into
            your arduino instead, uncomment the code labeled "without driver" and 
            comment out the code labeled "with driver"

  
  Assumptions:
  - distance sensor code will give the motors an int within the range [-3, 3]
  - +ve speed means "up", -ve speed means "down", 0 means stationary, and |3| is the fastest speed
  - when u power on the arduino, the bar always starts at the height needed to load a ball
  - spinning motor CCW moves bar UP
  - the top of the display is at stepper position 0
  - stepper position increases as bar goes down

 coordinate system:

        (top of display)
 [barPosL = 0         barPosR = 0]
  |                     |
  |                     |
  ...    (gameboard)   ...
  |                     |
  |                     |
[barPosL = 10000       barPosR = 10000]
   (bottom of playing area aka FLOOR)
  |                     |
  |                     |
 [barPosL = 11000       barPosR = 11000]
 (height for ball return area aka BALL_RETURN_HEIGHT)

  References:

  AccelStepper Library Documentation
  https://www.airspayce.com/mikem/arduino/AccelStepper/classAccelStepper.html

 DRV8825 Stepper Driver Tutorial
  - includes tips about setting up hardware as well as step-by-step code
  https://www.makerguides.com/drv8825-stepper-motor-driver-arduino-tutorial/

  Tutorial for smaller 5V 28byj48 stepper motors
  https://lastminuteengineers.com/28byj48-stepper-motor-arduino-tutorial/
  Warning: If using ULN2003 motor drivers, you must intentionally swap pins 2 and 3 for the reverse direction to work

 ***************************/

#include <Stepper.h>
//#include <MultiStepper.h>

// Feb 5, 2023: measurements from real Remy's backboard
// 30.5" from ball return to last hole = 775mm
// 7" = 178mm from ball return to start of playing area. not sure where visible area is
// 2mm/rev
// 178mm / 2 = 89 revs from ball return to start
// 388 revs from ball return to last hole
// Guess: 76mm = 38 rev max height difference between L and R ends of bar

#define CEILING 0                //highest height of bar
#define FLOOR 299             //bottom of the playing area, not actually the floor
#define BALL_RETURN_HEIGHT 388 //lowest height of bar, where bar will pick up ball
#define MAX_BAR_TILT 38         //maximum vertical slope of bar, aka barPosRight - barPosLeft
#define MAX_SPEED 300
#define STEP_INCR 2000 //about 1cm in height. steps taken on each loop() iteration
#define STEPS_PER_REV 200 //DRV driver, 1.8 deg step angle
#define SPEED_MULT 5           //multiply user input value with this number to set desired stepper speed
#define BALL_RETURN_DELAY 2000 //time in ms to wait until a new ball has rolled onto bar
//distance sensors code will give the motors a number within the range [-3, 3]
//-ve speed means "down", +ve speed means "up"
#define STOP 0
#define SLOW 1
#define MED 2
#define FAST 3

int userInputLeft; //from distance sensor functions
int userInputRight;
int prevSpeedLeft;
int prevSpeedRight;
int barPosL = FLOOR;
int barPosR = FLOOR;
int barTilt = 0;

int speedBoost = 1; //increment this number to increase the bar speed

// To setup DRV drivers
// DIR pin is in the opposite corner and on the same long edge as the pot
// STP is right next to DIR
// pull RST and SLP to 5V to activate driver
// note int stepsFor30Degrees=67; //800*(30/360) rounded as int

  #define DIR_R 2
  #define STEP_R 3
  #define DIR_L 4
  #define STEP_L 5

  Stepper motorR = Stepper(STEPS_PER_REV, STEP_R, DIR_R);
  Stepper motorL = Stepper(STEPS_PER_REV, STEP_L, DIR_L);

/***************************************/

////To make both motors arrive at the same position at the same time? need to research
//MultiStepper bothSteppers;

//FOR PROTOTYPING ONLY. pushbuttons and LED pins
//these should actually be global variables from other parts of code
#define L_UP 6 //pushbuttons
#define L_DOWN 7
#define R_UP 8
#define R_DOWN 9
#define SWAP 10

#define L_FLOOR A5 //LEDs
#define L_CEIL A4
#define R_FLOOR A3
#define R_CEIL A2
#define MAX_TILT_REACHED A1

bool swapControls = false;

int userSpeed = 1;
//end of prototyping variables

void setup()
{

  barPosL = FLOOR;
  barPosR = FLOOR;
  
  //Note: speed is in rpm for stepper.h
  motorR.setSpeed(MAX_SPEED);
  motorL.setSpeed(MAX_SPEED);


  //LEDS FOR PROTOTYPING ONLY
  pinMode(R_DOWN, INPUT_PULLUP);
  pinMode(R_UP, INPUT_PULLUP);
  pinMode(L_DOWN, INPUT_PULLUP);
  pinMode(L_UP, INPUT_PULLUP);
  pinMode(L_FLOOR, OUTPUT);
  pinMode(R_FLOOR, OUTPUT);
  pinMode(L_CEIL, OUTPUT);
  pinMode(R_CEIL, OUTPUT);
  pinMode(MAX_TILT_REACHED, OUTPUT);
   Serial.begin(9600);

for (int i = 0; i < 50; i++) { // nov 6: negative is DOWN
motorL.step(-STEP_INCR);
motorR.step(-STEP_INCR);
}
  //resetBar();
}

void loop()
{
  //PROTYPING ONLY - get user input from pushbuttons
  // if (digitalRead(R_UP) == LOW)
  // {
  //   userInputRight = 1;
  // }

  // // TODO replace all variables with joystick values
  // // digitalRead(JoystickDownPin) == LOW)
  // else if (digitalRead(R_DOWN  == LOW))
  // {
  //   userInputRight = -1;
    
  // }
  // else
  // {
  //   userInputRight = 0;
  // }

  // if (digitalRead(L_UP) == LOW)
  // {
  //   userInputLeft = 1;
  // }
  // else if (digitalRead(L_DOWN) == LOW)
  // {
  //   userInputLeft = -1;
  // }
  // else
  // {
  //   userInputLeft = 0;
  // }

    
   moveBar();

}

void setBarSpeed() {
    if(userInputLeft != prevSpeedLeft) {
      motorL.setSpeed(userSpeed * SPEED_MULT + speedBoost);
    }
    if(userInputRight != prevSpeedRight) {
      motorR.setSpeed(userSpeed * SPEED_MULT + speedBoost);
    }
  prevSpeedLeft = userInputLeft;
  prevSpeedRight = userInputRight;
}

void moveBar()
{
    if (userInputRight > 0 && barPosR > CEILING && barTilt < MAX_BAR_TILT)
    { //move right side of bar UP
      motorR.step(-STEP_INCR);
      barPosR-= 1;
    }
    else if (userInputRight < 0 && barPosR < FLOOR && barTilt > -MAX_BAR_TILT)
    { //move right side of bar DOWN
      motorR.step(STEP_INCR);
      barPosR+= 1;
    }

    if (userInputLeft > 0 && barPosL > CEILING && barTilt > -MAX_BAR_TILT)
    { //move left side of bar UP
      motorL.step(-STEP_INCR);
      barPosL-= 1;
    }
    else if (userInputLeft < 0 && barPosL < FLOOR && barTilt < MAX_BAR_TILT)
    { //move left side of bar DOWN
      motorL.step(STEP_INCR);
      barPosL+= 1;
    }

  barTilt = barPosL - barPosR;

  //turn on LEDs for debugging
  if (abs(barTilt) == MAX_BAR_TILT)
  {
    digitalWrite(MAX_TILT_REACHED, HIGH);
  }
  else
  {
    digitalWrite(MAX_TILT_REACHED, LOW);
  }

  switch (barPosL)
  {
  case FLOOR:
    digitalWrite(L_FLOOR, HIGH);
    break;
  case CEILING:
    digitalWrite(L_CEIL, HIGH);
    break;
  default:
    digitalWrite(L_FLOOR, LOW);
    digitalWrite(L_CEIL, LOW);
  }

 // LEDs for debugging
  switch (barPosR)
  {
  case FLOOR:
    digitalWrite(R_FLOOR, HIGH);
    break;
  case CEILING:
    digitalWrite(R_CEIL, HIGH);
    break;
  default:
    digitalWrite(R_FLOOR, LOW);
    digitalWrite(R_CEIL, LOW);
  }
}

void resetBar()
{
  motorR.setSpeed(MAX_SPEED);
  motorL.setSpeed(MAX_SPEED);

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

//TODO: when bar sensor triggered, reset barPos to FLOOR
// if we notice the software motor position values drift over time, add a sensor to re-calibrate
// in variable declarations, add this line: #define BAR_SENSOR_PIN (insert pin number here)
//   if(digitalRead(BAR_SENSOR_PIN) == HIGH) {
//      barPosL = FLOOR;
//      barPosR = FLOOR;
//   }
   motorL.step(STEP_INCR);
   motorR.step(STEP_INCR); 
   barPosL+= 1;
   barPosR+= 1;
  }

  //resetBall(); //matt has the code for this function

  //lift bar to start of playing area
  while (barPosL > FLOOR)
  {
    motorL.step(-STEP_INCR);
    motorR.step(-STEP_INCR);
    barPosL-= 1;
    barPosR-= 1;
  }
}
