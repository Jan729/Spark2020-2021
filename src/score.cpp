#include <Arduino.h>
#include "functions.h"
#include "global-variables.h"

void updateScore() { 
  if (!loseGame) {
    targetDifficulty += 1;
  }
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
