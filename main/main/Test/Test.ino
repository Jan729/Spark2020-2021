// REFERENCES:
// circuit: https://www.youtube.com/watch?v=FgiSbwKplmU&ab_channel=PraveenDehari
// code: https://learn.adafruit.com/ir-breakbeam-sensors/arduino

// PIN DECLARATIONS
int IRSensor = A5; // connect ir sensor to arduino pin A5
int LED = 12; // connect Led to arduino pin 12


// GLOBAL VARIABLES 
int sensorValue = 0;
int threshold = 200;


void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);

  pinMode(LED, OUTPUT); // led pin as OUTPUT
  pinMode(IRSensor, INPUT); // ir sensor pin as INPUT
  digitalWrite(IRSensor, HIGH); // turn on the pullup
}


void loop() {
  // read the analog in value:
  sensorValue = analogRead(IRSensor);
  Serial.println(IRSensor);

//  Serial.print("sensor = " );
//  Serial.println(sensorValue);

  if (sensorValue > threshold){
    //turn LED on:
    digitalWrite(LED, HIGH);
  }else{
    digitalWrite(LED, LOW);
  }

}
