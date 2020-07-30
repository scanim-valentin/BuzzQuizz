/*
  Derivated from initial code created by Geo - 2019
  https://iliketomakestuff.com/make-quiz-game-buttons/#comment-4382622946
  (...)
  Lionel - Science animation for tubecon 2019
  Add NeoMatrix Adafruit drive + led strip ws2812
  3 buttons version
*/

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#include <Fonts/Picopixel.h>

//Define Buttons

//Score increment/decrement buttons
const int scoreIncPin = 2;
const int scoreDecPin = 3;

//Reset Button
const int resetButtonPin = 17;
const int resetledPin  = 13;
boolean resetledState = LOW;
boolean resetButtonState = LOW;

// Correct Answer Button
//const int correctAnswerButtonPin = ??;
//bool correctAnswerButtonState = ??;

// Incorrect Answer Button
//const int incorrectAnswerButtonPin = ??;
//bool incorrectAnswerButtonState = ??;

//Button1 jaune
const int button1Pin = 14;
// const int led1Pin = 5; // !! neomatrix on this pin
const int led1Pin = 9;
boolean led1State = LOW;
boolean button1State = LOW;
int score1 = 0;
boolean activePlayer1 = LOW;

//Button2 vert
const int button2Pin = 15;
//const int led2Pin = 6; // !! neomatrix on this pin
const int led2Pin = 10;
boolean led2State = LOW;
boolean button2State = LOW;
int score2 = 0;
boolean activePlayer2 = LOW;

//Button3 bleu
const int button3Pin = 16;
//const int led3Pin = 7; // !! neomatrix on this pin
const int led3Pin = 11;
boolean led3State = LOW;
boolean button3State = LOW;
int score3 = 0;
boolean activePlayer3 = LOW;

//Buzzer
const int buzzerPin = 8;
boolean buzzerState  = LOW;
int frequency = 440;
boolean pollingForPresses  = LOW;
int whichButtonWasPressed  = 4;
int LoopTimer  = 145;


Adafruit_NeoMatrix matrix1 = Adafruit_NeoMatrix(8, 10, 5,
                             NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
                             NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
                             NEO_GRB            + NEO_KHZ800);

Adafruit_NeoMatrix matrix2 = Adafruit_NeoMatrix(8, 10, 6,
                             NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
                             NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
                             NEO_GRB            + NEO_KHZ800);

Adafruit_NeoMatrix matrix3 = Adafruit_NeoMatrix(8, 10, 7,
                             NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
                             NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
                             NEO_GRB            + NEO_KHZ800);


void setup() {
  Serial.begin(9600);

  matrix1.begin();
  matrix2.begin();
  matrix3.begin();

  matrix1.setTextWrap(false);
  matrix1.setBrightness(20);
  matrix2.setTextWrap(false);
  matrix2.setBrightness(20);
  matrix3.setTextWrap(false);
  matrix3.setBrightness(20);

  matrix1.setTextColor(matrix1.Color(255, 255, 0));
  matrix2.setTextColor(matrix2.Color(0, 255, 0));
  matrix3.setTextColor(matrix2.Color(0, 0, 255));

  matrix1.setFont(&Picopixel);
  matrix2.setFont(&Picopixel);
  matrix3.setFont(&Picopixel);

  // initializes increment and decrement buttons pins as input
  pinMode(scoreIncPin, INPUT_PULLUP);
  pinMode(scoreDecPin, INPUT_PULLUP);

  // initialize the LED pins as an output:
  pinMode(resetledPin, OUTPUT);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led3Pin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  // initialize the pushbutton pins as input:
  pinMode(resetButtonPin, INPUT);
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  pinMode(button3Pin, INPUT);

  // turn off all the LEDs except the reset LED
  digitalWrite(resetledPin, HIGH);
  digitalWrite(led1Pin, LOW);
  digitalWrite(led2Pin, LOW);
  digitalWrite(led3Pin, LOW);

}

void loop() {
  //delay(1000);
  //Serial.println("loop");
  // read the state of the pushbutton values:

  boolean resetButtonState = digitalRead(resetButtonPin);
  boolean button1State = digitalRead(button1Pin);
  boolean button2State = digitalRead(button2Pin);
  boolean button3State = digitalRead(button3Pin);
  boolean buttonIncState = !digitalRead(scoreIncPin);
  boolean buttonDecState = !digitalRead(scoreDecPin);

  // Debugging Code:
  Serial.print(resetButtonState);
  Serial.print(button1State);
  Serial.print(button2State);
  Serial.print(button3State);
  Serial.print(" ");
  Serial.print(resetledState);
  Serial.print(led1State);
  Serial.print(led2State);
  Serial.print(led3State);
  Serial.print(buzzerState);
  Serial.print(pollingForPresses);
  Serial.print(buttonIncState);
  Serial.println(buttonDecState);

  // Are we polling?  (i.e. are we waiting for somebody to press a button?).
  // If so, then check to see if anybody has pressed one on this pass through the code.
  if (pollingForPresses == HIGH) {
    if (button1State == HIGH) {
      whichButtonWasPressed = 1;
    }
    if (button2State == HIGH) {
      whichButtonWasPressed = 2;
    }
    if (button3State == HIGH) {
      whichButtonWasPressed = 3;
    }

    // While polling is on, we ignore the reset button.
    //
  }
  else {
    // Otherwise, Polling is off.  That means one of the 3 player buttons has been pressed.
    // If the resetbutton was pressed, that means that the player has taken his turn and we're
    // starting another round.  In that case, set "whichbuttonwasPressed" to 4.
    // But we only do that if polling is off.
    // (If it's on, the resetbuttonis ignored until somebody presses button 1-3).
    if (buttonIncState == HIGH) {
      whichButtonWasPressed = 5;
    }
    if (buttonDecState == HIGH) {
      whichButtonWasPressed = 6;
    }
    if (resetButtonState == HIGH) {
      whichButtonWasPressed = 4;
    }
    //if (correctAnswerButtonState == HIGH) {whichButtonWasPressed = 5;}
    //if (incorrectAnswerButtonState == HIGH) {whichButtonWasPressed = 6;}
  }

  // Debugging Code
  //  Serial.print(" ");
  //  Serial.print(whichButtonWasPressed);
  //  Serial.println(pollingForPresses);

  //  The switch statement executes one of 4 "case" statements,
  //  depending on the value of whichButtonWasPressed.
  switch (whichButtonWasPressed) {

    case (1):

      // Button1 was pushed.
      // Turn off Polling.   Turn on the buzzer using led 1's frequency.
      // Turn on LED1 & the reset LED - turn all all others off.

      pollingForPresses = LOW;
      buzzerState = HIGH;
      frequency = 880;
      resetledState = HIGH;

      // Switch on Led 1 - Yellow
      led1State = HIGH;

      // Switch off Led 2
      led2State = LOW;

      // Switch off Led 3
      led3State = LOW;

      matrix1.fillScreen(0);
      matrix1.show();
      matrix2.fillScreen(0);
      matrix2.show();
      matrix3.fillScreen(0);
      matrix3.show();

      delay(100);

      matrix1.fillScreen(matrix1.Color(255, 255, 0));
      matrix1.show();

      // Tell the computer which team pushed its button.
      Serial.print("Team ");
      Serial.println(whichButtonWasPressed);
      break;

    case (2):

      // Button2 was pushed.
      // Turn off Polling.   Turn on the buzzer.
      // Turn on LED2 - all others off.

      pollingForPresses = LOW;
      buzzerState = HIGH;
      frequency = 1320;
      //880+440=1320
      resetledState = HIGH;

      // Switch off Led 1 - Yellow
      led1State = LOW;


      // Switch on Led 2 - Green
      led2State = HIGH;

      // Switch off Led 3 - Blue
      led3State = LOW;

      matrix1.fillScreen(0);
      matrix1.show();
      matrix2.fillScreen(0);
      matrix2.show();
      matrix3.fillScreen(0);
      matrix3.show();

      delay(100);

      matrix2.fillScreen(matrix2.Color(0, 255, 0));
      matrix2.show();

      Serial.print("Team ");
      Serial.println(whichButtonWasPressed);
      break;

    case (3):

      // Button3 was pushed.
      // Turn off Polling.   Turn on the buzzer.
      // Turn on LED3 - all others off.
      pollingForPresses = LOW;
      buzzerState = HIGH;
      frequency = 1540;
      // 1320+220=1540
      resetledState = HIGH;

      // Switch off Led 1 - Yellow
      led1State = LOW;

      // Switch off Led 2 - Green
      led2State = LOW;

      // Switch on Led 3 - Blue
      led3State = HIGH;


      matrix1.fillScreen(0);
      matrix1.show();
      matrix2.fillScreen(0);
      matrix2.show();
      matrix3.fillScreen(0);
      matrix3.show();

      delay(100);

      matrix3.fillScreen(matrix2.Color(0, 0, 255));
      matrix3.show();

      Serial.print("Team ");
      Serial.println(whichButtonWasPressed);

      break;


    case (4):
      // The reset button (Button4) was pushed.
      // Turn off all LEDs and the buzzer.
      // Turn Polling back ON.
      // Set zero to all scores

      pollingForPresses = HIGH;
      buzzerState = LOW;
      resetledState = LOW;

      led1State = LOW;

      led2State = LOW;

      led3State = LOW;

      matrix1.fillScreen(0);
      matrix1.show();
      matrix2.fillScreen(0);
      matrix2.show();
      matrix3.fillScreen(0);
      matrix3.show();

      delay(100);

      matrix1.setCursor(1, 6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
      matrix1.print(score1);
      matrix1.show();

      matrix2.setCursor(1, 6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
      matrix2.print(score2);
      matrix2.show();

      matrix3.setCursor(1, 6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
      matrix3.print(score3);
      matrix3.show();


      Serial.println("Reset");

      break;

    case (5):
      // The score increment button (Button5) was pushed.
      // If Polling is off
      // Turn off all LEDs and the buzzer.
      // Turn Polling back ON.
      // Increment the score of the player

      if (pollingForPresses == 0) {

        if (led1State == HIGH) {
          score1++;
        }
        if (led2State == HIGH) {
          score2++;
        }
        if(led3State == HIGH) {
          score3++;
        }

        pollingForPresses = HIGH;
        buzzerState = LOW;
        resetledState = LOW;

        led1State = LOW;

        led2State = LOW;

        led3State = LOW;

        matrix1.fillScreen(0);
        matrix1.show();
        matrix2.fillScreen(0);
        matrix2.show();
        matrix3.fillScreen(0);
        matrix3.show();

        delay(100);

        matrix1.setCursor(1, 6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
        matrix1.print(score1);
        matrix1.show();

        matrix2.setCursor(1, 6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
        matrix2.print(score2);
        matrix2.show();

        matrix3.setCursor(1, 6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
        matrix3.print(score3);
        matrix3.show();


        Serial.println("+1");
      }
      break;

    case (6):
      // The score increment button (Button5) was pushed.
      // If Polling is off
      // Turn off all LEDs and the buzzer.
      // Turn Polling back ON.
      // Decrement the score of the player

      if (pollingForPresses == 0) {

        if (led1State == HIGH) {
          if(score1>0){score1--;}
        }
        if (led2State == HIGH) {
          if(score2>0){score2--;}
        }
        if(led3State == HIGH) {
          if(score3>0){score3--;};
        }

        pollingForPresses = HIGH;
        buzzerState = LOW;
        resetledState = LOW;

        led1State = LOW;

        led2State = LOW;

        led3State = LOW;

        matrix1.fillScreen(0);
        matrix1.show();
        matrix2.fillScreen(0);
        matrix2.show();
        matrix3.fillScreen(0);
        matrix3.show();

        delay(100);

        matrix1.setCursor(1, 6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
        matrix1.print(score1);
        matrix1.show();

        matrix2.setCursor(1, 6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
        matrix2.print(score2);
        matrix2.show();

        matrix3.setCursor(1, 6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
        matrix3.print(score3);
        matrix3.show();


        Serial.println("-1");
      }
      break;
    //  That's the end of the case statement.

    default:
      break;
  }

  // zero out which button was pressed for next loop
  whichButtonWasPressed = 0;

  // DEBUGGING CODE
  // Serial.print(" ");
  // Serial.print(whichButtonWasPressed);
  // Serial.print(pollingForPresses);
  // Serial.print(" ");
  // Serial.print(resetledState);
  // Serial.print(led1State);
  // Serial.print(led2State);
  // Serial.print(led3State);
  // Serial.println(buzzerState);

  // Now update the arduino with the new LED values.
  digitalWrite(resetledPin, resetledState);
  digitalWrite(led3Pin, led3State);
  digitalWrite(led2Pin, led2State);
  digitalWrite(led1Pin, led1State);

  // if the buzzerState was turned on, Sound the buzzer for a half second & turn it off.
  if (buzzerState == 1) {
    tone(buzzerPin, frequency, 500);
    beep();
    buzzerState = 0;
  }
  //  That's the end of the loop.
}

void beep() {
  //digitalWrite(buzzerPin, HIGH);
  //delay(100);
  //digitalWrite(buzzerPin, LOW);
}
