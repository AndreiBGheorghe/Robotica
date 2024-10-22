#include <Arduino.h>

// Definirea pinilor pentru LED-uri si butoane
#define BUTTONSTART 3 // Buton - start
#define BUTTONSTOP 2  // Buton - stop
#define LEDR 6        // LED-ul RGB - Rosu
#define LEDG 5        // LED-ul RGB - Verde
#define LED1 10       // LED-ul 1 - 25%
#define LED2 9        // LED-ul 2 - 50%
#define LED3 8        // LED-ul 3 - 75%
#define LED4 7        // LED-ul 4 - 100%

bool charging = false;            // Contorizeaza incarcarea
int chargeLvl = 0;                // Ne arata nivelul incarcarii
unsigned long lastUpdate = 0;     // Timp ultim update
unsigned long lastDebounce = 0;   // Timp ultim debounce
unsigned long debounceDelay = 50; // Timp debounce (50ms)
unsigned long stopHold = 1000;    // Timp apasare lunga

int lastbutonStartState = LOW;    // Stare precedenta - buton start
int butonStartState = LOW;        // Stare curenta - buton start
int butonStopState = LOW;         // Stare curenta - buton stop

bool blinking = false;            // Contorizeaza clipirea
unsigned long lastBlink = 0;      // Timp ultim blink

// Functie - porneste incarcarea
void startCharging() {
    charging = true;
    chargeLvl = 1;
    lastUpdate = millis();
    digitalWrite(LEDG, LOW); 
    digitalWrite(LEDR, HIGH);
}

// Functie - reseteaza toate LED-urile
void resetAllLeds() {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
}

// Functie - opreste incarcarea dupa ce este finalizata si reseteaza procesul
void stopCharging() {
    charging = false;
    digitalWrite(LEDG, HIGH);
    digitalWrite(LEDR, LOW);
    resetAllLeds();
}

// Functie - clipirea LED-ului curent
void blinkCurrentLed() {
    int currentLed;
    if (chargeLvl == 1)
        currentLed = LED1;
    if (chargeLvl == 2)
        currentLed = LED2;
    if (chargeLvl == 3)
        currentLed = LED3;
    if (chargeLvl == 4)
        currentLed = LED4;
    if (millis() - lastBlink > 500) {
        blinking = !blinking;
        digitalWrite(currentLed, blinking ? LOW : HIGH);
        lastBlink = millis();
    }
}

// Functie - animatie final
void blinkAllLeds() {
    for (int i = 0; i < 4; i++) {
        digitalWrite(LED1, HIGH);
        digitalWrite(LED2, HIGH);
        digitalWrite(LED3, HIGH);
        digitalWrite(LED4, HIGH);
        delay(500);
        resetAllLeds();
        delay(500);
    }
}

// Functie - opreste incarcarea fortat
void interruptCharging() {
    charging = false;
    blinkAllLeds();
    stopCharging();
}

// Functie - procesul de incarcare
void updateCharging() {
    if (chargeLvl <= 4) {
        if (millis() - lastUpdate > 3000) {
            chargeLvl++;
            lastUpdate = millis();
        }
        blinkCurrentLed();
    } else {
        blinkAllLeds();
        stopCharging();
    }
}

void setup() {
    // Declararea pinilor de intrare
    pinMode(BUTTONSTART, INPUT);
    pinMode(BUTTONSTOP, INPUT);
    
    // Declararea pinilor de iesire
    pinMode(LEDR, OUTPUT);
    pinMode(LEDG, OUTPUT);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(LED4, OUTPUT);

    // Starea initiala - Verde
    digitalWrite(LEDG, HIGH);
}

void loop() {
    // Buton - start
    int reading = !digitalRead(BUTTONSTART);
    if (reading != lastbutonStartState) {
        lastDebounce = millis();
    }
    if ((millis() - lastDebounce) > debounceDelay) {
        if (reading != butonStartState) {
            butonStartState = reading;
            if (butonStartState == HIGH && !charging) {
                startCharging();
            }
        }
    }
    lastbutonStartState = reading;

    // Buton - stop
    butonStopState = digitalRead(BUTTONSTOP);
    if (butonStopState == LOW) {
        unsigned long pressStart = millis();
        while (digitalRead(BUTTONSTOP) == LOW) {
            if (millis() - pressStart > stopHold) {
                interruptCharging();
                break;
            }
        }
    }

    // Actualizarea procesului de incarcare
    if (charging) {
        updateCharging();
    }
}