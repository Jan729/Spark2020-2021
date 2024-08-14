// #define A1PIN 2 // select pin A
// #define B1PIN 3 // select pin B
// #define C1PIN 4 // select pin C
// #define A2PIN 5 // select pin A
// #define B2PIN 6 // select pin B
// #define C2PIN 7 // select pin C
// #define IOPIN A0

// int ir_reading = 0;

// void setup() {
//   pinMode(A1PIN, OUTPUT);
//   pinMode(B1PIN, OUTPUT);
//   pinMode(C1PIN, OUTPUT);

//   pinMode(A2PIN, OUTPUT);
//   pinMode(B2PIN, OUTPUT);
//   pinMode(C2PIN, OUTPUT);
  
//   pinMode(IOPIN, INPUT);
  
//   // signal pins: little endian, A is least significant
//   digitalWrite(A1PIN, LOW);
//   digitalWrite(B1PIN, LOW);
//   digitalWrite(C1PIN, LOW);

//   digitalWrite(A2PIN, HIGH);
//   digitalWrite(B2PIN, LOW);
//   digitalWrite(C2PIN, LOW);
  
//   Serial.begin(9600);
// }

// void loop() {
//   ir_reading = analogRead(IOPIN);
//   Serial.println(ir_reading);
// }

// Define Arduino pins for parent and child MUX controls
const int parentA = 2;
const int parentB = 3;
const int parentC = 4;

const int childS1 = 5;
const int childS2 = 6;
const int childS3 = 7;

const int analogPin = A0; // Analog input to read the selected mux output

void setup() {
  // Set all control pins as outputs
  pinMode(parentA, OUTPUT);
  pinMode(parentB, OUTPUT);
  pinMode(parentC, OUTPUT);

  pinMode(childS1, OUTPUT);
  pinMode(childS2, OUTPUT);
  pinMode(childS3, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  for (int parentChannel = 0; parentChannel < 4; parentChannel++) {
    // Select the child MUX by setting the parent multiplexer
    selectChannel(parentChannel, parentA, parentB, parentC);
    
    for (int childChannel = 0; childChannel < 8; childChannel++) {
      // Select the input of the selected child MUX
      selectChannel(childChannel, childS1, childS2, childS3);

      // Read the value from the selected channel of the selected child mux
      int sensorValue = analogRead(analogPin);

      // Print the results
      Serial.print("Parent MUX Channel: ");
      Serial.print(parentChannel);
      Serial.print(", Child MUX Channel: ");
      Serial.print(childChannel);
      Serial.print(", Sensor Value: ");
      Serial.println(sensorValue);

      delay(100); // Short delay between readings
    }
  }
}

// Function to select a channel on the CD4051
void selectChannel(int channel, int pinA, int pinB, int pinC) {
  digitalWrite(pinA, bitRead(channel, 0));
  digitalWrite(pinB, bitRead(channel, 1));
  digitalWrite(pinC, bitRead(channel, 2));
}