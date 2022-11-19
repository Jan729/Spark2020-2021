// REFERENCES:
// circuit: https://www.youtube.com/watch?v=FgiSbwKplmU&ab_channel=PraveenDehari
// code: https://learn.adafruit.com/ir-breakbeam-sensors/arduino

// PIN DECLARATIONS
int IRSensor = A5; // connect ir sensor to arduino pin A5
int LED = 12; // connect Led to arduino pin 12
int latchPin = 5;  // latch pin of shift register
int clockPin = 6; // clock pin of shift register
int dataPin = 4;  // data pin of shift register

// GLOBAL VARIABLES 
int sensorValue = 0;
int threshold = 200;


void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);

  pinMode(LED, OUTPUT); // led pin as OUTPUT
  pinMode(IRSensor, INPUT); // ir sensor pin as INPUT
  digitalWrite(IRSensor, HIGH); // turn on the pullup
  // shift registers
  pinMode(latchPin, INPUT); // set all the pins of 74HC595 as INPUT
  pinMode(dataPin, INPUT);  
  pinMode(clockPin, INPUT);
}


void loop() {
  // read the analog in value:
  sensorValue = analogRead(IRSensor);

  Serial.print("sensor = " );
  Serial.println(sensorValue);

  if (sensorValue > threshold){
    //turn LED on:
    digitalWrite(LED, HIGH);
  }else{
    digitalWrite(LED, LOW);
  }

}
