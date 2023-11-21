#define A1PIN 2 // select pin A
#define B1PIN 3 // select pin B
#define C1PIN 4 // select pin C
#define A2PIN 5 // select pin A
#define B2PIN 6 // select pin B
#define C2PIN 7 // select pin C
#define IOPIN A0

int ir_reading = 0;

void setup() {
  pinMode(A1PIN, OUTPUT);
  pinMode(B1PIN, OUTPUT);
  pinMode(C1PIN, OUTPUT);

  pinMode(A2PIN, OUTPUT);
  pinMode(B2PIN, OUTPUT);
  pinMode(C2PIN, OUTPUT);
  
  pinMode(IOPIN, INPUT);
  
  // signal pins: little endian, A is least significant
  digitalWrite(A1PIN, LOW);
  digitalWrite(B2PIN, LOW);
  digitalWrite(C1PIN, LOW);

  digitalWrite(A2PIN, HIGH);
  digitalWrite(B2PIN, LOW);
  digitalWrite(C2PIN, LOW);
  
  Serial.begin(9600);
}

void loop() {
  ir_reading = analogRead(IOPIN);
  Serial.println(ir_reading);
}
