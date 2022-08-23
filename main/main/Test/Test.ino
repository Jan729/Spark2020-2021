int IRSensor = A5; // connect ir sensor to arduino pin 2
int LED = 13; // conect Led to arduino pin 13

// youtube: https://www.youtube.com/watch?v=FgiSbwKplmU&ab_channel=PraveenDehari

int sensorValue = 0;

void setup() 
{
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);

  pinMode (LED, OUTPUT); // Led pin OUTPUT
}


void loop(){
  // read the analog in value:
  sensorValue = analogRead(IRSensor);

  Serial.print("sensor = " );
  Serial.println(sensorValue);

  delay(200);

  if(sensorValue > 500){
    digitalWrite(LED,1);
  }else{
    digitalWrite(LED,0);
  }
  
}
