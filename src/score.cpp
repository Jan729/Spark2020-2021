#include <Arduino.h>
#include "functions.h"
#include "global-variables.h"
#include "TM1637Display.h"

// using 2 four digit 7-segment displays –> one for current score, one for high score
// use TM1637Display library (installed as a .cpp and .h file in the src directory)
// https://github.com/avishorp/TM1637
// CLK and DIO pins need to be connected to Arduino

// pin declarations just for reference – to be removed?
#define bonusScoreCLK 2
#define curScoreCLK 3
#define curScoreDIO 4

#define bonusScoreDIO 5
#define highScoreCLK 6
#define highScoreDIO 7

// create new instance of the TM1637Display class by passing clk and dio pin values to it
TM1637Display bonusScoreDisplay(bonusScoreCLK, bonusScoreDIO);
TM1637Display curScoreDisplay(curScoreCLK, curScoreDIO);
TM1637Display highScoreDisplay(highScoreCLK, highScoreDIO);

void setHexDisplayBrightness() {
  bonusScoreDisplay.setBrightness(7); // brightness level from 0 (lowest) to 7 (highest)
  curScoreDisplay.setBrightness(7);
  highScoreDisplay.setBrightness(7);
}

void displayBonus(int bonusScore, int targetDifficulty, int finishTime) { // calculate bonus score base on level and time, display onto hex display
  // starting from 100, the max bonus points increments by 100 for each level
  // every 3 seconds, the bonus points decreases by 10, until 0 is reached
  
  if (finishTime % 3 == 0){
    bonusScore -= 10;
  } 
  displayScore(bonusScoreDisplay, bonusScore);
}

void updateScore(int curScore, int bonusScore, int highScore) { // after every round, update current score with bonus score
  curScore += bonusScore;
  displayScore(curScoreDisplay, curScore);
  sethighScore(curScore, highScore); // check if high score has been surpassed
}

void displayScore(TM1637Display display, int score) {
  curScoreDisplay.clear();                     // clear previous value
  delay(500);                                  // delay to make score update more obvious to player
  curScoreDisplay.showNumberDec(score, false); // display new score <- need to test this
}

void resetScores() {
  bonusScoreDisplay.clear();
  curScoreDisplay.clear();
}

void sethighScore(int curScore, int highScore) {
  if (curScore > highScore) {      // if high score is updated, blink new high score 3 times
    highScore = curScore;
    highScoreDisplay.clear();
    delay(500);
    highScoreDisplay.showNumberDec(highScore, false);
    highScoreDisplay.clear();
    delay(500);
    highScoreDisplay.showNumberDec(highScore, false);
    highScoreDisplay.clear();
    delay(500);
    highScoreDisplay.showNumberDec(highScore, false);
  }
}

void displayLoseMessage() { // display "OOPS" if game is lost
  uint8_t oops[] = {
		 SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,  // O
		 SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,  // O
		 SEG_A | SEG_B | SEG_E | SEG_F | SEG_G,          // P
		 SEG_A | SEG_C | SEG_D | SEG_F | SEG_G           // S
		};
    curScoreDisplay.clear();
    curScoreDisplay.setSegments(oops);
}

void displayWinMessage() { // display "WIN" if game is won... a little sketchy but best i can do
    uint8_t win[] = {
		 SEG_C | SEG_D | SEG_E | SEG_F,         // W first half
		 SEG_B | SEG_C | SEG_D | SEG_E,         // W second half
		 SEG_B | SEG_C | SEG_E | SEG_F,        // I + N first half
		 SEG_B | SEG_C | SEG_D | SEG_E | SEG_F  // N second half
		};
    curScoreDisplay.clear();
    curScoreDisplay.setSegments(win);
}
