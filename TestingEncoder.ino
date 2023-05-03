#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
#include <avr/wdt.h>


int rightcounter = 0; // count which will determine y position of right paddle
int leftcounter = 0; // count which will determine y position of left paddle
unsigned long last_run_right = 0; // how long ago the last run happened
unsigned long last_run_left = 0;

Adafruit_8x8matrix matrix = Adafruit_8x8matrix();

int left_score = 0; // score for left player
int right_score = 0; // score for right player

int ball_x = 3; // initial x and y position of the ball
int ball_y = 4;
int ball_dir_x = 1; // initial direction of the ball
int ball_dir_y = 0;


void setup() {
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(3), right_shaft_moved, LOW); // attaches the interrupt to pin 3 = right controller (potentiometer)
  attachInterrupt(digitalPinToInterrupt(2), left_shaft_moved, LOW); // attaches the interrupt to pin 2 - left controller (potentiometer)
  matrix.begin(0x70);  // initialize the matrix
  startGame(); // draws a face and then does the transition into starting the game
  matrix.setRotation(1);
}

void loop() {
  matrix.clear();
  lPaddle();
  rPaddle();
  ball();
  checkCollisions();
  matrix.writeDisplay();
  delay(600);
  if (right_score == 10) {
    //Serial.print("Right Wins!!!");
    wdt_enable(WDTO_15MS);
    while(1);
    delay(5000);
  }
  if (left_score == 10) {
    //Serial.print("Left Wins!!!");
    wdt_enable(WDTO_15MS);
    while(1);
    delay(5000);
  }

}

void left_shaft_moved() {
  if (millis()-last_run_left>5) {
    if (digitalRead(5)==0) {
      if (leftcounter < 3) leftcounter ++;
    }
    if (digitalRead(5)==1) {
      if (leftcounter > -3) leftcounter --;
    }
    last_run_left = millis();
  }
}

void right_shaft_moved() { // determines if the right shaft moved, if true then the interrupt will execute
  if (millis()-last_run_right>5) { // debouncer, if the time between the last run and the current run is under 5 millis, then it will be ignored.
    if (digitalRead(4)==0) {
      if (rightcounter < 3) rightcounter ++; // counter goes up 1 when counterclockwise
    }
    if (digitalRead(4)==1) {
      if (rightcounter > -3) rightcounter --; // counter goes down one when clockwise
    }
    last_run_right = millis(); // resets the last run to current millis
  }
}

void rPaddle() { // determines the position of the right paddle based on the right counter
  int right_x1 = 7;
  int right_y1 = 3 + rightcounter;
  int right_x2 = 7;
  int right_y2 = 4 + rightcounter;
  matrix.drawLine(right_x1, right_y1, right_x2, right_y2, LED_ON);
}

void lPaddle() { // determined the position of the left paddle based on the left counter
  int left_x1 = 0;
  int left_y1 = 3 + leftcounter;
  int left_x2 = 0;
  int left_y2 = 4 + leftcounter;
  matrix.drawLine(left_x1, left_y1, left_x2, left_y2, LED_ON);
}

void ball() {
  // Draw ball
  matrix.drawPixel(ball_x, ball_y, LED_ON);

  // Update ball position
  ball_x += ball_dir_x;
  ball_y += ball_dir_y;
} 

void checkCollisions() {
  int y1 = 3 + leftcounter;
  int y2 = 4 + leftcounter;
  int y3 = 3 + rightcounter; 
  int y4 = 4 + rightcounter;

  if (ball_y == 0) {
    ball_dir_y = -ball_dir_y;
  }
  if ( ball_y == 7) {
    ball_dir_y = -ball_dir_y;
  }
  if (ball_x == 1 && (ball_y == y1 || ball_y == y2)) {
    ball_dir_x = -ball_dir_x;
  }
  if (ball_x == 6 && (ball_y == y3 || ball_y == y4)) {
    ball_dir_x = -ball_dir_x;
  }
  if (ball_x == -2) {
    ball_x = 0;
    ball_y = ball_y;
    matrix.drawPixel(ball_x, ball_y, LED_ON);
    right_score++;
    // Serial.print(left_score);
    // Serial.print(" - ");
    // Serial.println(right_score);  
    delay(5000);
    reset();
  }
  if (ball_x == 9) {
    ball_x = 7;
    ball_y = ball_y;
    matrix.drawPixel(ball_x, ball_y, LED_ON);
    left_score++;
    // Serial.print(left_score);
    // Serial.print(" - ");
    // Serial.println(right_score);  
    delay(5000);
    reset();
  }
}

void resetAnim() { //resets animation with LED roll
  matrix.clear(); //clear the display
  matrix.writeDisplay(); //update the display
  delay(100); //pause briefly
  for (int i = 0; i < 8; i++) { //turn on all LEDs in each row
    matrix.drawLine(0, i, 7, i, LED_ON);
    matrix.writeDisplay(); //update the display
    delay(25); //pause briefly
  }
  for (int i = 0; i < 8; i++) { //turn off all LEDs in each row
    matrix.drawLine(0, i, 7, i, LED_OFF);
    matrix.writeDisplay(); //update the display
    delay(25); //pause briefly
  }
}

void startGame() { // sequence that starts the game
  face();
  matrix.writeDisplay();
  delay(3000);
  resetAnim();
  matrix.clear();
}

void reset() { // goes to the next round when someone wins a point
  resetAnim();
  leftcounter = 0;
  rightcounter = 0;
  ball_x = 3;
  ball_y = 4;
}

void face() { // draws a face on the screen :)
  matrix.clear();
  matrix.drawLine(2,0,5,0,LED_ON);
  matrix.drawLine(0,2,0,5,LED_ON);
  matrix.drawLine(2,7,5,7,LED_ON);
  matrix.drawLine(7,2,7,5,LED_ON);
  matrix.drawPixel(1,1,LED_ON);
  matrix.drawPixel(1,6,LED_ON);
  matrix.drawPixel(6,1,LED_ON);
  matrix.drawPixel(6,6,LED_ON);
  matrix.drawPixel(5,2,LED_ON);
  matrix.drawPixel(5,5,LED_ON);
  matrix.drawLine(2,3,2,4,LED_ON);
  matrix.drawPixel(3,2,LED_ON);
  matrix.drawPixel(3,5,LED_ON);
  matrix.writeDisplay();
}

// things to try:
// 1. potentiometer twist/paaddle twist is coupling to ball movement, should be independant
// 2. adjust the ball position

