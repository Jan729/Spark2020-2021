//jotted down some ideas. we'll decide what kinds of arguments to pass to the functions soon

/**********CONSTANTS*****************************/

const int scoreIncrease = 10;
const int echoPin = 9;
const int trigPin = 10;
const int numTargetHoles = 8;

/************END OF CONSTANTS*********************/

/************GLOBAL VARIABLES**********************/
//declare any variables here that need to be remembered throughout the entire game

// time for sound wave to travel to object and bounce back
long duration;
// distance between sensor and object
int distance;

bool playingGame = true; //true if someone is playing, false if game is idle

int score = 0;

int targetHoles[numTargetHoles];

/************END OF GLOBAL VARIABLES**********************/
void waitToStartGame() {
  //wiat and do nothing until someone presses "start"
  //then continue with game loop
  Serial.println("wait to start the game. pretend someone starts playing game");
}

void checkIfGameOver () {
//check if user "won" or "lost"
//if user hasn't moved for a while, we assume they walked away and end the game
Serial.println("check if game over");
}

void resetBar() {
  //given current position of bar, reset the bar
  //put the ball back onto the bar
  Serial.println("reset bar");
}

void resetBall() {
  //wait until ball is ready to roll onto the bar
  //put the ball back onto the bar
  Serial.println("reset ball");
}

/******USER INPUT FUNCTIONS ****/
int smooth_distance (int num_samples) {
  int value = 0;
  int current_distance = 0;
  for (int i = 0; i < num_samples; i++) {
    current_distance = sample_distance();
    if (current_distance > 0 && current_distance < 25) {
      value = value + sample_distance();
      //Serial.print("Good Reading.\n");
    } else {
      //Serial.print("Bad Reading.");
      //take another sample of the data if there was a bad read
      //probably going to need to remove this
      //but it works well to test averages
      i--;
    }
    
  }
  
  return (value/num_samples);
}

int sample_distance() {
   int current_duration;
  int current_distance;
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  //wait max 50,000 microseconds before reading
  //may have to change this value
  current_duration = pulseIn(echoPin, HIGH, 50000);
  
  // Calculating the distance
  current_distance = current_duration*0.034/2;
  
  // Prints the distance on the Serial Monitor
  //Serial.print("Distance: ");
  //Serial.println(current_distance);

  return current_distance;
}

/******END OF USER INPUT FUNCTIONS ****/

void moveBar() {
  //get user input, ie "move left side down by X amount"
  
  //control motors to move bar
//Need to check position of bar:
//If bar is too tilted, don't move it
//If bar is at top of game, don't move up anymore
//If bar is at bottom of game, don't move down anymore

}

void ballEntry() {
//  If ball falls into a bad hole
//    End the game

//If ball falls into target hole
//  Increment and display score
//  increment the score
//  Start the next level
//  Choose a new target hole
  
  //TODO for next week:
  //choose an input sensor (IR vs ultrasonic distance vs reed switch vs limit switch)
  //based on sensor, write code to determine if ball got close enough === fell into that hole
  //need 1 sensor for bottom of gameboard: if ball got to bottom of the gameboard, and didn't trigger a target hole, it went into a bad hole
  
  int fellIntoTargetHole = true; //TODO: change to false
  int fellIntoBadHole = false;
  

  if(fellIntoTargetHole) {
    score += 1;
    //choose a new hole
  } else if (fellIntoBadHole) {
    
  }
  
  resetBar();
}


void updateTarget(int prevTarget) {
  //change the new target hole based on the last one
  //decide if we want pre-specified "levels"
  //turn off any lights on the old target hole
  //make the new target hole light up
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // Starts the serial communication
  
  for(int i = 0; i < numTargetHoles; i++) {
    pinMode(i, INPUT); //IR receivers, one pin per target hole
  }
  
  pinMode(8, OUTPUT);

  pinMode(echoPin, INPUT); //echoPin, for distance sensor
  pinMode(trigPin, OUTPUT); //trigggerPin, for distance sensor
  
  pinMode(11, OUTPUT); //IR LEDs. keep on at all times
  pinMode(12, OUTPUT); 
  pinMode(13, INPUT); //IR receiver to detect ball that fell to the bottom of gameboard
  
  //initialize motors
  //initialize user input devices
  //initialize displays

    resetBar();
}

void loop() {
  // put your main code here, to run repeatedly:
  waitToStartGame();
  resetBar();
  resetBall();
  score = 0;
  
  //updateTarget(); //choose 1st target

  //dummy counter for mimicking a game loop
  int dummyCounter = 0;
  
  while(playingGame) {
    //user input: just for testing purposes
    //distance = smooth_distance(20);
    //Serial.print("Distance: ");
    //Serial.println(distance);
  
    moveBar();
    ballEntry();
    checkIfGameOver();

    if(dummyCounter > 10) {
      playingGame = false;
    }
      dummyCounter++;
  }

  resetBar();
  //updateHighScore();
  //turnOffAllLights();
}
