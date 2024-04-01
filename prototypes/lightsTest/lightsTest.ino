void updateTarget() {
  level++; 
  randomSeed(analogRead(0));

  int targetIncr = (int)random(1,3);

  int oldTarget = targetLEDPin;
  
  targetLEDPin += targetIncr;
  targetSensorPin += targetIncr;

  if(targetLEDPin > NUMTARGETS){
    playingGame = false;
    winGame = true; //win the game
  }

  updateLights(oldTarget, targetLEDPin);
}

/************ START OF OUTPUT FUNCTIONS ***********/
void updateLights(int lastHole, int newHole){
  digitalWrite(lastHole, LOW)
  digitalWrite(newHole, HIGH)
}

void flashAllTargetLEDs() {
    //TODO Matt: index target LEDs from top to bottom
    // and uncomment this loop
    for(int i = 15; i <=0 ; i--) {
     mcp3.digitalWrite(i, HIGH);
     delay(100);
     mcp4.digitalWrite(i, LOW);
    }
  
  for(int i = 15; i <=0 ; i--) {
     mcp2.digitalWrite(i, HIGH);
     delay(100);
     mcp2.digitalWrite(i, LOW);
    }
  
  for(int i = 15; i <=0 ; i--) {
     mcp1.digitalWrite(i, HIGH);
     delay(100);
     mcp1.digitalWrite(i, LOW);
    }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // Starts the serial communication

  mcp1.begin(0);
  mcp2.begin(1);
  mcp3.begin(2);
  mcp4.begin(3);
  mcp5.begin(4);
  mcp6.begin(5);
  mcp7.begin(6);
  mcp8.begin(7);
  
  for (int i=0; i<16; i++) {
    mcp1.pinMode(i, INPUT);
    mcp1.pullUp(i, HIGH);
    mcp2.pinMode(i, INPUT);
    mcp2.pullUp(i, HIGH);
    mcp3.pinMode(i, INPUT);
    mcp3.pullUp(i, HIGH);
    mcp4.pinMode(i, INPUT);
    mcp4.pullUp(i, HIGH);
    mcp5.pinMode(i, INPUT);
    mcp5.pullUp(i, HIGH);
    mcp6.pinMode(i, INPUT);
    mcp6.pullUp(i, HIGH);
    mcp7.pinMode(i, INPUT);
    mcp7.pullUp(i, HIGH);
    mcp8.pinMode(i, INPUT);
    mcp8.pullUp(i, HIGH);
  }
  
  for (int i = IROFFSET; i < NUMTARGETS+IROFFSET; i++) {
    pinMode(i, INPUT); //IR receivers, one pin per target hole
    digitalWrite(i, HIGH); // turn on the pullup
    targetHoles[i] = i;
  }

  // Game Input Buttons to Move Bar 
  pinMode(left_button, INPUT);
  pinMode(right_button, INPUT);

  pinMode(11, OUTPUT); //IR LEDs. keep on at all times
  pinMode(12, OUTPUT);
  pinMode(BOTTOMPIN, INPUT); //IR receivers for all bad holes

  //fill targetHoles array with pin numbers

  //set up seven seg display x3
  byte numDigits = 1;
  byte digitPins[] = {};
  byte segmentPins1[] = {6, 5, 2, 3, 4, 7, 8, 9};
  byte segmentPins2[] = {6, 5, 2, 3, 4, 7, 8, 9};
  byte segmentPins3[] = {6, 5, 2, 3, 4, 7, 8, 9};
  bool resistorsOnSegments = true;

  byte hardwareConfig = COMMON_ANODE; 
  sevseg1.begin(hardwareConfig, numDigits, digitPins, segmentPins1, resistorsOnSegments);
  sevseg1.setBrightness(90);
  sevseg2.begin(hardwareConfig, numDigits, digitPins, segmentPins2, resistorsOnSegments);
  sevseg2.setBrightness(90);
  sevseg3.begin(hardwareConfig, numDigits, digitPins, segmentPins3, resistorsOnSegments);
  sevseg3.setBrightness(90);
  
  sevseg1.setNumber(0);
  sevseg1.refreshDisplay(); 
  sevseg2.setNumber(0);
  sevseg2.refreshDisplay(); 
  sevseg3.setNumber(0);
  sevseg3.refreshDisplay(); 

  //reset bar stepper motors
  barPosL = FLOOR;
  barPosR = FLOOR;
  motorR.setSpeed(MAX_SPEED);
  motorL.setSpeed(MAX_SPEED);

  //set up two gpio expanders
  //chip 1 address: 0x20
  //chip 2 address: 0x27
 Wire.begin(); //wake up I2C bus
  // set I/O pins for chip 0x20 to outputs
 Wire.beginTransmission(0x20);
 Wire.write(0x00); // IODIRA register
 Wire.write(0x00); // set all of port A to outputs
 Wire.endTransmission();
 Wire.beginTransmission(0x20);
 Wire.write(0x01); // IODIRB register
 Wire.write(0x00); // set all of port B to outputs
 Wire.endTransmission();

 // set I/O pins for chip 0x27 to outputs
 Wire.beginTransmission(0x27);
 Wire.write(0x00); // IODIRA register
 Wire.write(0x00); // set all of port A to outputs
 Wire.endTransmission();
 Wire.beginTransmission(0x27);
 Wire.write(0x01); // IODIRB register
 Wire.write(0x00); // set all of port B to outputs
 Wire.endTransmission();
  
  resetBar();
}

void loop() {
    updateTarget();
    updateLights()
