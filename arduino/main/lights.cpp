#include <Arduino.h>
#include "functions.h"
#include "global-variables.h"

void writeLed(int ledNum)
{
  for (int i = 0; i < NUM_LEDS; i++) {
    *(ledsArray[i]) = 0;
  }

  int mult = ledNum / 8;
  int mod = ledNum % 8;

  Serial.println("Mult and mod");
  Serial.println(mult);
  Serial.println(mod);

  bitSet(*ledsArray[mult], mod);
  updateShiftRegister();
}

void updateShiftRegister()
{
   digitalWrite(LED_LATCH_PIN, LOW);
   shiftOut(LED_DATA_PIN, LED_CLOCK_PIN, LSBFIRST, leds4);
   shiftOut(LED_DATA_PIN, LED_CLOCK_PIN, LSBFIRST, leds3);
   shiftOut(LED_DATA_PIN, LED_CLOCK_PIN, LSBFIRST, leds2);
   shiftOut(LED_DATA_PIN, LED_CLOCK_PIN, LSBFIRST, leds1);
   digitalWrite(LED_LATCH_PIN, HIGH);
}
