#define APIN 2 // select pin A
#define BPIN 3 // select pin B
#define CPIN 4 // select pin C
#define IOPIN A0

int ir_reading = 0;

void setup() {
  pinMode(APIN, OUTPUT);
  pinMode(BPIN, OUTPUT);
  pinMode(CPIN, OUTPUT);
  pinMode(IOPIN, INPUT);

  // little endian, A is least significant
  digitalWrite(APIN, HIGH);
  digitalWrite(BPIN, HIGH);
  digitalWrite(CPIN, HIGH);

  Serial.begin(9600);
}

void loop() {
  ir_reading = analogRead(IOPIN);
  Serial.println(ir_reading);
}
