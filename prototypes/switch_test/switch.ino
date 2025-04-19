const int parentA = 2;
const int parentB = 3;
const int parentC = 4;
const int childS1 = 5;
const int childS2 = 6;
const int childS3 = 7;

// const int limitSwitch_1 = 8; // Limit switch 1
// const int limitSwitch_2 = 9; // Limit switch 2

const int analogPin = A1; // Analog input to read the selected mux output

int parentChannel = 2; //Set to 0-3 to select which Child Multiplexer
int childChannel = 7; //Set to 0-7 to select which IR Mux

int idleState = 1;

void setup() {
  // // put your setup code here, to run once:
  // pinMode(limitSwitch_1, INPUT);
  // pinMode(limitSwitch_2, INPUT);
  
  pinMode(analogPin, INPUT);
  // Set all control pins as outputs
  pinMode(parentA, OUTPUT);
  pinMode(parentB, OUTPUT);
  pinMode(parentC, OUTPUT);

  pinMode(childS1, OUTPUT);
  pinMode(childS2, OUTPUT);
  pinMode(childS3, OUTPUT);
  
  // CHANGE HERE to change the selected button: currently reading from input 1 #2
  digitalWrite(parentA, bitRead(parentChannel, 0));
  digitalWrite(parentB, bitRead(parentChannel, 1));
  digitalWrite(parentC, bitRead(parentChannel, 2));

  digitalWrite(childS1, bitRead(childChannel, 0));
  digitalWrite(childS2, bitRead(childChannel, 1));
  digitalWrite(childS3, bitRead(childChannel, 2));

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  while (idleState) {
    int sensorValue = analogRead(analogPin);
    // Serial.print("Sensor Value: ");
    // Serial.println(sensorValue);
    if (sensorValue > 500) {
      Serial.print("Button is pressed from mux: ");
      Serial.print(parentChannel);
      Serial.print(", input: ");
      Serial.println(childChannel);
      // idleState=0;
    }
  }

}
