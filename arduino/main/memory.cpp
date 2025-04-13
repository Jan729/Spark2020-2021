#include "global-variables.h"
#include "EEPROM.h"
#include <AccelStepper.h>
#define HI_SCORE_ADDR_LOWER 0
#define R_BAR_POS_ADDR_LOWER 2
#define L_BAR_POS_ADDR_LOWER 6

// helper functions to store integers in non-volatile Memory

// WARNING: these functions are SLOW

void writeIntIntoEEPROM(int address, int number)
{ 
  EEPROM.write(address, number >> 8);
  EEPROM.write(address + 1, number & 0xFF);
}

int readIntFromEEPROM(int address)
{
  return (EEPROM.read(address) << 8) + EEPROM.read(address + 1);
}

long readLongFromEEPROM(long address) {
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);
  
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void writeLongIntoEEPROM(int address, long value) {
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);
  
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

int getHighScoreFromEEPROM() {
    return readIntFromEEPROM(HI_SCORE_ADDR_LOWER);
}

void writeHighScoreToEEPROM() {
    writeIntIntoEEPROM(HI_SCORE_ADDR_LOWER, highScore);
}

void writeBarPositionsIntoEEPROM() {
    writeLongIntoEEPROM(L_BAR_POS_ADDR_LOWER, barPosL);
    writeLongIntoEEPROM(R_BAR_POS_ADDR_LOWER, barPosR);
}

void setBarPositionsFromEEPROM() {
    motorL.setCurrentPosition(readLongFromEEPROM(L_BAR_POS_ADDR_LOWER));
    motorR.setCurrentPosition(readLongFromEEPROM(R_BAR_POS_ADDR_LOWER));
}