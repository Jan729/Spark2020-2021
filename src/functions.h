#include "TM1637Display.h"
#pragma once

/**********HELPER FUNCTION PROTOTYPES******************/
void waitToStartGame();
void resetAllVariables();
void incrementLevel();
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
void updateScore(int curScore, int bonusScore, int highScore);
void displayBonus(int bonusScore, int targetDifficulty, int finishTime);
void updateScore(int curScore, int bonusScore, int highScore);
void displayScore(TM1637Display display, int score);
void resetScores();
void sethighScore(int curScore, int highScore);

void resetScores();
void flashAllTargetLEDs();
void displayWinMessage();
void displayLoseMessage();
/******END OF HELPER FUNCTION PROTOTYPES***************/
