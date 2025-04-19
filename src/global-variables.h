#pragma once

#include <Arduino.h>
#include <AccelStepper.h>
#include "RunningAverage.h"
#include "Wire.h"
#include <Wire.h>
#include "Adafruit_MCP23X17.h"
#include <avr/interrupt.h>
#include "TM1637Display.h"


/**********CONSTANTS*****************************/
#define IS_WOKWI_TEST false

#define BOTTOMPIN 44 // FIXME this sensor at the bottom of the backboard ramp
                     // has not been built
#define BONUS_DECREASE_INTERVAL_MS 5000
#define SCOREINCREASE 10
#define NUMTARGETS 30
#define NUM_LEDS 4

#define STARTBUTTONPIN 2

#define STEP_L 3
#define DIR_L 4
#define STEP_R 5
#define DIR_R 6

#define JOYSTICK_L_UP 7
#define JOYSTICK_L_DOWN 8
#define JOYSTICK_R_UP 9
#define JOYSTICK_R_DOWN 10

#define PARENT_A 11
#define PARENT_B 12
#define PARENT_C 13
#define CHILD_S1 14
#define CHILD_S2 15
#define CHILD_S3 16

#define MUX_OUT_ANALOG A1 // Analog input to read the selected mux output

#define LED_LATCH_PIN 17  // latch pin of shift register
#define LED_CLOCK_PIN 18 // clock pin of shift register
#define LED_DATA_PIN 19 // data pin of shift register

// TODO ball return stepper pins
// #define RESET_MOTOR_STEP 11
// #define RESET_MOTOR_DIR 12

// fixme give these a better name
#define SPARK_PCB_1 24
#define SPARK_PCB_2 26
#define SPARK_PCB_3 28

#define BAR_SENSOR_PIN 30 // TODO: add sensor to calibrate bar position when display powered on? or store bar position in non-volatile memory?
#define HEX_CUR_SCORE_CLK 32
#define HEX_CUR_SCORE_DIO 34
#define HEX_BONUS_CLK 36
#define HEX_BONUS_DIO 38
#define HEX_HIGHSCORE_CLK 40
#define HEX_HIGHSCORE_DIO 42

// place the mux pins along the same row on the mega, next to the analog pins
// fixme give these a better name
#define MUX_DATA A15
#define MUX_SELECT_A1 46
#define MUX_SELECT_B1 48
#define MUX_SELECT_C1 50
#define MUX_SELECT_A2 47
#define MUX_SELECT_B2 49
#define MUX_SELECT_C2 51

extern int BUILTIN_LED; // connect Led to arduino pin 13

// start/reset button constants
#define DEBOUNCE_DELAY_MS 100

// bar motors constants
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
#define BAR_SAVE_PERIOD_S 3

// FOR WOKWI TESTING ONLY:
#define TEST_LOSE_GAME_BUTTON 14
#define TEST_WIN_LEVEL_BUTTON 15

#define NUM_IR_SAMPLES 10 // TODO please adjust as you see fit

/************END OF CONSTANTS*********************/

/************GLOBAL VARIABLE DEFINITIONS **********************/
extern volatile bool playingGame; //true if someone is playing, false if game over
extern bool winGame; 
extern bool loseGame;
extern int level;
extern int targetLEDPin;
extern int targetSensorPin;
extern bool wonLevel;
extern bool ballFellIntoBackboard;
extern bool wonLevelState;
extern bool ballAtBottomState;

// hex display variables
extern int highScore;
extern int curScore;
extern int bonusScore;
extern TM1637Display bonusScoreDisplay;
extern TM1637Display curScoreDisplay;
extern TM1637Display highScoreDisplay;



//global variables for timing 
extern unsigned long startLevelTime;
extern unsigned long finishTime;  //time when the ball drops into target hole, resets each round
extern unsigned long lastBonusUpdateTime;
extern unsigned long lastBarSaveTime;

//global vars for pins for bar movement
extern int leftBarInput;
extern int rightBarInput;
extern unsigned long lastBarTime;
extern AccelStepper motorR;
extern AccelStepper motorL;

// to calculate maximum bar tilt
extern long barPosR;
extern long barPosL;
extern int barTilt;

// Global variables for LEDs/shift registers

extern byte leds1;   // each var controls leds in each shift register
extern byte leds2; 
extern byte leds3;
extern byte leds4;

extern byte* ledsArray[NUM_LEDS];

/************END OF GLOBAL DEFINITIONS**********************/
