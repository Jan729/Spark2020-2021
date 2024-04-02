#include "global-variables.h"
#include "functions.h"
#include "RunningAverage.h"
#include "SevSeg.h"
#include "Wire.h"
#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include <avr/interrupt.h>
#include <AccelStepper.h>

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
int targetDifficulty;
int level;
int targetLEDPin;
int targetSensorPin;
bool targetBroken;
bool bottomBroken;

// hex display variables
int bonusScore;
int curScore;
int highScore;

//global variables for timing 
unsigned long finishTime;  //time when the ball drops into target hole, resets each round
volatile long debounce_time;
volatile long current_button_time;

//global vars for bar movement
int leftBarInput;
int rightBarInput;
int barTilt;
unsigned long lastBarTime;
AccelStepper motorR = AccelStepper(AccelStepper::DRIVER, STEP_R, DIR_R);
AccelStepper motorL = AccelStepper(AccelStepper::DRIVER, STEP_L, DIR_L);

// TODO Stepper motorBallReturn = Stepper(STEPS_PER_REV, RESET_MOTOR_STEP, RESET_MOTOR_DIR); 

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
  winGame = false; 
  loseGame = false;
  bonusScore = 0;
  curScore = 0;
  targetDifficulty = 0; //TODO: what's the difference between targetDifficulty and level?
  level = 1;

  // control next level and game over conditions
  targetLEDPin = 0;
  targetSensorPin = 0;
  targetBroken = false;
  bottomBroken = false;
  wonLevelState = false;
  ballAtBottomState = false;

  //global vars for bar movement
  leftBarInput = 0;
  rightBarInput = 0;
  barTilt = 0;

  //global variables for timing 
  finishTime = 0;  //time when the ball drops into target hole, resets each round
  lastBarTime = millis();
  debounce_time = 0;
  current_button_time = 0;
  
}

void waitToStartGame() { 
    Serial.println("waiting for someone to start game");
    while(!playingGame) {
      // do nothing until interrupt changes the state
    }
}

void resetGame(){
  // TODO calibrate bar if powered off display and bar is in an uknown position
  resetAllVariables();
  resetBarAndBall();

  resetScores();
  digitalWrite(targetLEDPin, HIGH);
}

bool checkPassingTime(){
  unsigned long currentTime = millis();
  unsigned long passingTime = currentTime  - lastBarTime;
  return passingTime/1000 > BAR_DOWN_DELAY_S;
}

//ISR for start/stop game button only to change gameState
void startButtonPressed() {
  static unsigned long lastDebounceTime = 0;
  bool debounceElapsed = millis() - lastDebounceTime >= DEBOUNCE_DELAY_MS;
  
  if (!debounceElapsed) {
    return;
  }
  
  lastDebounceTime = millis();

  #if IS_WOKWI_TEST
  if (playingGame) {
    Serial.println("Button to end game is pressed");
  } else {
    Serial.println("Button to start game is pressed");
  }
  #endif
  
  playingGame = !playingGame;
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
  
  //button interrupt
  pinMode(STARTBUTTONPIN, INPUT_PULLUP);  //Start button, LOW when pressed
  attachInterrupt(digitalPinToInterrupt(STARTBUTTONPIN), startButtonPressed, FALLING);

  // Game Input Buttons to Move Bar 
  pinMode(JOYSTICK_R_UP, INPUT_PULLUP);
  pinMode(JOYSTICK_R_DOWN, INPUT_PULLUP);
  pinMode(JOYSTICK_L_UP, INPUT_PULLUP);
  pinMode(JOYSTICK_L_DOWN, INPUT_PULLUP);

  pinMode(11, OUTPUT); //IR LEDs. keep on at all times
  pinMode(12, OUTPUT);

  // set up hex displays
  setHexDisplayBrightness();
}

void loop() {

  waitToStartGame(); 
  resetGame();
  Serial.println("Game starts!");
  while (playingGame) {
    //TODO: Work out where to call start and end times (NOT DONE)
    // finishTime = millis(); //might not need here depending on when incrementLevel is called. 
    
    moveBar();

    pollIRSensors(); // newTarget, winGame, loseGame is decided here. score is updated here too
    
    if (checkPassingTime()) {
      lastBarTime = millis();
      moveBarDown();
    }
  }

  // else wait for someone to start game

  if (winGame) {
    Serial.println("You win");
    digitalWrite(targetLEDPin, LOW); 
    // flashAllTargetLEDs(); //TODO: change to new code with LED array
    // displayWinMessage();
  } else if (loseGame){
    Serial.println("You lose");
    digitalWrite(targetLEDPin, LOW); 
    // displayLoseMessage();
  } else {
    Serial.println("You were idle or you reset the game");
    // flashAllTargetLEDs();
    // displayLoseMessage();
    // TODO: edge case when it is idle: force ball to any hole? or don't reset ball next time?
  }

}
