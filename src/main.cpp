#include "global-variables.h"
#include "functions.h"
#include <Stepper.h>
#include "RunningAverage.h"
#include "SevSeg.h"
#include "Wire.h"
#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include <avr/interrupt.h>

/************LOCAL VARIABLE DECLARATIONS **********************/
// To make a variable global, add it to
// global-variables.h with an extern definition

// spark PCB
// todo: change these to the correct chip: SN74HC595N
Adafruit_MCP23X17 mcp1; //shift registers; 8 of them, each with 16 pins
Adafruit_MCP23X17 mcp2;
Adafruit_MCP23X17 mcp3;
Adafruit_MCP23X17 mcp4;
Adafruit_MCP23X17 mcp5;
Adafruit_MCP23X17 mcp6;
Adafruit_MCP23X17 mcp7;
Adafruit_MCP23X17 mcp8;

RunningAverage targetIRBuffer(NUM_IR_SAMPLES); // stores IR sensor data
int targetHoles[NUMTARGETS]; // stores pin numbers of target LEDs to light up

bool winGame; 
bool loseGame;
int score;
int targetDifficulty;
int level;
int targetLEDPin;
int targetSensorPin;
bool targetBroken;
bool bottomBroken;

// hex display variables
int highscore;

//global variables for timing 
unsigned long finishTime;  //time when the ball drops into target hole, resets each round
unsigned long lastIdleTime;
volatile long debounce_time;
volatile long current_button_time;

//global vars for bar movement
int leftBarInput; // -1 = down, 0 = stationary, 1 = up
int rightBarInput;
int barPosL;
int barPosR;
int barTilt;
Stepper motorR = Stepper(STEPS_PER_REV, MOTOR_R_STEP, MOTOR_R_DIR);
Stepper motorL = Stepper(STEPS_PER_REV, MOTOR_L_STEP, MOTOR_L_DIR);
int stepsPerRevolution;
Stepper motorBallReturn = Stepper(STEPS_PER_REV, RESET_MOTOR_STEP, RESET_MOTOR_DIR); 

SevSeg sevseg1;
SevSeg sevseg2;
SevSeg sevseg3;

// main loop control variables
volatile bool playingGame = false; //true if someone is playing, false if game over
bool wonLevelState = false;
bool ballAtBottomState = false;

/************END OF LOCAL DECLARATIONS**********************/

/****** GAME CONTROL FUNCTIONS ****/

void resetAllVariables() {
  playingGame = true; //true if someone is playing, false if game over
  winGame = false; 
  loseGame = false;
  score = 0;
  targetDifficulty = 0;
  level = 1;

  // control next level and game over conditions
  targetLEDPin = 0;
  targetSensorPin = 0;
  targetBroken = false;
  bottomBroken = false;
  wonLevelState = false;
  ballAtBottomState = false;

  // hex display variables
  highscore = 0;

  //global vars for bar movement
  leftBarInput = 0;
  rightBarInput = 0;
  barPosL = FLOOR;
  barPosR = FLOOR;
  barTilt = 0;
  stepsPerRevolution = 800;

  //global variables for timing 
  finishTime = 0;  //time when the ball drops into target hole, resets each round
  lastIdleTime = millis();
  debounce_time = 0;
  current_button_time = 0;
  
}

void waitToStartGame() { 
    // TODO: add turning off joysticks, LED, IR transceiver
    while(!playingGame) {
      // do nothing until interrupt changes the state
    }

}

void resetGame(){
  // TODO calibrate bar if powered off display and bar is in an uknown position
  resetAllVariables();
  resetBarAndBall();

  displayScore();
  digitalWrite(targetLEDPin, HIGH);
}

void checkIdleTime(){
  unsigned long currentIdleTime = millis();
  unsigned long idleTime = currentIdleTime  - lastIdleTime;
  if (idleTime/60000 > 5) // 5 minutes of idle
    playingGame = false;
}

//ISR for start/stop game button only to change gameState
void buttonPressed() {
  if (playingGame == true) {
    Serial.println("Button to end game is pressed");
    playingGame = false;
    delayMicroseconds(100000); //non-blocking delay to debounce button
  }
  else if(playingGame == false) {
    Serial.println("Button to start game is pressed");
    playingGame = true;
    delayMicroseconds(100000); //non-blocking delay to debounce button
  }
}

/****** END OF GAME CONTROL FUNCTIONS ****/

/**********END OF HELPER FUNCTION IMPLEMENTATIONS******/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // Starts the serial communication

  #if IS_WOKWI_TEST

    pinMode(WIN_LEVEL_BUTTON, INPUT_PULLUP);
    pinMode(LOSE_GAME_BUTTON, INPUT_PULLUP);
  #endif

  // TODO initialize spark pcb
  // TODO init IR receivers

  // mcp1.begin_I2C(0);
  // mcp2.begin_I2C(1);
  // mcp3.begin_I2C(2);
  // mcp4.begin_I2C(3);
  // mcp5.begin_I2C(4);
  // mcp6.begin_I2C(5);
  // mcp7.begin_I2C(6);
  // mcp8.begin_I2C(7);
  
  // for (int i=0; i<16; i++) {
  //   mcp1.pinMode(i, INPUT_PULLUP);
  //   mcp2.pinMode(i, INPUT_PULLUP);
  //   mcp3.pinMode(i, INPUT_PULLUP);
  //   mcp4.pinMode(i, INPUT_PULLUP);
  //   mcp5.pinMode(i, INPUT_PULLUP);
  //   mcp6.pinMode(i, INPUT_PULLUP);
  //   mcp7.pinMode(i, INPUT_PULLUP);
  //   mcp8.pinMode(i, INPUT_PULLUP);
  // }
  

  //button interrupt
  pinMode(STARTBUTTONPIN, INPUT_PULLUP);  //Start button, LOW when pressed
  attachInterrupt(digitalPinToInterrupt(STARTBUTTONPIN), buttonPressed, FALLING);

  // Game Input Buttons to Move Bar 
  pinMode(JOYSTICK_R_UP, INPUT_PULLUP);
  pinMode(JOYSTICK_R_DOWN, INPUT_PULLUP);
  pinMode(JOYSTICK_L_UP, INPUT_PULLUP);
  pinMode(JOYSTICK_L_DOWN, INPUT_PULLUP);

  pinMode(11, OUTPUT); //IR LEDs. keep on at all times
  pinMode(12, OUTPUT);

  //set up seven seg display x3
  // byte numDigits = 1;
  // byte digitPins[] = {};
  // byte segmentPins1[] = {6, 5, 2, 3, 4, 7, 8, 9};
  // byte segmentPins2[] = {6, 5, 2, 3, 4, 7, 8, 9};
  // byte segmentPins3[] = {6, 5, 2, 3, 4, 7, 8, 9};
  // bool resistorsOnSegments = true;

  // byte hardwareConfig = COMMON_ANODE; 
  // sevseg1.begin(hardwareConfig, numDigits, digitPins, segmentPins1, resistorsOnSegments);
  // sevseg1.setBrightness(90);
  // sevseg2.begin(hardwareConfig, numDigits, digitPins, segmentPins2, resistorsOnSegments);
  // sevseg2.setBrightness(90);
  // sevseg3.begin(hardwareConfig, numDigits, digitPins, segmentPins3, resistorsOnSegments);
  // sevseg3.setBrightness(90);
  
  // sevseg1.setNumber(0);
  // sevseg1.refreshDisplay(); 
  // sevseg2.setNumber(0);
  // sevseg2.refreshDisplay(); 
  // sevseg3.setNumber(0);
  // sevseg3.refreshDisplay(); 

  //set up two gpio expanders
  //chip 1 address: 0x20
  //chip 2 address: 0x27
//  Wire.begin(); //wake up I2C bus
  // set I/O pins for chip 0x20 to outputs
//  Wire.beginTransmission(0x20);
//  Wire.write(0x00); // IODIRA register
//  Wire.write(0x00); // set all of port A to outputs
//  Wire.endTransmission();
//  Wire.beginTransmission(0x20);
//  Wire.write(0x01); // IODIRB register
//  Wire.write(0x00); // set all of port B to outputs
//  Wire.endTransmission();

//  // set I/O pins for chip 0x27 to outputs
//  Wire.beginTransmission(0x27);
//  Wire.write(0x00); // IODIRA register
//  Wire.write(0x00); // set all of port A to outputs
//  Wire.endTransmission();
//  Wire.beginTransmission(0x27);
//  Wire.write(0x01); // IODIRB register
//  Wire.write(0x00); // set all of port B to outputs
//  Wire.endTransmission();
  

  //reset stepper motors
  barPosL = FLOOR;
  barPosR = FLOOR;
  motorBallReturn.setSpeed(MAX_SPEED); //setSpeed only take a positive number
  motorR.setSpeed(MAX_SPEED);
  motorL.setSpeed(MAX_SPEED);
}

void loop() {

  waitToStartGame(); 
  resetGame();
   
  while (playingGame) {
    //TODO: Work out where to call start and end times (NOT DONE)
    // finishTime = millis(); //might not need here depending on when incrementLevel is called. 
    
    pollBarJoysticks(); 
    moveBar();
    pollIRSensors(); // newTarget, winGame, loseGame is decided here. 
    checkIdleTime();

  }

  //if idle
  if (!targetBroken && !bottomBroken){
    // force ball to any hole?
    // or don't reset ball next time?
  } 
  // else wait for someone to start game

  // if (winGame) {
  //   Serial.println("You win");
  //   digitalWrite(targetLEDPin, LOW); 
  //   // flashAllTargetLEDs(); //TODO: change to new code with LED array
  //   // displayWinMessage();
  // } else if (loseGame){
  //   Serial.println("You lose");
  //   digitalWrite(targetLEDPin, LOW); 
  //   // displayLoseMessage();
  // }else {
  //   // flashAllTargetLEDs();
  //   // displayLoseMessage();
  // }

  // displayScore(); //replace win/lose message with player score
}
