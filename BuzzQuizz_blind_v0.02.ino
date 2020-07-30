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

// LCD Display Libraries
#include "Wire.h"
#include <Deuligne.h>

// initialize the library with the numbers of the interface pins
Deuligne lcd;

// Define Buttons

// Score increment/decrement buttons
const int scoreIncPin = 2;
const int scoreDecPin = 3;

// Reset Button
const int resetButtonPin = 17;
const int resetledPin = 13;
boolean resetledState = LOW;
boolean resetButtonState = LOW;
boolean buttonIncState;
boolean buttonDecState;

// To avoid maintaining the reset button pressed
// boolean reset = 0;
boolean prereset = 0;

// Button1 jaune
const int button1Pin = 14;
// const int led1Pin = 5; // !! neomatrix on this pin
const int led1Pin = 9;
boolean led1State = LOW;
boolean button1State = LOW;
int score1 = 0;
boolean activePlayer1 = LOW;

// Button2 vert
const int button2Pin = 15;
// const int led2Pin = 6; // !! neomatrix on this pin
const int led2Pin = 10;
boolean led2State = LOW;
boolean button2State = LOW;
int score2 = 0;
boolean activePlayer2 = LOW;

// Button3 bleu
const int button3Pin = 16;
// const int led3Pin = 7; // !! neomatrix on this pin
const int led3Pin = 11;
boolean led3State = LOW;
boolean button3State = LOW;
int score3 = 0;
boolean activePlayer3 = LOW;

// Buzzer
const int buzzerPin = 8;
boolean buzzerState = LOW;
int frequency = 440;
boolean pollingForPresses = LOW;
int whichButtonWasPressed = 4;
int LoopTimer = 145;

// Timer
const int defaultTimer = 30 * 1000; // Default time limit
int currentTime;

// Navigating through the menu : 3 states, 2 substates
char state = 'M'; // state can be equal to M(enu), T(ournament), Q(uick game)
boolean substate = 0; // meaning can vary through the different states
boolean presubstate = 0;

// Matrix animation
int matrixStep = 1;

// Podium
char podium[] = {0, 0, 0};

Adafruit_NeoMatrix matrix1 = Adafruit_NeoMatrix(
    8, 10, 5,
    NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
    NEO_GRB + NEO_KHZ800);

Adafruit_NeoMatrix matrix2 = Adafruit_NeoMatrix(
    8, 10, 6,
    NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
    NEO_GRB + NEO_KHZ800);

Adafruit_NeoMatrix matrix3 = Adafruit_NeoMatrix(
    8, 10, 7,
    NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
    NEO_GRB + NEO_KHZ800);

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

  // LCD Menu
  lcd.init();
  lcd.setCursor(0, 0);
  lcd.print("Ready for some");
  lcd.setCursor(0, 1);
  lcd.print("quizz?");
  delay(2000);
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print("> Tournament");
  lcd.setCursor(0, 1);
  lcd.print("  Quick Game");

  currentTime = millis();
  matrixFillScreen();
}

void loop() {

  // delay(1000);
  // Serial.println("loop");
  // read the state of the pushbutton values:

  resetButtonState = digitalRead(resetButtonPin);
  button1State = digitalRead(button1Pin);
  button2State = digitalRead(button2Pin);
  button3State = digitalRead(button3Pin);
  buttonIncState = !digitalRead(scoreIncPin);
  buttonDecState = !digitalRead(scoreDecPin);
  lightMenu();

  // Tournament
  if (state == 'T') {
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print(" - Tournament - "); // First player to get 5 points win the game
    lcd.setCursor(0, 1);
    lcd.print("Round 1: To Five"); //
    delay(500);
    to5();

    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("Round 2: Duel   "); //
    delay(500);
    duel();
    delay(3000);
    state = 'M';
    substate = 0;
    presubstate = 1;
  }

  // Quick game
  if (state == 'Q') { // Quick game

    if (substate == 0 && presubstate == 1) {
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("> Getting to 5"); // First player to get 5 points win the game
      lcd.setCursor(0, 1);
      lcd.print("  Duel"); //
      presubstate = substate;
    }

    if (substate == 1 && presubstate == 0) {
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("  Getting to 5");
      lcd.setCursor(0, 1);
      lcd.print("> Duel");
      presubstate = substate;
    }

    if (resetButtonState && ((millis() - currentTime) >
                             1000)) { // Using the reset button as select button

      if (substate == 1) { // Duel

        duel();
        state = 'M';
        substate == 0;
        presubstate == 1;
      }

      else { // Winning at 5

        to5();
        state = 'M';
        presubstate = 1;
      }
    }

    if (buttonIncState &&
        substate) { // If + button is pressed and Getting to 5 is selected
      substate = 0;
    }

    if (buttonDecState &&
        !substate) { // If - button is pressed and Duel is selected
      substate = 1;
    }
  }

  // Menu
  if (state == 'M') {

    // Menu State
    if (substate == 0 &&
        presubstate == 1) { // To avoid continuous refreshing of the screen
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("> Tournament");
      lcd.setCursor(0, 1);
      lcd.print("  Quick Game");

      presubstate = substate;
    }
    if (substate == 1 && presubstate == 0) {
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("  Tournament");
      lcd.setCursor(0, 1);
      lcd.print("> Quick Game");

      presubstate = substate;
    }

    if (resetButtonState &&
        (millis() >
         currentTime + 1000)) { // Using the reset button as select button

      if (substate) {

        state = 'Q';
        substate = 0;
        presubstate = 1;
        currentTime = millis();

      } else {

        state = 'T';
        presubstate = 1;
      }
    }

    if (buttonIncState && substate) { // If + button is pressed and Quick Game
                                      // is selected, go to Tournament

      substate = 0;
    }

    if (buttonDecState && !substate) { // If - button is pressed and Tournament
                                       // is selected, go to Quick Game

      substate = 1;
    }
  }
}

void beep() {
  // digitalWrite(buzzerPin, HIGH);
  // delay(100);
  // digitalWrite(buzzerPin, LOW);
}

void beep_answer(
    boolean answer) { // Plays a sound if the answer is true or false.
  if (answer) {

    tone(buzzerPin, 880, 200);
    tone(buzzerPin, 1760, 200);

  } else {

    tone(buzzerPin, 880, 200);
    tone(buzzerPin, 500, 200);
  }
}

void matrixFillScreen() {
  matrix1.fillScreen(0);
  matrix1.show();
  matrix2.fillScreen(0);
  matrix2.show();
  matrix3.fillScreen(0);
  matrix3.show();

  delay(100);

  matrix3.fillScreen(matrix2.Color(0, 0, 255));
  matrix2.fillScreen(matrix2.Color(0, 255, 0));
  matrix1.fillScreen(matrix2.Color(255, 255, 0));
  matrix1.show();
  matrix2.show();
  matrix3.show();
}

void lightMenu() {

  if (millis() % 500 == 0) {
    if (matrixStep == 1) {

      matrix3.fillScreen(matrix2.Color(0, 0, 255));
      matrix2.fillScreen(0);
      matrix1.fillScreen(0);
      matrix1.show();
      matrix2.show();
      matrix3.show();
    }
    if (matrixStep == 2) {

      matrix3.fillScreen(0);
      matrix2.fillScreen(matrix2.Color(0, 255, 0));
      matrix1.fillScreen(0);
      matrix1.show();
      matrix2.show();
      matrix3.show();
    }
    if (matrixStep == 3) {

      matrix3.fillScreen(0);
      matrix2.fillScreen(0);
      matrix1.fillScreen(matrix2.Color(255, 255, 0));
      matrix1.show();
      matrix2.show();
      matrix3.show();
    }

    matrixStep = (matrixStep % 3) + 1;
  }
}

void addToPodium(char Player) { // Adds a player (Y,G or B) to the last place
                                // available on the podium

  for (int k = 0; k < 3; k++) {

    if (podium[k] == 0) {

      podium[k] = Player;
    }
  }
}

boolean isInPodium(char Player) { // Checks if player is in podium

  boolean R = 0;
  for (int k = 0; k < 3; k++) {

    R = R || (podium[k] == Player);
  }
  return R;
}

void to5() {

  boolean quit = 0;

  while (!quit) {
    // delay(1000);
    // Serial.println("loop");
    // read the state of the pushbutton values:

    resetButtonState = digitalRead(resetButtonPin);
    if (!isInPodium('Y')) {
      button1State = digitalRead(button1Pin);
    }
    if (!isInPodium('G')) {
      button2State = digitalRead(button2Pin);
    }
    if (!isInPodium('B')) {
      button3State = digitalRead(button3Pin);
    }
    buttonIncState = !digitalRead(scoreIncPin);
    buttonDecState = !digitalRead(scoreDecPin);

    // Debugging Code:
    //  Serial.print(resetButtonState);
    //  Serial.print(button1State);
    //  Serial.print(button2State);
    //  Serial.print(button3State);
    //  Serial.print(" ");
    //  Serial.print(resetledState);
    //  Serial.print(led1State);
    //  Serial.print(led2State);
    //  Serial.print(led3State);
    //  Serial.print(buzzerState);
    //  Serial.print(pollingForPresses);
    //  Serial.print(buttonIncState);
    //  Serial.println(buttonDecState);
    Serial.println(prereset);

    // Are we polling?  (i.e. are we waiting for somebody to press a button?).
    // If so, then check to see if anybody has pressed one on this pass through
    // the code.

    if (resetButtonState == HIGH && (millis() == currentTime + 1000)) {
      quit = 0;
      state = 'M';
      substate = 0;
      presubstate = 1;
    }

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
    } else {
      // Otherwise, Polling is off.  That means one of the 3 player buttons has
      // been pressed. If the resetbutton was pressed, that means that the
      // player has taken his turn and we're starting another round.  In that
      // case, set "whichbuttonwasPressed" to 4. But we only do that if polling
      // is off. (If it's on, the resetbuttonis ignored until somebody presses
      // button 1-3).
      if (buttonIncState == HIGH) {
        whichButtonWasPressed = 5;
      }
      if (buttonDecState == HIGH) {
        whichButtonWasPressed = 6;
      }

      // if (correctAnswerButtonState == HIGH) {whichButtonWasPressed = 5;}
      // if (incorrectAnswerButtonState == HIGH) {whichButtonWasPressed = 6;}
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
      // 880+440=1320
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

      //      Serial.print("Team ");
      //      Serial.println(whichButtonWasPressed);
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
      // IF IT WASNT PUSHED AT THE LAST ITTERATION ie if it's not maintained
      // pushed Turn off all LEDs and the buzzer. Turn Polling back ON. Set zero
      // to all scores
      if (prereset == 0) {

        tone(buzzerPin, 1760, 500);
        tone(buzzerPin, 1000, 500);
        tone(buzzerPin, 800, 500);
        tone(buzzerPin, 1000, 500);

        score1 = 0;
        score2 = 0;
        score3 = 0;
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

        matrix1.setCursor(
            1,
            6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
        matrix1.print(score1);
        matrix1.show();

        matrix2.setCursor(
            1,
            6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
        matrix2.print(score2);
        matrix2.show();

        matrix3.setCursor(
            1,
            6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
        matrix3.print(score3);
        matrix3.show();
        prereset = 1;

        Serial.println("Reset");
      }

      break;

    case (5):
      // The score increment button (Button5) was pushed.
      // If Polling is off
      // Turn off all LEDs and the buzzer.
      // Turn Polling back ON.
      // Increment the score of the player

      beep_answer(1);
      if (led1State == HIGH) {
        score1++;
      }
      if (led2State == HIGH) {
        score2++;
      }
      if (led3State == HIGH) {
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

      matrix1.setCursor(
          1,
          6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
      matrix1.print(score1);
      matrix1.show();

      matrix2.setCursor(
          1,
          6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
      matrix2.print(score2);
      matrix2.show();

      matrix3.setCursor(
          1,
          6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
      matrix3.print(score3);
      matrix3.show();

      Serial.println("+1");
      break;

    case (6):
      // The score decrement button (Button6) was pushed.
      // If Polling is off
      // Turn off all LEDs and the buzzer.
      // Turn Polling back ON.
      // Decrement the score of the player

      beep_answer(0);
      if (led1State == HIGH) {
        if (score1 > 0) {
          score1--;
        }
      }
      if (led2State == HIGH) {
        if (score2 > 0) {
          score2--;
        }
      }
      if (led3State == HIGH) {
        if (score3 > 0) {
          score3--;
        };
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

      matrix1.setCursor(
          1,
          6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
      matrix1.print(score1);
      matrix1.show();

      matrix2.setCursor(
          1,
          6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
      matrix2.print(score2);
      matrix2.show();

      matrix3.setCursor(
          1,
          6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
      matrix3.print(score3);
      matrix3.show();

      Serial.println("-1");
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

    // if the buzzerState was turned on, Sound the buzzer for a half second &
    // turn it off.
    if (buzzerState == 1) {
      tone(buzzerPin, frequency, 500);
      beep();
      buzzerState = 0;
    }
    //  That's the end of the loop.

    if (score1 == 5) {

      addToPodium('Y');
    }
    if (score2 == 5) {

      addToPodium('G');
    }
    if (score3 == 5) {

      addToPodium('B');
    }
    if (score1 == 5 && score2 == 5 && score3 == 5) {

      quit = 1;
    }
  }

  matrix1.fillScreen(0);
  matrix1.show();
  matrix2.fillScreen(0);
  matrix2.show();
  matrix3.fillScreen(0);
  matrix3.show();

  for (int k = 1; k < 4; k++) { // Displaying the rank

    if (podium[k] == 'Y') {

      matrix1.print(k);
      matrix1.show();

    } else if (podium[k] == 'G') {

      matrix2.print(k);
      matrix2.show();

    } else {

      matrix3.print(k);
      matrix3.show();
    }
  }

  if (podium[2] == 'Y') {
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("Yellow is out!  ");
  } else if (podium[2] == 'G') {
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("Green is out!   ");
  }

  else {
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("Blue is out!   ");
  }

  delay(2000);

  matrix1.fillScreen(0);
  matrix1.show();
  matrix2.fillScreen(0);
  matrix2.show();
  matrix3.fillScreen(0);
  matrix3.show();
}

void duelObsolete() {

  int timer = defaultTimer;
  boolean quit = 0;
  currentTime = millis();
  int updateTime = 0; // To avoid continuous refreshing of the screen
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  while (!quit) {

    // Timer \/ \/ \/ \/ \/ \/ \/ \/ \/

    if (pollingForPresses) {
      if (updateTime < 0) {
        Serial.print(millis());
        Serial.print(" ");
        Serial.print(currentTime);
        Serial.print(" ");
        lcd.setCursor(0, 0);
        lcd.print("Time's up");
        tone(buzzerPin, 1760, 200);
        tone(buzzerPin, 880, 200);
        tone(buzzerPin, 1760, 200);
        tone(buzzerPin, 880, 200);

        quit = 1;

        delay(3000);

      } else {

        if (millis() % 1000 == 0) {
          tone(buzzerPin, 880, 200);
          lcd.setCursor(0, 0);
          lcd.print("          ");
          lcd.setCursor(0, 0);
          updateTime = (timer - (millis() - currentTime));
          //          Serial.print(timer);
          //          Serial.print(" = timer, ");
          //          Serial.print(currentTime);
          //          Serial.print(" = currentTime ");
          //          Serial.print(updateTime);
          //          Serial.print(millis() - currentTime);
          //          Serial.print(" = millis() - currentTime ");
          //          Serial.print(updateTime);
          //          Serial.println(" = updateTime ");
          lcd.print(updateTime / 1000);
        }
      }
    } else {

      currentTime = millis();
    }
    //Timer /\ /\ /\ /\ /\ /\ /\ /\ /\


    resetButtonState = digitalRead(resetButtonPin);
    button1State = digitalRead(button1Pin);
    button3State = digitalRead(button3Pin);
    buttonIncState = !digitalRead(scoreIncPin);
    buttonDecState = !digitalRead(scoreDecPin);

    Serial.print(currentTime);
    if (resetButtonState == HIGH && ((millis() - currentTime) > 1000)) {
      quit = 1;
      substate = 0;
      presubstate = 1;
      state = 'M';
    }

    if (pollingForPresses == HIGH) {
      if (button1State == HIGH) {
        whichButtonWasPressed = 1;
      }

      if (button3State == HIGH) {
        whichButtonWasPressed = 3;
      }

      // While polling is on, we ignore the reset button.
      //
    } else {
      // Otherwise, Polling is off.  That means one of the 3 player buttons has
      // been pressed. If the resetbutton was pressed, that means that the
      // player has taken his turn and we're starting another round.  In that
      // case, set "whichbuttonwasPressed" to 4. But we only do that if polling
      // is off. (If it's on, the resetbuttonis ignored until somebody presses
      // button 1-3).
      if (buttonIncState == HIGH) {
        whichButtonWasPressed = 5;
      }
      if (buttonDecState == HIGH) {
        whichButtonWasPressed = 6;
      }

      // if (correctAnswerButtonState == HIGH) {whichButtonWasPressed = 5;}
      // if (incorrectAnswerButtonState == HIGH) {whichButtonWasPressed = 6;}
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

      timer = updateTime;
      pollingForPresses = LOW;
      buzzerState = HIGH;
      frequency = 880;
      resetledState = HIGH;

      // Switch on Led 1 - Yellow
      led1State = HIGH;

      // Switch off Led 3
      led3State = LOW;

      matrix1.fillScreen(0);
      matrix1.show();
      matrix3.fillScreen(0);
      matrix3.show();

      delay(100);

      matrix1.fillScreen(matrix1.Color(255, 255, 0));
      matrix1.show();

      // Tell the computer which team pushed its button.
      // Serial.print("Team ");
      // Serial.println(whichButtonWasPressed);
      break;

    case (3):

      // Button3 was pushed.
      // Turn off Polling.   Turn on the buzzer.
      // Turn on LED3 - all others off.
      timer = updateTime;
      pollingForPresses = LOW;
      buzzerState = HIGH;
      frequency = 1540;
      // 1320+220=1540
      resetledState = HIGH;

      // Switch off Led 1 - Yellow
      led1State = LOW;

      // Switch on Led 3 - Blue
      led3State = HIGH;

      matrix1.fillScreen(0);
      matrix1.show();
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
      // IF IT WASNT PUSHED AT THE LAST ITTERATION ie if it's not maintained
      // pushed Turn off all LEDs and the buzzer. Turn Polling back ON. Set zero
      // to all scores
      if (prereset == 0) {

        tone(buzzerPin, 1760, 500);
        tone(buzzerPin, 1000, 500);
        tone(buzzerPin, 800, 500);

        score1 = 0;
        score3 = 0;
        pollingForPresses = HIGH;
        buzzerState = LOW;
        resetledState = LOW;

        led1State = LOW;

        led3State = LOW;

        matrix1.fillScreen(0);
        matrix1.show();

        matrix3.fillScreen(0);
        matrix3.show();

        delay(100);

        matrix1.setCursor(
            1,
            6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
        matrix1.print(score1);
        matrix1.show();

        matrix3.setCursor(
            1,
            6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
        matrix3.print(score3);
        matrix3.show();
        prereset = 1;

        Serial.println("Reset");
      }

      break;

    case (5):
      // The score increment button (Button5) was pushed.
      // If Polling is off
      // Turn off all LEDs and the buzzer.
      // Turn Polling back ON.
      // Increment the score of the player

      beep_answer(1);
      if (led1State == HIGH) {
        score1++;
      }

      if (led3State == HIGH) {
        score3++;
      }

      pollingForPresses = HIGH;
      buzzerState = LOW;
      resetledState = LOW;

      led1State = LOW;

      led3State = LOW;

      matrix1.fillScreen(0);
      matrix1.show();

      matrix3.fillScreen(0);
      matrix3.show();

      delay(100);

      matrix1.setCursor(
          1,
          6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
      matrix1.print(score1);
      matrix1.show();

      matrix3.setCursor(
          1,
          6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
      matrix3.print(score3);
      matrix3.show();

      // Serial.println("+1");
      break;

    case (6):
      // The score decrement button (Button6) was pushed.
      // If Polling is off
      // Turn off all LEDs and the buzzer.
      // Turn Polling back ON.
      // Decrement the score of the player

      beep_answer(0);
      if (led1State == HIGH) {
        if (score1 > 0) {
          score1--;
        }
      }

      if (led3State == HIGH) {
        if (score3 > 0) {
          score3--;
        };
      }

      pollingForPresses = HIGH;
      buzzerState = LOW;
      resetledState = LOW;

      led1State = LOW;

      led3State = LOW;

      matrix1.fillScreen(0);
      matrix1.show();

      matrix3.fillScreen(0);
      matrix3.show();

      delay(100);

      matrix1.setCursor(
          1,
          6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
      matrix1.print(score1);
      matrix1.show();

      matrix3.setCursor(
          1,
          6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
      matrix3.print(score3);
      matrix3.show();

      // Serial.println("-1");
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

    // if the buzzerState was turned on, Sound the buzzer for a half second &
    // turn it off.
    if (buzzerState == 1) {
      tone(buzzerPin, frequency, 500);
      beep();
      buzzerState = 0;
    }
  }

  if (updateTime < 0) {
    // Displaying the results
    delay(2000);

    if (score1 > score3) { // Player 1 win
      lcd.setCursor(0, 0);
      lcd.print("          ");
      lcd.setCursor(0, 0);
      lcd.print("Yellow wins!");
      matrix1.fillScreen(0);
      matrix1.show();
      matrix1.setCursor(
          1,
          6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
      matrix1.print("W");
      matrix1.show();
      matrix3.fillScreen(0);
      matrix3.show();
      matrix3.setCursor(
          1,
          6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
      matrix3.print("L");
      matrix3.show();

    } else if (score3 > score1) {

      lcd.setCursor(0, 0);
      lcd.print("          ");
      lcd.setCursor(0, 0);
      lcd.print("Blue wins!");
      matrix1.fillScreen(0);
      matrix1.show();
      matrix1.setCursor(
          1,
          6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
      matrix1.print("L");
      matrix1.show();
      matrix3.fillScreen(0);
      matrix3.show();
      matrix3.setCursor(
          1,
          6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
      matrix3.print("W");
      matrix3.show();

    } else {

      lcd.print("          ");
      lcd.setCursor(0, 0);
      lcd.print("Stalemate");
      matrix1.fillScreen(0);
      matrix1.show();
      matrix1.setCursor(
          1,
          6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
      matrix1.print("S");
      matrix1.show();
      matrix3.fillScreen(0);
      matrix3.show();
      matrix3.setCursor(
          1,
          6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
      matrix3.print("S");
      matrix3.show();
    }

    delay(2000);

    matrix1.fillScreen(0);
    matrix1.show();
    matrix3.fillScreen(0);
    matrix3.show();
  }
  currentTime = millis();
}

char whoWon() { // Returns which player won (Y, G or B) or 0 if stalemate

  char R = 0;
  int maxScore;
  maxScore = max(score1, max(score2, score3));
  if (score1 == maxScore && score2 != maxScore && score3 != maxScore) {

    R = 'Y';
  } else if (score2 == maxScore && score1 != maxScore && score3 != maxScore) {

    R = 'G';
  } else if (score3 == maxScore && score1 != maxScore && score2 != maxScore) {

    R = 'B';
  }

  return R;
}

void duel() {

  int timer = defaultTimer;
  boolean quit = 0;
  currentTime = millis();
  int updateTime = 0; // To avoid continuous refreshing of the screen
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  while (!quit) {

    // Timer \/ \/ \/ \/ \/ \/ \/ \/ \/

    if (pollingForPresses) {
      if (updateTime < 0) {
        Serial.print(millis());
        Serial.print(" ");
        Serial.print(currentTime);
        Serial.print(" ");
        lcd.setCursor(0, 0);
        lcd.print("Time's up");
        tone(buzzerPin, 1760, 200);
        tone(buzzerPin, 880, 200);
        tone(buzzerPin, 1760, 200);
        tone(buzzerPin, 880, 200);

        quit = 1;

        delay(3000);

      } else {

        if (millis() % 1000 == 0) {
          tone(buzzerPin, 880, 200);
          lcd.setCursor(0, 0);
          lcd.print("          ");
          lcd.setCursor(0, 0);
          updateTime = (timer - (millis() - currentTime));
          //          Serial.print(timer);
          //          Serial.print(" = timer, ");
          //          Serial.print(currentTime);
          //          Serial.print(" = currentTime ");
          //          Serial.print(updateTime);
          //          Serial.print(millis() - currentTime);
          //          Serial.print(" = millis() - currentTime ");
          //          Serial.print(updateTime);
          //          Serial.println(" = updateTime ");
          lcd.print(updateTime / 1000);
        }
      }
    } else {

      currentTime = millis();
    }
    //Timer /\ /\ /\ /\ /\ /\ /\ /\ /\


    resetButtonState = digitalRead(resetButtonPin);

    if (podium[3] != 'Y') {
      button1State = digitalRead(button1Pin);
    }

    if (podium[3] != 'G' && podium[3] != 0) {
      button2State = digitalRead(button2Pin);
    }

    if (podium[3 != 'B']) {
      button3State = digitalRead(button3Pin);
    }
    buttonIncState = !digitalRead(scoreIncPin);
    buttonDecState = !digitalRead(scoreDecPin);

    Serial.print(currentTime);
    if (resetButtonState == HIGH && ((millis() - currentTime) > 1000)) {
      quit = 1;
      substate = 0;
      presubstate = 1;
      state = 'M';
    }

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
    } else {
      // Otherwise, Polling is off.  That means one of the 3 player buttons has
      // been pressed. If the resetbutton was pressed, that means that the
      // player has taken his turn and we're starting another round.  In that
      // case, set "whichbuttonwasPressed" to 4. But we only do that if polling
      // is off. (If it's on, the resetbuttonis ignored until somebody presses
      // button 1-3).
      if (buttonIncState == HIGH) {
        whichButtonWasPressed = 5;
      }
      if (buttonDecState == HIGH) {
        whichButtonWasPressed = 6;
      }

      // if (correctAnswerButtonState == HIGH) {whichButtonWasPressed = 5;}
      // if (incorrectAnswerButtonState == HIGH) {whichButtonWasPressed = 6;}
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

      timer = updateTime;
      pollingForPresses = LOW;
      buzzerState = HIGH;
      frequency = 880;
      resetledState = HIGH;

      // Switch on Led 1 - Yellow
      led1State = HIGH;

      matrix1.fillScreen(0); // If Yellow button can be pressed then the other
                             // player is either Green or Blue
      matrix1.show();
      if (podium[2] != 'G' && podium[2] != 0) {
        // Switch off Led 2
        led2State = LOW;
        matrix2.fillScreen(0);
        matrix2.show();
      } else {
        // Switch off Led 3
        led3State = LOW;
        matrix3.fillScreen(0);
        matrix3.show();
      }

      delay(100);

      matrix1.fillScreen(matrix1.Color(255, 255, 0));
      matrix1.show();

      // Tell the computer which team pushed its button.
      // Serial.print("Team ");
      // Serial.println(whichButtonWasPressed);
      break;

    case (2):

      // Button2 was pushed.
      // Turn off Polling.   Turn on the buzzer.
      // Turn on LED2 - all others off.

      pollingForPresses = LOW;
      buzzerState = HIGH;
      frequency = 1320;
      // 880+440=1320
      resetledState = HIGH;

      // Switch on Led 2 - Green
      led2State = HIGH;
      // If Green button can be pressed then the other
      // player is either Yellow or Blue
      matrix2.fillScreen(0);
      matrix2.show();
      if (podium[2] != 'Y') {
        matrix1.fillScreen(0);
        matrix1.show();
        // Switch off Led 1 - Yellow
        led1State = LOW;
      } else {
        matrix3.fillScreen(0);
        matrix3.show();

        // Switch off Led 3 - Blue
        led3State = LOW;
      }
      delay(100);

      matrix2.fillScreen(matrix2.Color(0, 255, 0));
      matrix2.show();

      //      Serial.print("Team ");
      //      Serial.println(whichButtonWasPressed);
      break;

    case (3):

      // Button3 was pushed.
      // Turn off Polling.   Turn on the buzzer.
      // Turn on LED3 - all others off.
      timer = updateTime;
      pollingForPresses = LOW;
      buzzerState = HIGH;
      frequency = 1540;
      // 1320+220=1540
      resetledState = HIGH;

      // Switch off Led 1 - Yellow
      led1State = LOW;

      // Switch on Led 3 - Blue
      led3State = HIGH;
      matrix3.fillScreen(0);
      matrix3.show();

      if (podium[2] != 0 && podium[2] != 'G') {
        matrix2.fillScreen(0);
        matrix2.show();
      } else {
        matrix1.fillScreen(0);
        matrix1.show();
      }

      delay(100);

      matrix3.fillScreen(matrix2.Color(0, 0, 255));
      matrix3.show();

      Serial.print("Team ");
      Serial.println(whichButtonWasPressed);

      break;

    case (5):
      // The score increment button (Button5) was pushed.
      // If Polling is off
      // Turn off all LEDs and the buzzer.
      // Turn Polling back ON.
      // Increment the score of the player

      beep_answer(1);

      pollingForPresses = HIGH;
      buzzerState = LOW;
      resetledState = LOW;

      if (podium[2] != 'Y') {
        matrix1.fillScreen(0);
        matrix1.show();
        led1State = LOW;
        if (led1State == HIGH) {
          score1++;
        }
      }
      if (podium[2] != 'G' && podium[2] != 0) {
        matrix2.fillScreen(0);
        matrix2.show();
        led2State = LOW;
        if (led2State == HIGH) {
          score2++;
        }
      }
      if (podium[2] != 'B') {
        matrix3.fillScreen(0);
        matrix3.show();
        led3State = LOW;
        if (led3State == HIGH) {
          score3++;
        }
      }

      delay(100);

      if (podium[2] != 'Y') {
        matrix1.setCursor(
            1,
            6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
        matrix1.print(score1);
        matrix1.show();
      }

      if (podium[2] != 'G' && podium[2] != 0) {
        matrix2.setCursor(
            1,
            6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
        matrix2.print(score2);
        matrix2.show();
      }

      if (podium[2] != 'B') {
        matrix3.setCursor(
            1,
            6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
        matrix3.print(score3);
        matrix3.show();
      }

      // Serial.println("+1");
      break;

    case (6):
      // The score decrement button (Button6) was pushed.
      // If Polling is off
      // Turn off all LEDs and the buzzer.
      // Turn Polling back ON.
      // Decrement the score of the player

      beep_answer(0);

      if (led1State == HIGH &&
          score1 > 0) { // This implies player Yellow is in
                        // game, no need to check the podium
        score1--;
      }

      if (led3State == HIGH && score3 > 0) {
        score3--;
      }

      if (led3State == HIGH && score3 > 0) {
        score3--;
      }

      pollingForPresses = HIGH;
      buzzerState = LOW;
      resetledState = LOW;

      if (podium[2] != 'Y') {
        led1State = LOW;
        matrix1.fillScreen(0);
        matrix1.show();
        matrix1.setCursor(
            1,
            6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
        matrix1.print(score1);
        matrix1.show();
      }

      if (podium[2] != 0 && podium[2] != 'G') {
        led2State = LOW;
        matrix2.fillScreen(0);
        matrix2.show();
        matrix2.setCursor(
            1,
            6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
        matrix3.print(score2);
        matrix3.show();
      }

      if (podium[2] != 'B') {
        led3State = LOW;
        matrix3.fillScreen(0);
        matrix3.show();
        matrix3.setCursor(
            1,
            6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
        matrix3.print(score3);
        matrix3.show();
      }

      delay(100);

      // Serial.println("-1");
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

    // if the buzzerState was turned on, Sound the buzzer for a half second &
    // turn it off.
    if (buzzerState == 1) {
      tone(buzzerPin, frequency, 500);
      beep();
      buzzerState = 0;
    }

    if (updateTime < 0) {
      // Displaying the results
      delay(2000);

      if (whoWon() == 'Y') { // Player 1 win
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print("Yellow wins!");
        matrix1.fillScreen(0);
        matrix1.show();
        matrix1.setCursor(
            1,
            6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
        matrix1.print("W");
        matrix1.show();
        if (podium[2] != 'B') {
          matrix3.fillScreen(0);
          matrix3.show();
          matrix3.setCursor(1,
                            6); // indispensable sinon n'affiche rien, le
                                // curseur est décalé!!!!
          matrix3.print("L");
          matrix3.show();
        } else {

          matrix2.fillScreen(0);
          matrix2.show();
          matrix2.setCursor(1,
                            6); // indispensable sinon n'affiche rien, le
                                // curseur est décalé!!!!
          matrix2.print("L");
          matrix2.show();
        }
      }

      else if (whoWon() == 'G') {

        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print("Green wins!");
        matrix2.fillScreen(0);
        matrix2.show();
        matrix2.setCursor(
            1,
            6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
        matrix2.print("W");
        matrix2.show();
        if (podium[2] != 'B') {
          matrix3.fillScreen(0);
          matrix3.show();
          matrix3.setCursor(1,
                            6); // indispensable sinon n'affiche rien, le
                                // curseur est décalé!!!!
          matrix3.print("L");
          matrix3.show();
        } else {

          matrix1.fillScreen(0);
          matrix1.show();
          matrix1.setCursor(1,
                            6); // indispensable sinon n'affiche rien, le
                                // curseur est décalé!!!!
          matrix1.print("L");
          matrix1.show();
        }
      } else if (whoWon() == 'B') {

        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print("Blue wins!");
        matrix3.fillScreen(0);
        matrix3.show();
        matrix3.setCursor(
            1,
            6); // indispensable sinon n'affiche rien, le curseur est décalé!!!!
        matrix3.print("W");
        matrix3.show();
        if (podium[2] != 'Y') {
          matrix1.fillScreen(0);
          matrix1.show();
          matrix1.setCursor(1,
                            6); // indispensable sinon n'affiche rien, le
                                // curseur est décalé!!!!
          matrix1.print("L");
          matrix1.show();
        } else {

          matrix2.fillScreen(0);
          matrix2.show();
          matrix2.setCursor(1,
                            6); // indispensable sinon n'affiche rien, le
                                // curseur est décalé!!!!
          matrix2.print("L");
          matrix2.show();
        }
      } else {

        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print("Stalemate");
        if (podium[2] != 'Y') {
          matrix1.fillScreen(0);
          matrix1.show();
          matrix1.setCursor(1,
                            6); // indispensable sinon n'affiche rien, le
                                // curseur est décalé!!!!
          matrix1.print("S");
          matrix1.show();
        }
        if (podium[2] != 'G' && podium[2] != 0) {
          matrix2.fillScreen(0);
          matrix2.show();
          matrix2.setCursor(1,
                            6); // indispensable sinon n'affiche rien, le
                                // curseur est décalé!!!!
          matrix2.print("S");
          matrix2.show();
        }
        if (podium[2] != 'B') {

          matrix3.fillScreen(0);
          matrix3.show();
          matrix3.setCursor(1,
                            6); // indispensable sinon n'affiche rien, le
                                // curseur est décalé!!!!
          matrix3.print("S");
          matrix3.show();
        }
      }
    }
  }

  delay(2000);
  if (podium[2] != 'Y') {
    matrix1.fillScreen(0);
    matrix1.show();
  }

  if (podium[2] != 'G' && podium[2] != 0) {
    matrix2.fillScreen(0);
    matrix2.show();
  }

  if (podium[2] != 'B') {
    matrix3.fillScreen(0);
    matrix3.show();
  }

  currentTime = millis();
}
