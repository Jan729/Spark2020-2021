#include "TM1637Display.h"
#pragma once

/**********HELPER FUNCTION PROTOTYPES******************/
void waitToStartGame();
void resetGameVariables();
void resetLevelVariables();
void setupNextLevel();
void resetBall();
void resetGame();
bool playerIsIdle();

bool beamBroken();
void pollIRSensors();
void calibrateBarPosition();
void pollLeftJoystick();
void pollRightJoystick();
void setupBarMotors();
void moveBar();
void moveBarDown();
void resetBarAndBall();

void updateLights(int lastHole, int newHole);

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
/******END OF HELPER FUNCTION PROTOTYPES***************/
