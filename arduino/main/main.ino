#include "global-variables.h"
#include "functions.h"
#include "RunningAverage.h"
#include "Wire.h"
#include <Wire.h>
#include <avr/interrupt.h>
#include <AccelStepper.h>
#include <Wire.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  //  OLED display height, in pixels
/************LOCAL VARIABLE DECLARATIONS **********************/
// To make a variable global, add it to
// global-variables.h with an extern definition

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
long barPosR;
long barPosL;
int barTilt;
unsigned long lastBarTime;
unsigned long lastBarSaveTime;
AccelStepper motorR = AccelStepper(AccelStepper::DRIVER, STEP_R, DIR_R);
AccelStepper motorL = AccelStepper(AccelStepper::DRIVER, STEP_L, DIR_L);

// TODO Stepper motorBallReturn = Stepper(STEPS_PER_REV, RESET_MOTOR_STEP, RESET_MOTOR_DIR); 

// main loop control variables
volatile bool playingGame = false; //true if someone is playing, false if game over

byte leds1 = 0;
byte leds2 = 0;
byte leds3 = 0;
byte leds4 = 0;

byte* ledsArray[NUM_LEDS] = {&leds1, &leds2, &leds3, &leds4};

/************END OF LOCAL DECLARATIONS**********************/

/****** GAME CONTROL FUNCTIONS ****/

void setupNextLevel() {
  level++;
  updateMux(); // Sets up MUX to poll a select switch
  wonLevel = false;
  ballFellIntoBackboard = false;

  // !!!!!! DO NOT GET RID OF THIS. IF THIS IS REMOVED, IT BREAKS THE CODE
  int oldTarget = targetLEDPin;
  oldTarget = oldTarget;
  // *******************************

  targetLEDPin++; // TODO select next target LED from spark PCB
  targetSensorPin++; // TODO select next IR sensor with mux

  resetBarAndBall();
  writeLed(targetLEDPin); // This resets the older pin too
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
  
  // TODO: Probably move these into their own setup functions and call them here
  
  pinMode(MUX_OUT_ANALOG, INPUT);
  // Set all control pins as outputs
  pinMode(PARENT_A, OUTPUT);
  pinMode(PARENT_B, OUTPUT);
  pinMode(PARENT_C, OUTPUT);

  pinMode(CHILD_S1, OUTPUT);
  pinMode(CHILD_S2, OUTPUT);
  pinMode(CHILD_S3, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT); // Debugging

  pinMode(LED_LATCH_PIN, OUTPUT);
  pinMode(LED_DATA_PIN, OUTPUT);  
  pinMode(LED_CLOCK_PIN, OUTPUT);


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

    pollMux();

    unsigned long timeNow = millis();
    updateBonus(timeNow);

    if (wonLevel) {
      Serial.println((String)"Won level " + level);
      finishTime = timeNow;

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
      writeBarPositionsIntoEEPROM();
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
