/**********HELPER FUNCTION PROTOTYPES******************/
void waitToStartGame();
void resetAllVariables();
void incrementLevel();
void resetBall();
void resetGame();
bool checkPassingTime();
bool beamBroken();
void pollIRSensors();
void pollBarJoysticks();
void moveBar();
void moveBarDown();
void resetBarAndBall();
void updateLights(int lastHole, int newHole);
void updateScore(int score, int loseGame, int targetDifficulty, int finishTime);
void displayScore(int score);
void sethighScore(int score, int highscore);
void flashAllTargetLEDs();
void displayWinMessage();
void displayLoseMessage();
/******END OF HELPER FUNCTION PROTOTYPES***************/
