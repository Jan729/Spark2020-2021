int latchPin = 5;    // latch pin of shift register
int clockPin = 6;    // clock pin of shift register
int dataPin = 4;    // data pin of shift register
int LED = 12;

byte leds1 = 0;        // each var controls leds in each shift register

void setup() 
{
  Serial.begin(9600);
  
  // set all the pins of 74HC595 as OUTPUT
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
}


void loop() 
{
  leds1 = 0; //initially turns all the LEDs off
  delay(500);
  int targetLEDPin = 0;   //test LED index variable
  bitSet(leds1,targetLEDPin);   //set an LED value to 1 depending on the register value
  updateShiftRegister();        //update the LEDs
  delay(2000);

}

void updateShiftRegister()
{
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, LSBFIRST, leds1);
   digitalWrite(latchPin, HIGH);
}
