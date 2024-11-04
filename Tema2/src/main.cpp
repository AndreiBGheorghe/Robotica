#include <Arduino.h>

// Declararea Pin-ilor, dar si a variabilelor de urmeaza sa fie folosite
const int buttonStartPin = 3;
const int buttonDifficultyPin = 2;
const int redRgbLedPin = 10;
const int greenRgbLedPin = 9;
const int blueRgbLedPin = 8;

void generateWord();

const unsigned long buttonDebounceDelay = 500;
const unsigned long roundTime = 30000;
const int wordCount = 30;
const char* words[wordCount] = { // Dictionarul de cuvinte
  "ambition", "whisper", "eclipse", "symphony", "mystery", "journey", "reflection", "harmony", "obsession", "inspiration",
  "courage", "fortress", "solitude", "illusion", "adventure", "wisdom", "vision", "enigma", "benevolence", "whirlpool",
  "infinity", "resilience", "miracle", "tranquility", "labyrinth", "essence", "destiny", "serenity", "wilderness", "echo"
};

// Optiunile folosite pentru dificultate
#define DIFFICULTY_OPTIONS 3
enum DifficultyOptions { 
  UNKNOWN = -1, 
  EASY = 0, 
  MEDIUM = 1, 
  HARD = 2 
};

// Starile posibile ale programului
enum StateOptions { 
  IDLE = 0, 
  STARTING = 1, 
  RUNNING = 2
};

DifficultyOptions difficulty = UNKNOWN;
StateOptions gameState = IDLE;

unsigned long lastStartPressTime = 0;
unsigned long lastDifficultyPressTime = 0;
unsigned long roundStartTime = 0;
unsigned long wordStartTime = 0;
unsigned int timeLimit = 0;
int correctWordNumber = 0;
String chosenWord;
String inputBuffer = "";

// Functie care seteaza intensitatea LED-ului RBG pentru fiecare culoare
void setLedColor(int red, int green, int blue) {
  analogWrite(redRgbLedPin, red);
  analogWrite(greenRgbLedPin, green);
  analogWrite(blueRgbLedPin, blue);
}

// Functie care cicleaza prin dificultatile posibile si afiseaza mesajul in terminal
void changeDifficulty() {
  if (millis() - lastDifficultyPressTime > buttonDebounceDelay) {
    lastDifficultyPressTime = millis();
    difficulty = (DifficultyOptions)((difficulty + 1) % DIFFICULTY_OPTIONS);
    Serial.print(difficulty == EASY ? "Easy" : difficulty == MEDIUM ? "Medium" : "Hard");
    Serial.println(" mode on!");
    timeLimit = difficulty == EASY ? 10000 : (difficulty == MEDIUM ? 5000 : 3000);
    Serial.print(difficulty == EASY ? "10 seconds" : difficulty == MEDIUM ? "5 seconds" : "3 seconds");
    Serial.println(" to type each word.");
  }
}

// Functie care porneste sau opreste programul, dar si detecteaza daca dificultatea nu a fost aleasa
void triggerStartStop() {
  if (millis() - lastStartPressTime > buttonDebounceDelay) {
    lastStartPressTime = millis();
    if (difficulty == UNKNOWN) {
      Serial.println("Select a difficulty level first.");
      return;
    }
    if (gameState == IDLE) {
      gameState = STARTING;
      Serial.println("Starting game.");
    }
    else {
      Serial.println("Game ended.");
      gameState = IDLE;
      correctWordNumber = 0;
      setLedColor(255, 255, 255);
    }
  }
}

// Functie care face animatia de start, dar scrie si in terminal numaratoarea inversa
void countdownStart() {
  for (int i = 3; i > 0; i--) {
    Serial.println(i);
    setLedColor(255, 255, 255);
    delay(500);
    setLedColor(0, 0, 0);
    delay(500);
  }
  gameState = RUNNING;
  roundStartTime = millis();
  Serial.println("Type the words as fast as you can!");
  setLedColor(0, 255, 0);
  generateWord();
}

// Functie care genereaza aleatoriu cuvinte
void generateWord() {
  chosenWord = words[random(wordCount)];
  Serial.print("Type this word: ");
  Serial.println(chosenWord);
  inputBuffer = "";
  wordStartTime = millis();
}

// Functie care detecteaza daca cuvantul scris in terminal este corect sau nu, dar anunta si cand timpul s-a terminat
void checkInput() {
  while (Serial.available()) {
    char receivedChar = Serial.read();
    if (receivedChar == '\b' && inputBuffer.length() > 0) {
      inputBuffer.remove(inputBuffer.length() - 1);
    }
    else if (receivedChar == '\n') {
      inputBuffer.trim();
      if (inputBuffer.equals(chosenWord)) {
        Serial.println("Correct!");
        correctWordNumber++;
        generateWord();
        setLedColor(0, 255, 0);
        wordStartTime = millis();
      }
      else {
        Serial.println("Incorrect. Try again!");
        setLedColor(255, 0, 0);
      }
      inputBuffer = "";
    }
    else {
      inputBuffer += receivedChar;
    }
  }
  if (millis() - wordStartTime > timeLimit) {
    Serial.println("Time's up!");
    generateWord();
    inputBuffer = "";
  }
}

void setup() {
  Serial.begin(28800);
  // Asignarea Pin-ilor
  pinMode(redRgbLedPin, OUTPUT);
  pinMode(greenRgbLedPin, OUTPUT);
  pinMode(blueRgbLedPin, OUTPUT);
  pinMode(buttonStartPin, INPUT_PULLUP);
  pinMode(buttonDifficultyPin, INPUT_PULLUP);

  // Folosirea intreruperilor pentru butoane
  attachInterrupt(digitalPinToInterrupt(buttonDifficultyPin), changeDifficulty, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonStartPin), triggerStartStop, FALLING);

  setLedColor(255, 255, 255);

  randomSeed(millis());
  Serial.println("Choose a difficulty!");
}

void loop() {
  if (gameState == STARTING) {
      countdownStart();
  }
  else if (gameState == RUNNING && millis() - roundStartTime < roundTime) {
      checkInput();
  }
  else if (gameState == RUNNING && millis() - roundStartTime >= roundTime) {
      Serial.print("Game over! Correct words: ");
      Serial.println(correctWordNumber);
      gameState = IDLE;
      setLedColor(255, 255, 255);
  }
}