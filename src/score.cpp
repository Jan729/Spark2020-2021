#include <Arduino.h>
#include "functions.h"
#include "global-variables.h"
#include "TM1637Display.h"

// using 2 four digit 7-segment displays –> one for current score, one for high score
// use TM1637Display library (installed as a .cpp and .h file in the src directory)
// https://github.com/avishorp/TM1637
// CLK and DIO pins need to be connected to Arduino

// pin declarations just for reference – to be removed?
#define curScoreCLK 2
#define curScoreDIO 3

#define highScoreCLK 4
#define highScoreDIO 5

// create new instance of the TM1637Display class by passing clk and dio pin values to it
TM1637Display curScoreDisplay(curScoreCLK, curScoreDIO);
TM1637Display highScoreDisplay(highScoreCLK, highScoreDIO);

//TO DO: fix the error
// display.setBrightness(7); // brightness level from 0 (lowest) to 7 (highest)

void updateScore(int score, int loseGame, int targetDifficulty, int finishTime) {
  // calculate score given based on level difficulty and time taken to complete level
  // level difficulty increases incrementally by 1 after each level
  if (!loseGame){
    targetDifficulty += 1;
    score += 5 * targetDifficulty * (2 - (finishTime/200000)); // score is constant if user completes level after 3 minutes
                                                              // https://www.desmos.com/calculator/xxpl1mwy9a
    displayScore(score);
    sethighScore(score, highscore);
  }
}

void displayScore(int score) {
  curScoreDisplay.clear(); // clear previous value
  delay(500); // delay to make score update more obvious to player
  curScoreDisplay.showNumberDec(score, false); // display new score <- need to test this
}

void sethighScore(int score, int highscore) {
  if (score > highscore) {
    highscore = score;
    highScoreDisplay.clear();
    delay(500);
    highScoreDisplay.showNumberDec(highscore, false);
    highScoreDisplay.clear();
    delay(500); // flash new high score
    highScoreDisplay.showNumberDec(highscore, false);
    highScoreDisplay.clear();
    delay(500);
    highScoreDisplay.showNumberDec(highscore, false);
  }
}
