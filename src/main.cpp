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

volatile bool playingGame; //true if someone is playing, false if game over
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
unsigned long idleTime;
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
bool wonLevelState = false;
bool ballAtBottomState = false;

/************END OF LOCAL DECLARATIONS**********************/

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

  //global variables for timing 
  finishTime = 0;  //time when the ball drops into target hole, resets each round
  idleTime = 0;
  debounce_time = 0;
  current_button_time = 0;

  //global vars for bar movement
  leftBarInput = 0;
  rightBarInput = 0;
  barPosL = FLOOR;
  barPosR = FLOOR;
  barTilt = 0;
  stepsPerRevolution = 800;
}

/**********START OF HELPER FUNCTION IMPLEMENTATIONS*******************/

void waitToStartGame() {
  //wait and do nothing until someone presses "start"
  //then continue with game loop

  //player places hand over one of the sensors to start
  //has to be high above sensor
  int start_game_dummy_var = 1;
  if (start_game_dummy_var == 1) {
    //start the game
    playingGame = true;
    resetGame();
  }
  //otherwise do nothing
  //Serial.println("wait to start the game. pretend someone starts playing game");

}

void updateTarget() {
  level++; 

  int oldTarget = targetLEDPin;
  
  targetLEDPin++; // TODO select next target LED from spark PCB
  targetSensorPin++; // TODO select next IR sensor with mux

  if(level > NUMTARGETS){
    playingGame = false;
    winGame = true; //win the game
  }

  updateLights(oldTarget, targetLEDPin);
}

void resetBall() { // TODO check +ve and -ve direction
    motorBallReturn.step(STEPS_30_DEG);
    delay(BALL_RETURN_DELAY_MS);
    motorBallReturn.step(-STEPS_30_DEG);
  
  Serial.println("reset ball");
}

void resetGame(){
  // TODO calibrate bar if powered off display and bar is in an uknown position
    level = 1;
    score = 0;
    targetLEDPin = 0;
    targetDifficulty = 0;
    targetBroken = false;
    bottomBroken = false;
    resetBarAndBall();
    idleTime = millis();
    displayScore();
    digitalWrite(targetLEDPin, HIGH);
}

void checkIdleTime(){
  if (idleTime/60000 > 5) // 5 minutes of idle
    playingGame = false;
}

/****** USER INPUT FUNCTIONS ****/
//ISR for start/stop game button
void buttonPressed() {
  if (playingGame == true) {
    Serial.println("button to end game is pressed");
    //stop game, reset, and wait to start
    playingGame = false;
    resetGame();
    // TO DO: wait to start in main loop, by reading playingGame value. have to redo main loop logic.
    delayMicroseconds(100000); //non-blocking delay to debounce button
  }
  else if(playingGame == false) {
    Serial.println("button to start game is pressed");
    //start game
    playingGame = true;
    delayMicroseconds(100000); //non-blocking delay to debounce button
  }
}

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

/****** END OF USER INPUT FUNCTIONS ****/

/************ BALL DETECTION FUNCTIONS *********/

bool beamBroken(int target) // FIXME this code is outdated
{

  // bool beamBroke = false;

  // variables will change: // 1 = unbroked, 0 = broke 
  // int sensorState;
    
  // if(targetSensorPin < 16)
  // {
  //   sensorState = mcp1.digitalRead(targetSensorPin%16);
  // }
  
  // else if(16 <= targetSensorPin < 32)
  // {
  //   sensorState = mcp2.digitalRead(targetSensorPin%16);
  // }
  
  // else
  // {
  //   sensorState = mcp3.digitalRead(targetSensorPin%16);
  // }
  

  //note that the array index may need to be changed
  //depending on the actual value of the sensorpin
  //this implementation assumes all IRs are using contiguous
  //pins starting at 0
  //so will probably need an offset (if pins start at 4 for example)
  
  // targetIRBuffer.addValue(!beamBroken*5);
  //fiddle with the number 5 s.t. when beam is broken
  //it affects the average more

  //maybe change value from 1 to 0 or something else
  // if (targetIRBuffer.getAverage() > 1) {
  //   return 1;
  // }
  return 0; //!beamBroken; //this way 1 = broken and 0 = unbroken
}


void pollIRSensors() {

#if IS_WOKWI_TEST
  bool wonLevel = digitalRead(WIN_LEVEL_BUTTON) == LOW;
  bool ballAtBottom = digitalRead(LOSE_GAME_BUTTON) == LOW;
  
  // catch rising edge of test buttons
  if (wonLevel && !wonLevelState) {
    wonLevelState = true;
  } else if (!wonLevel && wonLevelState) {
    wonLevelState = false;
  }
  
  if (ballAtBottom && !ballAtBottomState) {
    ballAtBottomState = true;
  } else if (!ballAtBottom && ballAtBottom) {
    ballAtBottomState = false;
  }
  
  targetBroken = wonLevelState;
  bottomBroken = ballAtBottomState;
#else
  targetBroken = false; // TODO replace with Ginny's IR sensor polling logic
  bottomBroken = false; // poll sensor at bottom of the game
#endif

  if (targetBroken) { //ball fell in good hole
    Serial.println((String)"Won level " + level);
    targetBroken = false;
    wonLevelState = false;
    resetBarAndBall();

    updateTarget();
    finishTime = millis();
    updateScore();
    displayScore();
    
  } else if (bottomBroken) { //ball fell in bad hole  
    playingGame = false;
    loseGame = true; // TODO add more than 1 life if game is too hard
    ballAtBottomState = false;
  }
}

/************ END OF BALL DETECTION FUNCTIONS ***********/

/************ START OF OUTPUT FUNCTIONS ***********/
void updateLights(int lastHole, int newHole){
  digitalWrite(lastHole, LOW);
  digitalWrite(newHole, HIGH);
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

// TODO: update this code to work with our current parts

  // Wire.beginTransmission(0x20); //select chip 0x20
  // Wire.write(0x12); // Select Port A
  // Wire.write(gpio_num_array[thirdDigit]); // Send data
  // Wire.endTransmission();
  // Wire.beginTransmission(0x20); //select chip 0x20
  // Wire.write(0x13); // Select Port B
  // Wire.write(gpio_num_array[highFirst]); // Send data
  // Wire.endTransmission();
  // Wire.beginTransmission(0x27); //select chip 0x27
  // Wire.write(0x12); // Select Port A
  // Wire.write(gpio_num_array[highSecond]); // Send data
  // Wire.endTransmission();
  // Wire.beginTransmission(0x27); //select chip 0x27
  // Wire.write(0x13); // Select Port B
  // Wire.write(gpio_num_array[highThird]); // Send data
  // Wire.endTransmission();

}

void sethighScore() {
  // if (score > highscore) {
  //   highscore = score;
  //   int highFirst = high/100;
  //   int highSecond = (highscore-highFirst)/10;
  //   int highThird = (highscore-highFirst-highSecond);
  // }
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
    sevseg3.setChars("u");
    sevseg3.refreshDisplay(); 
    delay(500);
    sevseg1.setChars("w");
    sevseg1.refreshDisplay(); 
    sevseg2.setChars("i");
    sevseg2.refreshDisplay(); 
    sevseg3.setChars("n");
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
    sevseg3.setChars("u");
    sevseg3.refreshDisplay(); 
    delay(500);
    sevseg1.setChars("l");
    sevseg1.refreshDisplay(); 
    sevseg2.setChars("o");
    sevseg2.refreshDisplay(); 
    sevseg3.setChars("-");
    sevseg3.refreshDisplay(); 
    delay(500);
    sevseg1.setChars("e");
    sevseg1.refreshDisplay(); 
    sevseg2.setChars("!");
    sevseg2.refreshDisplay(); 
    sevseg3.setChars("!");
    sevseg3.refreshDisplay(); 
    delay(500);
  }
}

/************ END OF OUTPUT FUNCTIONS ***********/

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

  waitToStartGame(); //FIXME wait for user to start game and set PlayingGame = true

  while (playingGame) {
    //TODO: Work out where to call start and end times (NOT DONE)
    finishTime = millis(); //might not need here depending on when updateTarget is called. 
  
    pollBarJoysticks(); 
    moveBar();
    pollIRSensors();
    checkIdleTime();

  }

  if (winGame) {
    Serial.println("You win");
    digitalWrite(targetLEDPin, LOW); 
    flashAllTargetLEDs();
    displayWinMessage();
  } else if (loseGame){
    Serial.println("You lose");
    digitalWrite(targetLEDPin, LOW); 
  }else {
    flashAllTargetLEDs();
    displayLoseMessage();
  }

  displayScore(); //replace win/lose message with player score
  resetGame();
}
