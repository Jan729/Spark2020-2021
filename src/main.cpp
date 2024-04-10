#include "global-variables.h"
#include "functions.h"
#include "RunningAverage.h"
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
int level;
int targetLEDPin;
int targetSensorPin;
bool wonLevel;
bool ballFellIntoBackboard;

// hex display variables
int bonusScore;
int curScore;
int highScore;

//global variables for timing 
unsigned long startLevelTime;
unsigned long finishTime;  //time when the ball drops into target hole, resets each round
unsigned long lastBonusUpdateTime;

//global vars for bar movement
int leftBarInput;
int rightBarInput;
int barTilt;
unsigned long lastBarTime;
AccelStepper motorR = AccelStepper(AccelStepper::DRIVER, STEP_R, DIR_R);
AccelStepper motorL = AccelStepper(AccelStepper::DRIVER, STEP_L, DIR_L);

// TODO Stepper motorBallReturn = Stepper(STEPS_PER_REV, RESET_MOTOR_STEP, RESET_MOTOR_DIR); 

// main loop control variables
volatile bool playingGame = false; //true if someone is playing, false if game over

/************END OF LOCAL DECLARATIONS**********************/

/****** GAME CONTROL FUNCTIONS ****/

void setupNextLevel() {
  level++;

  wonLevel = false;
  ballFellIntoBackboard = false;

  int oldTarget = targetLEDPin;
  targetLEDPin++; // TODO select next target LED from spark PCB
  targetSensorPin++; // TODO select next IR sensor with mux

  resetBarAndBall();
  updateLights(oldTarget, targetLEDPin); //TODO: update lights on spark PCB
  lastBarTime = millis();

  startLevelTime = millis();
  lastBonusUpdateTime = startLevelTime;
  bonusScore = level*100;
  displayScore(bonusScoreDisplay, bonusScore);
  displayScore(curScoreDisplay, curScore);
}

void waitToStartGame() { 
    Serial.println("waiting for someone to start game");
    while(!playingGame) {
      // do nothing until interrupt changes the state
    }
}

void resetGame() {
  winGame = false; 
  loseGame = false;
  curScore = 0;
  level = 0;
  retrieveHighScore();

  calibrateBarPosition();

  setupNextLevel();
}

bool playerIsIdle() {
  unsigned long currentTime = millis();
  unsigned long joysticksIdleMs = currentTime  - lastBarTime;
  return joysticksIdleMs/1000 > BAR_DOWN_DELAY_S;
}

//ISR for start/stop game button only to change game state
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

void setupJoystickPins() {
  pinMode(JOYSTICK_R_UP, INPUT_PULLUP);
  pinMode(JOYSTICK_R_DOWN, INPUT_PULLUP);
  pinMode(JOYSTICK_L_UP, INPUT_PULLUP);
  pinMode(JOYSTICK_L_DOWN, INPUT_PULLUP);
}

/****** END OF GAME CONTROL FUNCTIONS ****/

/**********END OF HELPER FUNCTION IMPLEMENTATIONS******/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // Starts the serial communication

  #if IS_WOKWI_TEST

    pinMode(TEST_WIN_LEVEL_BUTTON, INPUT_PULLUP);
    pinMode(TEST_LOSE_GAME_BUTTON, INPUT_PULLUP);
  #endif

  // TODO initialize spark pcb
  // TODO init IR receivers
  //TODO keep IR emitters on at all times? is that too much current?
  
  //button interrupt
  pinMode(STARTBUTTONPIN, INPUT_PULLUP);  //Start button, LOW when pressed
  attachInterrupt(digitalPinToInterrupt(STARTBUTTONPIN), startButtonPressed, FALLING);

  // Game Input Buttons to Move Bar
  setupJoystickPins();

  setHexDisplayBrightness();
  setupBarMotors();
}

void loop() {

  waitToStartGame(); 
  resetGame();
  Serial.println("Game starts!");

  while (playingGame) {
    moveBar();
    pollIRSensors();
    updateBonus(millis());

    if (wonLevel) {
      Serial.println((String)"Won level " + level);

      finishTime = millis();
      updateScore();
      bonusScoreDisplay.clear();

      if(level == NUMTARGETS) {
        playingGame = false;
        winGame = true; //win the game
      } else {
        setupNextLevel();
      }

    } else if (ballFellIntoBackboard) {
      playingGame = false;
      loseGame = true;
    }

    if (playerIsIdle()) {
      lastBarTime = millis();
      moveBarDown();
    }
  }

  digitalWrite(targetLEDPin, LOW); 

  if (winGame) {
    Serial.println("You win");
    displayWinMessage();
  } else if (loseGame){
    Serial.println("You lose");
    displayLoseMessage();
  } else {
    Serial.println("You reset the game");
  }

}
