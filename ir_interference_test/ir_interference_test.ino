#define IR_EMIT_1 12
#define IR_EMIT_2 13
#define IR_RECV_1 A0
#define IR_RECV_2 A1

int ir_data_1 = 0;
int ir_data_2 = 0;

// connect IR LEDs to the PCB to match the circuit taped to the display
void setup() {
  pinMode(IR_EMIT_1, OUTPUT);
  pinMode(IR_EMIT_2, OUTPUT);
  pinMode(IR_RECV_1, INPUT);
  pinMode(IR_RECV_2, INPUT);

  digitalWrite(IR_EMIT_1, HIGH);
  digitalWrite(IR_EMIT_2, HIGH);

  Serial.begin(9600);
}

void loop() {
  ir_data_1 = analogRead(IR_RECV_1);
  ir_data_2 = analogRead(IR_RECV_2);
  Serial.print("HOLE 1: ");
  Serial.print(ir_data_1);
  Serial.print(" HOLE 2: ");
  Serial.print(ir_data_2);
  Serial.println();
  Serial.println();
}
