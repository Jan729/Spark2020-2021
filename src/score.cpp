#include <Arduino.h>
#include "functions.h"
#include "global-variables.h"
#include "TM1637Display.h"

// using 2 four digit 7-segment displays –> one for current score, one for high score
// use TM1637Display library (installed as a .cpp and .h file in the src directory)
// https://github.com/avishorp/TM1637
// CLK and DIO pins need to be connected to Arduino

// create new instance of the TM1637Display class by passing clk and dio pin values to it
TM1637Display bonusScoreDisplay(HEX_BONUS_CLK, HEX_BONUS_DIO);
TM1637Display curScoreDisplay(HEX_CUR_SCORE_CLK, HEX_CUR_SCORE_DIO);
TM1637Display highScoreDisplay(HEX_HIGHSCORE_CLK, HEX_HIGHSCORE_DIO);

void setHexDisplayBrightness() {
  bonusScoreDisplay.setBrightness(7); // brightness level from 0 (lowest) to 7 (highest)
  curScoreDisplay.setBrightness(7);
  highScoreDisplay.setBrightness(7);
}

void updateBonus(unsigned long timeNow) {
  if (timeNow - lastBonusUpdateTime < BONUS_DECREASE_INTERVAL_MS) {
    return;
  }

  bonusScore -= SCOREINCREASE;
  if (bonusScore < 0) {
    bonusScore = 0;
  }
  lastBonusUpdateTime = timeNow;
  displayScore(bonusScoreDisplay, bonusScore);
}

void updateScore() {
  curScore += bonusScore;
  displayScore(curScoreDisplay, curScore);
  sethighScore(); // check if high score has been surpassed
}

void displayScore(TM1637Display display, int score) {
  display.showNumberDec(score, false);
}

void retrieveHighScore() {
  // TODO:  retrieve high score from non volatile memory
}
void sethighScore() {
  if (curScore > highScore) {
    highScore = curScore;
    highScoreDisplay.clear();
    delay(250);
    highScoreDisplay.showNumberDec(highScore, false);
    delay(500);
    highScoreDisplay.clear();
    delay(250);
    highScoreDisplay.showNumberDec(highScore, false);
    delay(500);
    highScoreDisplay.clear();
    delay(250);
    highScoreDisplay.showNumberDec(highScore, false);

    // TODO: store highscore in non volatile memory 
  }
}

void displayLoseMessage() { // display "OOPS" if game is lost
  uint8_t oops[] = {
		 SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,  // O
		 SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,  // O
		 SEG_A | SEG_B | SEG_E | SEG_F | SEG_G,          // P
		 SEG_A | SEG_C | SEG_D | SEG_F | SEG_G           // S
		};

    bonusScoreDisplay.setSegments(oops);
    curScoreDisplay.clear();
    delay(250);
    displayScore(curScoreDisplay, curScore);
    bonusScoreDisplay.clear();
    delay(500);
    bonusScoreDisplay.setSegments(oops);
    delay(500);
    displayScore(curScoreDisplay, curScore);
}

void displayWinMessage() { // display "WIN" if game is won
    uint8_t win[] = {
    SEG_C | SEG_D | SEG_E | SEG_F, // w first half
    SEG_B | SEG_C | SEG_D | SEG_E, // w second half
    SEG_B | SEG_C, // "I"
    SEG_A | SEG_B | SEG_C | SEG_E | SEG_F // "n"
    };
    bonusScoreDisplay.setSegments(win);
    curScoreDisplay.clear();
    delay(250);
    displayScore(curScoreDisplay, curScore);
    bonusScoreDisplay.clear();
    delay(500);
    bonusScoreDisplay.setSegments(win);
    delay(500);
    displayScore(curScoreDisplay, curScore);
}
