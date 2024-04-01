int latchPin = 5;  // latch pin of shift register
int clockPin = 6; // clock pin of shift register
int dataPin = 4;  // data pin of shift register

byte leds1 = 0;   // each var controls leds in each shift register
byte leds2 = 0; 


void setup() 
{
  Serial.begin(9600);
  
  // set all the pins of 74HC595 as OUTPUT
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
}

void updateShiftRegister()
{
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, LSBFIRST, leds4);
   shiftOut(dataPin, clockPin, LSBFIRST, leds3);
   shiftOut(dataPin, clockPin, LSBFIRST, leds2);
   shiftOut(dataPin, clockPin, LSBFIRST, leds1);
   digitalWrite(latchPin, HIGH);
}

void loop() 
{
  leds1 = 0;  // initialize leds
  leds2 = 0;
  leds3 = 0;
  leds4 = 0;
  updateShiftRegister();
  delay(2000);
  
  // turn on a given LED pattern
  int pattern1[] = {0, 1, 2, 3, 4, 5, 6, 7};
  int pattern2[] = {0, 1, 2, 3, 4, 5, 6, 7};
  int pattern3[] = {0, 1, 2, 3, 4, 5, 6, 7};
  int pattern4[] = {0, 1, 2, 3, 4, 5, 6, 7};
  
  for (int i = 0; i < sizeof(pattern1)/sizeof(pattern1[0]); i++){
    bitSet(leds1, pattern1[i]);
    updateShiftRegister();
    delay(500);
  }
  
  for (int j = 0; j < sizeof(pattern2)/sizeof(pattern2[0]); j++){
    bitSet(leds2, pattern2[j]);
    updateShiftRegister();
     delay(500);
  }
  
  for (int k = 0; k < sizeof(pattern3)/sizeof(pattern3[0]); k++){
    bitSet(leds3, pattern3[k]);
    updateShiftRegister();
     delay(500);
  }
  
  for (int l = 0; l < sizeof(pattern4)/sizeof(pattern4[0]); l++){
    bitSet(leds4, pattern4[l]);
    updateShiftRegister();
     delay(500);
  }

}
