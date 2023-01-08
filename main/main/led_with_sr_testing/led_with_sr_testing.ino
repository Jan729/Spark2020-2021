int latchPin = 5;  // latch pin of shift register
int clockPin = 6; // clock pin of shift register
int dataPin = 4;  // data pin of shift register

byte leds1 = 0;   // each var controls leds in each shift register


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
   shiftOut(dataPin, clockPin, LSBFIRST, leds1);
   digitalWrite(latchPin, HIGH);
}

void loop() 
{
  leds1 = 0;  // initialize leds
  updateShiftRegister();
  delay(2000);

  // turn on a given LED pattern
  int pattern1[] = {0, 1, 2, 3, 4, 5, 6, 7};
  
  for (int i = 0; i < sizeof(pattern1)/sizeof(pattern1[0]); i++){
    bitSet(leds1, pattern1[i]);
    updateShiftRegister();
    delay(100);
  }
 
}
