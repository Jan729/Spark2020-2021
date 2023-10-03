
/* 
  IR Breakbeam sensor demo!
*/

/**********CONSTANTS*****************************/
#define SENSORPIN 0
#define BOTTOMPIN 13
#define SCOREINCREASE 10
#define NUMTARGETS 30
#define IROFFSET 25
#define STARTBUTTONPIN 2 //TODO: add circuit usually ON button (HIGH)
#include <Stepper.h>
#include "RunningAverage.h"
#include "SevSeg.h"
#include "Wire.h"
#include <Wire.h>
#include "Adafruit_MCP23017.h"
#include <avr/interrupt.h>

//these can be changed and we need 
//two for each IR sensor

/************START OF CONSTANTS*********************/
int IRSensor = 2; // connect ir sensor to arduino pin 2
int LED = 13; // connect Led to arduino pin 13

#define BAR_SENSOR_PIN 12
#define MOTOR_R_STEP 14
#define MOTOR_R_DIR  15
#define MOTOR_L_STEP 16
#define MOTOR_L_DIR  17
#define RESET_MOTOR_STEP 0
#define RESET_MOTOR_DIR 1
#define STEPS_PER_REV 800 //DRV driver
#define CEILING 0                //highest height of bar
#define FLOOR 299             //bottom of the playing area, not actually the floor
#define BALL_RETURN_HEIGHT 388 //lowest height of bar, where bar will pick up ball
#define MAX_BAR_TILT 38         //maximum vertical slope of bar, aka barPosRight - barPosLeft
#define MAX_SPEED 200 //in rpm
#define STEP_INCR 800 //steps taken on each loop() iteration
#define BALL_RETURN_DELAY 2000 //time to wait until a new ball has rolled onto bar

//gpio pins. MUST USE SPECIAL PINS 20 and 21
#define SERIAL_DATA_LINE 20
#define SERIAL_CLOCK_LINE 21

/*
Pin count
MOTORS – 6 digital driver pins (step and dir x3 drivers, right bar, left bar, ball return)
IR SENSORS – 1 analog, 6 digital (for muxes)
LED + SHIFT REGISTOR – 3 digital
Reset/Start button - 1 digital interrupt pin
Left joystick - 2 digital pins (up and down)
Right joystick - 2 digital pins (up and down)
Hex displays () - 4 digital pins total, 2 per 4 digits, CLK and DIO
(reference: https://www.instructables.com/How-to-Use-the-TM1637-Digit-Display-With-Arduino/)

*/

/************END OF CONSTANTS*********************/

/************GLOBAL VARIABLES**********************/
// spark PCB
Adafruit_MCP23017 mcp1; //shift registers; 8 of them, each with 16 pins
Adafruit_MCP23017 mcp2;
Adafruit_MCP23017 mcp3;
Adafruit_MCP23017 mcp4;
Adafruit_MCP23017 mcp5;
Adafruit_MCP23017 mcp6;
Adafruit_MCP23017 mcp7;
Adafruit_MCP23017 mcp8;

RunningAverage holes[(int)NUMTARGETS]; // stores IR sensor data
int targetHoles[NUMTARGETS]; // stores pin numbers of target LEDs to light up

bool playingGame = true; //true if someone is playing, false if game over
bool winGame = false; 
bool loseGame = false;
int score = 0;
int targetDifficulty = 0;
int highscore = 0;
int level = 0;
int targetLEDPin = 0;
int targetSensorPin = 0;
bool targetBroken = false;
bool bottomBroken = false;

//global vars for pins for input buttons 
int left_button = 7;
int right_button = 8;

//global variables for timing 
unsigned long finishTime;  //time when the ball drops into target hole, resets each round
unsigned long idleTime;
volatile long debounce_time = 0;
volatile long current_button_time = 0

//global vars for bar movement
int moveLeftBarUp = 0;
int moveRightBarUp = 0;
int barPosL = FLOOR;
int barPosR = FLOOR;
int barTilt = 0;
Stepper motorR = Stepper(STEPS_PER_REV, MOTOR_R_STEP, MOTOR_R_DIR);
Stepper motorL = Stepper(STEPS_PER_REV, MOTOR_L_STEP, MOTOR_L_DIR);

SevSeg sevseg1;
SevSeg sevseg2;
SevSeg sevseg3;

/************END OF GLOBAL VARIABLES**********************/

/**********HELPER FUNCTION PROTOTYPES******************/
void waitToStartGame();
void updateTarget();
void resetBall();
void resetGame();
void checkIdleTime();
bool beamBroken(int target);
void ballEntry();
void moveBar();
void resetBar();
void updateLights(int lastHole, int newHole);
void updateScore();
void displayScore();
void sethighScore();
void flashAllTargetLEDs();
void displayWinMessage();
void displayLoseMessage();
/******END OF HELPER FUNCTION PROTOTYPES***************/

/**********START OF HELPER FUNCTION IMPLEMENTATIONS*******************/

void waitToStartGame() {
  //wait and do nothing until someone presses "start"
  //then continue with game loop

  //player places hand over one of the sensors to start
  //has to be high above sensor
  if (start_game_input() == 1) {
    //start the game
    playingGame = true;
    resetGame();
    
  }
  //otherwise do nothing
  //Serial.println("wait to start the game. pretend someone starts playing game");

}

void updateTarget() {
  level++; 
  randomSeed(analogRead(0));

  int targetIncr = (int)random(1,3);

  int oldTarget = targetLEDPin;
  
  targetLEDPin += targetIncr;
  targetSensorPin += targetIncr;

  if(targetLEDPin > NUMTARGETS){
    playingGame = false;
    winGame = true; //win the game
  }

  updateLights(oldTarget, targetLEDPin);
}

void resetBall() {
  //wait until ball is ready to roll onto the bar
  //put the ball back onto the bar
    myStepper.step(stepsFor30Degrees); //rotate 30 degrees
    delay(1000);
    myStepper.step(stepsFor30Degrees*-1); //rotate in opposite direction
  
  Serial.println("reset ball");
}

void resetGame(){
    level = 0;
    score = 0;
    targetLEDPin = 0;
    targetDifficulty = 0;
    targetBroken = false;
    bottomBroken = false;
    resetBar();
    resetBall();
    idleTime = millis();
    displayScore();
    digitalWrite(targetLEDPin, HIGH);
}

void checkIdleTime(){
  if (idleTime/60000 > 5) // 5 minutes of idle
    playingGame = false;
}

/****** USER INPUT FUNCTIONS ****/
//ISR for start button
void buttonPressed() {
  if(playingGame == true){
    current_button_time = millis();
    if (current_button_time - debounce_time > 200) {
      Serial.println("start button pressed");
      playingGame = !playingGame; // change state of game
      resetGame(); //TODO: make sure it's functioning well to reset starting parameters
    }
    if (playingGame == false) {
      waitToStartGame();
    }
  }
  debounce_time = current_button_time; //to make sure we're not reading the button press multiple times
}

void get_left_user_input() {
	moveLeftBarUp = !digitalRead(left_button); // active low
}
void get_right_user_input() {
	moveRightBarUp = !digitalRead(right_button); // active low
}

/****** END OF USER INPUT FUNCTIONS ****/

/************ BALL DETECTION FUNCTIONS *********/

bool beamBroken(int target)
{

  bool beamBroke = false;
  //#define SENSORPIN 4

  // variables will change: // 1 = unbroked, 0 = broke 
  int sensorState;
  
  //sensorState = digitalRead(SENSORPIN);
  
  if(SENSORPIN < 16)
  {
    sensorState = mcp1.digitalRead(targetSensorPin%16)
  }
  
  else if(16 <= SENSORPIN < 32)
  {
    sensorState = mcp2.digitalRead(targetSensorPin%16)
  }
  
  else
  {
    sensorState = mcp3.digitalRead(targetSensorPin%16)
  }
  

  //note that the array index may need to be changed
  //depending on the actual value of the sensorpin
  //this implementation assumes all IRs are using contiguous
  //pins starting at 0
  //so will probably need an offset (if pins start at 4 for example)
  
  holes[SENSORPIN].addValue(!beamBroken*5);
  //fiddle with the number 5 s.t. when beam is broken
  //it affects the average more

  //maybe change value from 1 to 0 or something else
  if (holes[SENSORPIN].getAverage() > 1) {
    return 1;
  }
  return !beamBroken; //this way 1 = broken and 0 = unbroken
}


void ballEntry() {

  targetBroken = beamBroken(targetLEDPin);
  bottomBroken=beamBroken(BOTTOMPIN);

  if (targetBroken) { //ball fell in good hole
    targetBroken = false;
    resetBar();
    resetBall();

    updateTarget();
    finishTime = millis();
    updateScore();
    displayScore();
    
  } else if (bottomBroken) { //ball fell in bad hole  
    playingGame = false;
    loseGame = true;
    resetGame();

  }

}

/************ END OF BALL DETECTION FUNCTIONS ***********/

/*********** START OF BAR MOVEMENT FUNCTIONS *********/

void moveBar()
{
  // TODO: the bar will only move up for now. add down controls if the game is too hard
    if (moveRightBarUp && barPosR > CEILING && barTilt < MAX_BAR_TILT)
    { //move right side of bar UP
      motorR.step(STEP_INCR);
      barPosR-= 1;
    }

    if (moveLeftBarUp && barPosL > CEILING && barTilt > -MAX_BAR_TILT)
    { //move left side of bar UP
      motorL.step(STEP_INCR);
      barPosL-= 1;
    }

  barTilt = barPosL - barPosR;
}

//lowers bar to ball return area, calls resetBall(), then lifts bar to start of game area
//assumes there won't be a ball on the bar when resetBar() is called
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
  //when bar triggers the bar sensor, reset the bar's position variables  
  if(digitalRead(BAR_SENSOR_PIN) == LOW) {
     barPosL = FLOOR;
     barPosR = FLOOR;
  }

   motorL.step(STEP_INCR);
   motorR.step(STEP_INCR); 
   barPosL+= 1;
   barPosR+= 1;
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

/********** END OF BAR MOVEMENT FUNCTIONS ************/

/************ START OF OUTPUT FUNCTIONS ***********/
void updateLights(int lastHole, int newHole){
  digitalWrite(lastHole, LOW)
  digitalWrite(newHole, HIGH)
}

//we can put this array somewhere else
//for the seven seg display
//we should probably wait until we know which display
//we're using to code this
int num_array[10][7] = {  { 1,1,1,1,1,1,0 },    // 0
                          { 0,1,1,0,0,0,0 },    // 1
                          { 1,1,0,1,1,0,1 },    // 2
                          { 1,1,1,1,0,0,1 },    // 3
                          { 0,1,1,0,0,1,1 },    // 4
                          { 1,0,1,1,0,1,1 },    // 5
                          { 1,0,1,1,1,1,1 },    // 6
                          { 1,1,1,0,0,0,0 },    // 7
                          { 1,1,1,1,1,1,1 },    // 8
                          { 1,1,1,0,0,1,1 }};   // 9

int gpio_num_array[10] = {  B01111110,    // 0
                            B00110000,    // 1
                            B01101101,    // 2
                            B01111001,    // 3
                            B00110011,    // 4
                            B1011011,    // 5
                            B01011111,    // 6
                            B01110000,    // 7
                            B01111111,    // 8
                            B01110011 };   // 9
                         
void updateScore() {
  targetDifficulty += 1;
  score += (30000 - finishTime)/1500 * targetDifficulty/5 ; //TODO: finish this code
  
  displayScore();
  sethighScore();
}

void displayScore() {
  int firstDigit = score/100;
  int secondDigit = (score-firstDigit)/10;
  int thirdDigit = (score-firstDigit-secondDigit);

  sevseg1.setNumber(firstDigit);
  sevseg1.refreshDisplay(); 
  sevseg2.setNumber(secondDigit);
  sevseg2.refreshDisplay(); 
  sevseg3.setNumber(thirdDigit);
  sevseg3.refreshDisplay(); 
  
  
  //update digits on the seven seg display 


  Wire.beginTransmission(0x20); //select chip 0x20
  Wire.write(0x12); // Select Port A
  Wire.write(gpio_num_array[thirdDigit]); // Send data
  Wire.endTransmission();
  Wire.beginTransmission(0x20); //select chip 0x20
  Wire.write(0x13); // Select Port B
  Wire.write(gpio_num_array[highFirst]); // Send data
  Wire.endTransmission();
  Wire.beginTransmission(0x27); //select chip 0x27
  Wire.write(0x12); // Select Port A
  Wire.write(gpio_num_array[highSecond]); // Send data
  Wire.endTransmission();
  Wire.beginTransmission(0x27); //select chip 0x27
  Wire.write(0x13); // Select Port B
  Wire.write(gpio_num_array[highThrid]); // Send data
  Wire.endTransmission();

}

void sethighScore() {
  if (score > highscore) {
    highscore = score;
    int highFirst = high/100;
    int highSecond = (highscore-highFirst)/10;
    int highThird = (highscore-highFirst-highSecond);
  }
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

void displayWinMessage() {
  for (int i=1; i<=3; i++) {
    sevseg1.setChars("y");
    sevseg1.refreshDisplay(); 
    sevseg2.setChars("o");
    sevseg2.refreshDisplay(); 
    sevseg3.setNumber("u");
    sevseg3.refreshDisplay(); 
    delay(500);
    sevseg1.setChars("w");
    sevseg1.refreshDisplay(); 
    sevseg2.setChars("i");
    sevseg2.refreshDisplay(); 
    sevseg3.setNumber("n");
    sevseg3.refreshDisplay(); 
    delay(500);
  }
}

void displayLoseMessage() {
  for (int i=1; i<=3; i++) {
    sevseg1.setChars("y");
    sevseg1.refreshDisplay(); 
    sevseg2.setChars("o");
    sevseg2.refreshDisplay(); 
    sevseg3.setNumber("u");
    sevseg3.refreshDisplay(); 
    delay(500);
    sevseg1.setChars("l");
    sevseg1.refreshDisplay(); 
    sevseg2.setChars("o");
    sevseg2.refreshDisplay(); 
    sevseg3.setNumber("-");
    sevseg3.refreshDisplay(); 
    delay(500);
    sevseg1.setNumber("e");
    sevseg1.refreshDisplay(); 
    sevseg2.setChars("!");
    sevseg2.refreshDisplay(); 
    sevseg3.setNumber("!");
    sevseg3.refreshDisplay(); 
    delay(500);
  }
}

void displayLoseMessage() {
  for (int i=1; i<=3; i++) {
    sevseg1.setChars("o");
    sevseg1.refreshDisplay(); 
    sevseg2.setChars("h");
    sevseg2.refreshDisplay(); 
    sevseg3.blank();
    sevseg3.refreshDisplay(); 
    delay(500);
    sevseg1.setChars("n");
    sevseg1.refreshDisplay(); 
    sevseg2.setChars("o");
    sevseg2.refreshDisplay(); 
    delay(500);
    sevseg1.setChars("s");
    sevseg1.refreshDisplay(); 
    sevseg2.setChars("o");
    sevseg2.refreshDisplay(); 
    delay(500);
    sevseg1.setChars("s");
    sevseg1.refreshDisplay(); 
    sevseg2.setChars("a");
    sevseg2.refreshDisplay(); 
    sevseg3.setNumber("d");
    sevseg3.refreshDisplay(); 
    delay(500);
  }
}

/************ END OF OUTPUT FUNCTIONS ***********/

/**********END OF HELPER FUNCTION IMPLEMENTATIONS******/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // Starts the serial communication

  mcp1.begin(0);
  mcp2.begin(1);
  mcp3.begin(2);
  mcp4.begin(3);
  mcp5.begin(4);
  mcp6.begin(5);
  mcp7.begin(6);
  mcp8.begin(7);
  
  for (int i=0; i<16; i++) {
    mcp1.pinMode(i, INPUT);
    mcp1.pullUp(i, HIGH);
    mcp2.pinMode(i, INPUT);
    mcp2.pullUp(i, HIGH);
    mcp3.pinMode(i, INPUT);
    mcp3.pullUp(i, HIGH);
    mcp4.pinMode(i, INPUT);
    mcp4.pullUp(i, HIGH);
    mcp5.pinMode(i, INPUT);
    mcp5.pullUp(i, HIGH);
    mcp6.pinMode(i, INPUT);
    mcp6.pullUp(i, HIGH);
    mcp7.pinMode(i, INPUT);
    mcp7.pullUp(i, HIGH);
    mcp8.pinMode(i, INPUT);
    mcp8.pullUp(i, HIGH);
  }
  
  for (int i = IROFFSET; i < NUMTARGETS+IROFFSET; i++) {
    pinMode(i, INPUT); //IR receivers, one pin per target hole
    digitalWrite(i, HIGH); // turn on the pullup
    targetHoles[i] = i;
  }

  //button interrupt
  pinMode(STARTBUTTONPIN, INPUT_PULLUP)  //Start button, LOW when pressed
  attachInterrupt(digitalPinToInterrupt(STARTBUTTONPIN), buttonPressed, FALLING);

  // Game Input Buttons to Move Bar 
  pinMode(left_button, INPUT);
  pinMode(right_button, INPUT);

  pinMode(11, OUTPUT); //IR LEDs. keep on at all times
  pinMode(12, OUTPUT);
  pinMode(BOTTOMPIN, INPUT); //IR receivers for all bad holes

  //fill targetHoles array with pin numbers

  //set up seven seg display x3
  byte numDigits = 1;
  byte digitPins[] = {};
  byte segmentPins1[] = {6, 5, 2, 3, 4, 7, 8, 9};
  byte segmentPins2[] = {6, 5, 2, 3, 4, 7, 8, 9};
  byte segmentPins3[] = {6, 5, 2, 3, 4, 7, 8, 9};
  bool resistorsOnSegments = true;

  byte hardwareConfig = COMMON_ANODE; 
  sevseg1.begin(hardwareConfig, numDigits, digitPins, segmentPins1, resistorsOnSegments);
  sevseg1.setBrightness(90);
  sevseg2.begin(hardwareConfig, numDigits, digitPins, segmentPins2, resistorsOnSegments);
  sevseg2.setBrightness(90);
  sevseg3.begin(hardwareConfig, numDigits, digitPins, segmentPins3, resistorsOnSegments);
  sevseg3.setBrightness(90);
  
  sevseg1.setNumber(0);
  sevseg1.refreshDisplay(); 
  sevseg2.setNumber(0);
  sevseg2.refreshDisplay(); 
  sevseg3.setNumber(0);
  sevseg3.refreshDisplay(); 

  //reset bar stepper motors
  barPosL = FLOOR;
  barPosR = FLOOR;
  motorR.setSpeed(MAX_SPEED);
  motorL.setSpeed(MAX_SPEED);

  //set up two gpio expanders
  //chip 1 address: 0x20
  //chip 2 address: 0x27
 Wire.begin(); //wake up I2C bus
  // set I/O pins for chip 0x20 to outputs
 Wire.beginTransmission(0x20);
 Wire.write(0x00); // IODIRA register
 Wire.write(0x00); // set all of port A to outputs
 Wire.endTransmission();
 Wire.beginTransmission(0x20);
 Wire.write(0x01); // IODIRB register
 Wire.write(0x00); // set all of port B to outputs
 Wire.endTransmission();

 // set I/O pins for chip 0x27 to outputs
 Wire.beginTransmission(0x27);
 Wire.write(0x00); // IODIRA register
 Wire.write(0x00); // set all of port A to outputs
 Wire.endTransmission();
 Wire.beginTransmission(0x27);
 Wire.write(0x01); // IODIRB register
 Wire.write(0x00); // set all of port B to outputs
 Wire.endTransmission();
  
// ball return stepper motor
const int speed=100; //arbitrary, to be adjusted with testing
const int stepsPerRevolution=800; //not used but for reference
int stepsFor30Degrees=67; //800*(30/360) rounded as int
Stepper myStepper(stepsPerRevolution,RESET_MOTOR_STEP,RESET_MOTOR_DIR); //need to make sure pins are consistent with pin declarations in main***
myStepper.setSpeed(speed); //setSpeed only take a positive number

motorR.setSpeed(MAX_SPEED)
motorL.setSpeed(MAX_SPEED)
resetBar();
}

void loop() {

  waitToStartGame(); //wait for user to start game and set PlayingGame = true

  while (playingGame) {
    //TODO: Work out where to call start and end times (NOT DONE)
    finishTime = millis(); //might not need here depending on when updateTarget is called. 
  
    get_left_user_input(); 
    get_right_user_input();
    moveBar();
    ballEntry();
    checkIdleTime();

  }

  if (winGame) {
    digitalWrite(targetLEDPin, LOW); 
    AllTargetLEDs();
    displayWinMessage();
  } else if (loseGame){
    digitalWrite(targetLEDPin, LOW); 

  }else {
    flashAllTargetLEDs();
    displayLoseMessage();
  }

  displayScore(); //replace win/lose message with player score
}
