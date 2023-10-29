#include <Arduino.h>
#include "functions.h"
#include "global-variables.h"

void updateLights(int lastHole, int newHole){
  digitalWrite(lastHole, LOW);
  digitalWrite(newHole, HIGH);
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

void flashAllTargetLEDs() {
    for(int i = 15; i <=0 ; i--) {
     mcp3.digitalWrite(i, HIGH);
     delay(100);
     mcp3.digitalWrite(i, LOW);
    }
  
  for(int i = 15; i <=0 ; i--) {
     mcp2.digitalWrite(i, HIGH);
     delay(100);
     mcp2.digitalWrite(i, LOW);
    }
  
  for(int i = 15; i <=0 ; i--) {
     mcp1.digitalWrite(i, HIGH);
     delay(100);
     mcp1.digitalWrite(i, LOW);
    }
}

void displayWinMessage() {
  for (int i=1; i<=3; i++) {
    sevseg1.setChars("y");
    sevseg1.refreshDisplay(); 
    sevseg2.setChars("o");
    sevseg2.refreshDisplay(); 
    sevseg3.setChars("u");
    sevseg3.refreshDisplay(); 
    delay(500);
    sevseg1.setChars("w");
    sevseg1.refreshDisplay(); 
    sevseg2.setChars("i");
    sevseg2.refreshDisplay(); 
    sevseg3.setChars("n");
    sevseg3.refreshDisplay(); 
    delay(500);
  }
}

void displayLoseMessage() {
  for (int i=1; i<=3; i++) {
    sevseg1.setChars("y");
    sevseg1.refreshDisplay(); 
    sevseg2.setChars("o");
    sevseg2.refreshDisplay(); 
    sevseg3.setChars("u");
    sevseg3.refreshDisplay(); 
    delay(500);
    sevseg1.setChars("l");
    sevseg1.refreshDisplay(); 
    sevseg2.setChars("o");
    sevseg2.refreshDisplay(); 
    sevseg3.setChars("-");
    sevseg3.refreshDisplay(); 
    delay(500);
    sevseg1.setChars("e");
    sevseg1.refreshDisplay(); 
    sevseg2.setChars("!");
    sevseg2.refreshDisplay(); 
    sevseg3.setChars("!");
    sevseg3.refreshDisplay(); 
    delay(500);
  }
}
