#include <Arduino.h>
#include <Stepper.h>
#include "RunningAverage.h"
#include "SevSeg.h"
#include "Wire.h"
#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include <avr/interrupt.h>
#pragma once

/**********CONSTANTS*****************************/
#define BOTTOMPIN 44 // FIXME this pin is for the sensor at the bottom of the backboard ramp
                     // add this IR sensor to the mux circuit
#define SCOREINCREASE 10
#define NUMTARGETS 30
#define IROFFSET 25
#define STARTBUTTONPIN 2
#define JOYSTICK_R_UP 3
#define JOYSTICK_R_DOWN 4
#define JOYSTICK_L_UP 5
#define JOYSTICK_L_DOWN 6

#define MOTOR_R_STEP 7
#define MOTOR_R_DIR  8
#define MOTOR_L_STEP 9
#define MOTOR_L_DIR  10
#define RESET_MOTOR_STEP 11
#define RESET_MOTOR_DIR 12

// fixme give these a better name
#define SPARK_PCB_1 24
#define SPARK_PCB_2 26
#define SPARK_PCB_3 28

#define BAR_SENSOR_PIN 30 // todo: add sensor to calibrate bar position when display powered on
#define HEX_PLAYER_CLK 32
#define HEX_PLAYER_DIO 34
#define HEX_HIGHSCORE_CLK 36
#define HEX_HIGHSCORE_DIO 38

// place the mux pins along the same row on the mega, next to the analog pins
// fixme give these a better name
#define MUX_DATA A15
#define MUX_SELECT_1 46
#define MUX_SELECT_2 48
#define MUX_SELECT_3 50
#define MUX_SELECT_A 47
#define MUX_SELECT_B 49
#define MUX_SELECT_C 51

extern int BUILTIN_LED; // connect Led to arduino pin 13

#define STEPS_PER_REV 800 //DRV driver
#define CEILING 0                //highest height of bar
#define FLOOR 299             //bottom of the playing area, not actually the floor
#define BALL_RETURN_HEIGHT 388 //lowest height of bar, where bar will pick up ball
#define MAX_BAR_TILT 38         //maximum vertical slope of bar, aka barPosRight - barPosLeft
#define MAX_SPEED 200 //in rpm
#define STEP_INCR 800 //steps taken on each loop() iteration
#define STEPS_30_DEG 67
#define BALL_RETURN_DELAY_MS 2000 //time to wait until a new ball has rolled onto bar

// FOR WOKWI TESTING ONLY:
#define LOSE_GAME_BUTTON 14
#define WIN_LEVEL_BUTTON 15

#define NUM_IR_SAMPLES 10 // @ginny please adjust as you see fit
/************END OF CONSTANTS*********************/

/************GLOBAL VARIABLE DEFINITIONS **********************/
// spark PCB
// todo: change these to the correct chip: SN74HC595N
extern Adafruit_MCP23X17 mcp1; //shift registers; 8 of them, each with 16 pins
extern Adafruit_MCP23X17 mcp2;
extern Adafruit_MCP23X17 mcp3;
extern Adafruit_MCP23X17 mcp4;
extern Adafruit_MCP23X17 mcp5;
extern Adafruit_MCP23X17 mcp6;
extern Adafruit_MCP23X17 mcp7;
extern Adafruit_MCP23X17 mcp8;

extern RunningAverage targetIRBuffer; // stores IR sensor data
extern int targetHoles[NUMTARGETS]; // stores pin numbers of target LEDs to light up

extern volatile bool playingGame; //true if someone is playing, false if game over
extern bool winGame; 
extern bool loseGame;
extern int score;
extern int targetDifficulty;
extern int level;
extern int targetLEDPin;
extern int targetSensorPin;
extern bool targetBroken;
extern bool bottomBroken;

// hex display variables
extern int highscore;

//global vars for pins for input buttons 
extern int left_button;
extern int right_button;

//global variables for timing 
extern unsigned long finishTime;  //time when the ball drops into target hole, resets each round
extern unsigned long idleTime;
extern volatile long debounce_time;
extern volatile long current_button_time;

//global vars for bar movement
extern int moveLeftBarUp;
extern int moveRightBarUp;
extern int barPosL;
extern int barPosR;
extern int barTilt;
extern Stepper motorR;
extern Stepper motorL;
extern int stepsPerRevolution;
extern Stepper motorBallReturn; 

extern SevSeg sevseg1;
extern SevSeg sevseg2;
extern SevSeg sevseg3;

/************END OF GLOBAL DEFINITIONS**********************/
