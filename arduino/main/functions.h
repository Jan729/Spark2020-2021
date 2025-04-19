#pragma once

#include "TM1637Display.h"
/**********HELPER FUNCTION PROTOTYPES******************/
void waitToStartGame();
void resetGameVariables();
void resetLevelVariables();
void setupNextLevel();
void resetBall();
void resetGame();
bool playerIsIdle();

bool beamBroken();
void pollMux();
void updateMux();
void calibrateBarPosition();
void pollLeftJoystick();
void pollRightJoystick();
void setupBarMotors();
void moveBar();
void moveBarDown();
void resetBarAndBall();

void writeLed(int ledNum);
void updateShiftRegister();

void setHexDisplayBrightness();
void updateScore();
void updateBonus(unsigned long timeNow);
void displayScore(TM1637Display display, int score);
void resetScores();
void retrieveHighScore();
void sethighScore();

void resetScores();
void flashAllTargetLEDs();
void displayWinMessage();
void displayLoseMessage();


int getHighScoreFromEEPROM();
void writeHighScoreToEEPROM();
void writeBarPositionsIntoEEPROM();
void setBarPositionsFromEEPROM();

/******END OF HELPER FUNCTION PROTOTYPES***************/
