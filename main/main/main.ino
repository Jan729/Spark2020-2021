
/* 
  IR Breakbeam sensor demo!
*/

/**********CONSTANTS*****************************/
#define SENSORPIN 4
#define BOTTOMPIN 13
#define SCOREINCREASE 10
#define NUMTARGETS 35
#define IROFFSET 25
#include <Stepper.h>
#include "RunningAverage.h"
#include "SevSeg.h"
#include "Wire.h"
//these can be changed and we need 
//two for each IR sensor

//START OF PIN DECLARATIONS
int IRSensor = 2; // connect ir sensor to arduino pin 2
int LED = 13; // conect Led to arduino pin 13

#define BAR_SENSOR_PIN 12
#define MOTOR_R_STEP 14
#define MOTOR_R_DIR  15
#define MOTOR_L_STEP 16
#define MOTOR_L_DIR  17
#define RESET_MOTOR_STEP 0
#define RESET_MOTOR_DIR 1
Stepper motorR = Stepper(MOTOR_R_STEP, MOTOR_R_DIR);
Stepper motorL = Stepper(MOTOR_L_STEP, MOTOR_L_DIR);

//gpio pins. MUST USE SPECIAL PINS 20 and 21
#define SERIAL_DATA_LINE 20
#define SERIAL_CLOCK_LINE 21

//END OF PIN DECLARATIONS

//experimental stepper positions for bar movement
#define CEILING 0                //highest height of bar
#define FLOOR 10000             //bottom of the playing area, not actually the floor
#define BALL_RETURN_HEIGHT 11000 //lowest height of bar, where bar will pick up ball
#define MAX_BAR_TILT 200         //maximum vertical slope of bar, aka barPosRight - barPosLeft
#define MAX_SPEED 10 //in rpm
#define STEP_INCR 1 //steps taken on each loop() iteration
#define SPEED_MULT 5           //multiply user input value with this number to set desired stepper speed
#define BALL_RETURN_DELAY 2000 //time to wait until a new ball has rolled onto bar

//distance sensors code will give the motors a number within the range [-3, 3]
//-ve speed means "down", +ve speed means "up"
#define STOP 0
#define SLOW 1
#define MED 2
#define FAST 3

/************END OF CONSTANTS*********************/

/************GLOBAL VARIABLES**********************/

// time for sound wave to travel to object and bounce back
long duration;
// distance between sensor and object
int distance;

RunningAverage holes[35];

bool playingGame = true; //true if someone is playing, false if game over
bool winGame = false; 
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

int targetHoles[NUMTARGETS]; //sequential pin numbers of target holes, eg 0, 1, 2, 3...

//global vars for bar movement
int userSpeedLeft = 0; //from get_left_user_input()
int userSpeedRight = 0;
int prevSpeedLeft = 0;
int prevSpeedRight = 0;
int barPosL = FLOOR;
int barPosR = FLOOR;
int barTilt = 0;

int speedBoost = 1; //TODO: increment this number to increase the bar speed

SevSeg sevseg1;
SevSeg sevseg2;
SevSeg sevseg3;

//global variables for power downs
bool sped_up = false;

/************END OF GLOBAL VARIABLES**********************/

/**********HELPER FUNCTION PROTOTYPES******************/
void waitToStartGame();
void updateTarget();
void resetBall();
void resetGame();
void checkIdleTime();
int smooth_distance (int num_samples);
int sample_distance();
bool beamBroken(int target);
void ballEntry();
void setBarSpeed();
void moveBar();
void resetBar();
void updateLights(int lastHole, int newHole);
void updateScore();
void displayScore();
void sethighScore();
void powerdown_handler();
void power_down_reverse_control (bool opposite);
void power_down_change_speed();
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
  
    const int speed=100; //arbitrary, to be adjusted with testing
    const int stepsPerRevolution=800; //not used but for reference
    int stepsFor30Degrees=67; //800*(30/360) rounded as int

    Stepper myStepper(stepsPerRevolution,2,3); //need to make sure pins (2,3) are consistent with pin declarations in main***
    
    myStepper.setSpeed(speed); //setSpeed only take a positive number
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
int smooth_distance (int num_samples) {
  int value = 0;
  int current_distance = 0;
  for (int i = 0; i < num_samples; i++) {
    current_distance = sample_distance();
    if (current_distance > 0 && current_distance < 25) {
      value = value + sample_distance();
      //Serial.print("Good Reading.\n");
    } else {
      //Serial.print("Bad Reading.");
      //take another sample of the data if there was a bad read
      //probably going to need to remove this
      //but it works well to test averages
      i--;
    }

  }

  return (value / num_samples);
}

int sample_distance() {
  int current_duration;
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  //wait max 50,000 microseconds before reading
  //may have to change this value
  current_duration = pulseIn(echoPin, HIGH, 50000);

  // Calculating the distance
  current_distance = current_duration * 0.034 / 2;

  // Prints the distance on the Serial Monitor
  //Serial.print("Distance: ");
  //Serial.println(current_distance);

  return current_distance;
}

/****** END OF USER INPUT FUNCTIONS ****/

/************ BALL DETECTION FUNCTIONS *********/

bool beamBroken(int target)
{

  bool beamBroke = false;
  //#define SENSORPIN 4

  // variables will change: // 1 = unbroked, 0 = broke 
  int sensorState;
  sensorState = digitalRead(SENSORPIN);

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
    resetGame();

  }

}

/************ END OF BALL DETECTION FUNCTIONS ***********/

/*********** START OF BAR MOVEMENT FUNCTIONS *********/

//helper fxn for moveBar()
//changes the motor speed if user input speed changes
void setBarSpeed() { 
  if(userSpeedLeft != prevSpeedLeft) {
      motorL.setSpeed(userSpeedLeft * SPEED_MULT + speedBoost);
    }
  if(userSpeedRight != prevSpeedRight) {
      motorR.setSpeed(userSpeedRight * SPEED_MULT + speedBoost);
    }
  prevSpeedLeft = userSpeedLeft;
  prevSpeedRight = userSpeedRight;
  userSpeedLeft = userSpeedLeft * SPEED_MULT + speedBoost;
  userSpeedRight = userSpeedRight * SPEED_MULT + speedBoost;
}

//will move right motor and left motor 1 step each time moveBar() is called
void moveBar()
{
  setBarSpeed();

    if (userSpeedRight > 0 && barPosR > CEILING && barTilt < MAX_BAR_TILT)
    { //move right side of bar UP
      motorR.step(STEP_INCR);
      barPosR-= 1;
    }
    else if (userSpeedRight < 0 && barPosR < FLOOR && barTilt > -MAX_BAR_TILT)
    { //move right side of bar DOWN
      motorR.step(-STEP_INCR);
      barPosR+= 1;
    }

    if (userSpeedLeft > 0 && barPosL > CEILING && barTilt > -MAX_BAR_TILT)
    { //move left side of bar UP
      motorL.step(STEP_INCR);
      barPosL-= 1;
    }
    else if (userSpeedLeft < 0 && barPosL < FLOOR && barTilt < MAX_BAR_TILT)
    { //move left side of bar DOWN
      motorL.step(-STEP_INCR);
      barPosL+= 1;
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
/************ END OF OUTPUT FUNCTIONS ***********/

/************ START OF POWER DOWN FUNCTIONS ***********/
void powerdown_handler(){
  if(level <=7){
    power_down_reverse_control(false);
  }else{
    int powerdown = (int)random(0,2);
    
    switch(powerdown){
      case 0:
        power_down_reverse_control(true);
        break;
      case 1:
        power_down_reverse_control(false);
        power_down_change_speed();
        break;
      case 2:
        power_down_reverse_control(true);
        power_down_change_speed(true);
        break;
      default:
        power_down_reverse_control(false);
        break;
    }
  }
}

void power_down_reverse_control () {
  int temp_left = left_button;
  left_button = right_button;
  right_button = temp_left;
}


void power_down_change_speed() { 
  int increase  = (int)random(1, 3);
  speedBoost = speedBoost + increase; 
}


/************ END OF POWER DOWN  FUNCTIONS ***********/

/**********END OF HELPER FUNCTION IMPLEMENTATIONS******/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // Starts the serial communication

  for (int i = IROFFSET; i < NUMTARGETS+IROFFSET; i++) {
    pinMode(i, INPUT); //IR receivers, one pin per target hole
    digitalWrite(i, HIGH); // turn on the pullup
    targetHoles[i] = i;
  }

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
  
  resetBar();
}

void loop() {

  waitToStartGame(); //wait for user to start game and set PlayingGame = true

  while (playingGame) {
    //user input: just for testing purposes
    //distance = smooth_distance(20);
    //Serial.print("Distance: ");
    //Serial.println(distance);

    
    //TODO: Work out where to call start and end times (NOT DONE)
    finishTime = millis(); //might not need here depending on when updateTarget is called. 
  
    get_left_user_input(); // needs to be written, use global var left_button
    get_right_user_input();// needs to be written, use global var right_button
    moveBar();
    ballEntry();
    checkIdleTime();

  }

  if (winGame) {
    digitalWrite(targetLEDPin, LOW); 
    Serial.println("Yay you win !!");
  } 
}
