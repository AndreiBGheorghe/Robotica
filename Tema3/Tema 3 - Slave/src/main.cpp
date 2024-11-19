#include <Arduino.h>
#include <SPI.h>

#define BAUD 28800

#define PLAYER1_BUTTON_GREEN 940
#define PLAYER1_BUTTON_YELLOW 380
#define PLAYER1_BUTTON_BLUE 200
#define PLAYER2_BUTTON_GREEN 1000
#define PLAYER2_BUTTON_YELLOW 440
#define PLAYER2_BUTTON_BLUE 205

#define PIN_PLAYER1_BUTTONS A0
#define PIN_PLAYER2_BUTTONS A1
#define PIN_PALYER1_BLUE_LED 4
#define PIN_PALYER1_GREEN_LED 6
#define PIN_PALYER1_RED_LED 8
#define PIN_PALYER2_GREEN_LED 2
#define PIN_PALYER2_YELLOW_LED 7
#define PIN_PALYER2_BLUE_LED 9
#define PIN_PERMANENT_LEDS 3

byte data = 0, dataSent = 0;
int valueAnalog;
int playerRound = 0;

volatile int idle = 1, wait = 0;

//Oprirea LED-urilor
void turnOffLeds() {
  digitalWrite(PIN_PALYER2_GREEN_LED, LOW);
  digitalWrite(PIN_PALYER2_YELLOW_LED, LOW);
  analogWrite(PIN_PALYER2_BLUE_LED, 0);
  digitalWrite(PIN_PALYER1_RED_LED, LOW);
  analogWrite(PIN_PALYER1_GREEN_LED, 0);
  digitalWrite(PIN_PALYER1_BLUE_LED, LOW);
}

//Functie folosita pentru citirea valorilor analog de la butoane
bool valueInRange(int value, int center) {
  int range = 50;
  return (value <= center + range && value >= center - range);
}

//Initializarea LED-urilor
void initLEDS() {
  pinMode(PIN_PALYER1_BLUE_LED, OUTPUT);
  pinMode(PIN_PALYER1_GREEN_LED, OUTPUT);
  pinMode(PIN_PALYER1_RED_LED, OUTPUT);
  pinMode(PIN_PALYER2_GREEN_LED, OUTPUT);
  pinMode(PIN_PALYER2_YELLOW_LED, OUTPUT);
  pinMode(PIN_PALYER2_BLUE_LED, OUTPUT);
  pinMode(PIN_PERMANENT_LEDS, OUTPUT);
}

ISR (SPI_STC_vect) {
  if (SPDR == 255) {
    idle = 1;
  } else if (SPDR == 241) {
    wait = 1;
    idle = 0;
  } else if (SPDR == 240) {
    turnOffLeds();
    wait = 1;
    idle = 0;
  } else if (SPDR == 112) {
    wait = 0;
    idle = 0;
  } else {
    data = SPDR;
    idle = 0;
    wait = 0;
  }
  SPDR = 0;
}

void setup() {
  Serial.begin(BAUD);
  initLEDS();
  analogWrite(PIN_PERMANENT_LEDS, 75);
  pinMode(MISO, OUTPUT);
  SPCR |= _BV(SPE);
  SPI.attachInterrupt();
  SPDR = 0;
}

void loop() {
  analogWrite(PIN_PERMANENT_LEDS, 75);

  if (idle) {
    turnOffLeds();
    int somethingPressed = (analogRead(PIN_PLAYER1_BUTTONS) + analogRead(PIN_PLAYER2_BUTTONS)) / 100;
    if (somethingPressed) {
      SPDR = 15;
    }
  } else {
    if (!wait) {
      if (data >> 4) {
        turnOffLeds();
        if (data & (1 << 7)) {
          playerRound = 1;
          if (data & (1 << 6))
            digitalWrite(PIN_PALYER2_GREEN_LED, HIGH);
          else if (data & (1 << 5))
            digitalWrite(PIN_PALYER2_YELLOW_LED, HIGH);
          else if (data & (1 << 4))
            analogWrite(PIN_PALYER2_BLUE_LED, 100);
        } else {
          playerRound = 0;
          if (data & (1 << 6))
            analogWrite(PIN_PALYER1_GREEN_LED, 75);
          else if (data & (1 << 5)) {
            digitalWrite(PIN_PALYER1_RED_LED, HIGH);
            analogWrite(PIN_PALYER1_GREEN_LED, 100);
          } else if (data & (1 << 4))
            digitalWrite(PIN_PALYER1_BLUE_LED, HIGH);
        }
      }

      if (!playerRound) {
        dataSent = 0;
        valueAnalog = analogRead(PIN_PLAYER1_BUTTONS);
        if (valueInRange(valueAnalog, PLAYER1_BUTTON_GREEN))
          dataSent |= (1 << 2);
        else if (valueInRange(valueAnalog, PLAYER1_BUTTON_YELLOW))
          dataSent |= (1 << 1);
        else if (valueInRange(valueAnalog, PLAYER1_BUTTON_BLUE))
          dataSent |= (1 << 0);
        else
          dataSent = 0;
      } else {
        dataSent = 8;
        valueAnalog = analogRead(PIN_PLAYER2_BUTTONS);
        if (valueInRange(valueAnalog, PLAYER2_BUTTON_GREEN))
          dataSent |= (1 << 2);
        else if (valueInRange(valueAnalog, PLAYER2_BUTTON_YELLOW))
          dataSent |= (1 << 1);
        else if (valueInRange(valueAnalog, PLAYER2_BUTTON_BLUE))
          dataSent |= (1 << 0);
        else
          dataSent = 0;
      }
      SPDR = dataSent;
    } else {
      turnOffLeds();
      SPDR = 0;
    }
  }
}