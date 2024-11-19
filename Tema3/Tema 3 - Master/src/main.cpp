#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include <SPI.h>
#include <string.h>

#define BAUD 28800
#define BACKSPACE 8

#define SERVO_PIN 5

Servo servoCeas; //Variabila pentru controlul servo motorului de la ceas

LiquidCrystal lcd(8, 9, 7, 4, 3, 2); //Variabila pentru controlul lcd-ului

byte data, dataReceived, dataSent, previousData;

int vectorBits[3] = {64, 32, 16};
int turn = 0, running = 0, ending = 0, nameReading = 0, nameIndex = 0, index = 0, rightAnswer = 0, angle = 90, scoreP1, scoreP2, _241ToSent = 0;

unsigned long gameTime = 60000, answerTime = 5000, breakTime = 2000, startDebounce = 500, transferInterval = 100, finishTime = 3000;
unsigned long lastDataSent = 0, lastAnswer = 0, startTime = 0, lastStartValue = 0, lastTransfer = 0, endTime = 0;

char line[20], name[2][5];

//Functie pentru citirea unui caracter de pe interfata seriala
//Caracterul citit va fi salavat in sirul de caractere pe linia val a matricii de caractere "name"
//Daca se apasa "Backspace" se va sterge ultimul caracter citit
void citireLitera(int val)
{
  char litera = Serial.read();
  if(int(litera) == BACKSPACE)
  {
    if(index > 0)
    {
      name[val][index - 1] = NULL;
      index--;
    }
  }else
  {
    name[val][index] = litera;
    index++;
    name[val][index] = NULL;
  }
}

//Funcite folosita pentru transformarea scorului in sir de caractere
//Sirul cu scorul va fi lipit la finalul sirului de caractere "str", care va contine numele player-ului caruia ii corespunde scorul
void toChar(int value, char str[20])
{
    char auxString[10];
    int pw = 1, i = 0;

    while(value/(pw*10))
      pw = pw*10;
    
    while(pw)
    {
      auxString[i] = (char)(((value/pw)%10) + 48);
      i++;
      pw/=10;
    }
    auxString[i] = NULL;
    strcat(str, auxString);
}

//Functie folosita pentru a printa scorul pe lcd; parametrul "mode" indica daca exista un castigator
//Daca mode = 0, atunci nu exista castigator, si se va afisa doar numele jucatorilor si scorul
//Daca mode = 1 sau 2, atunci inseamna ca a castigat jucatorul 1, respectiv jocatorul 2, lucru care va fi indicat pe lcd
void printScores(int mode)
{
  lcd.clear();

  if(mode == 1)
    strcpy(line, "WIN ");
  else
    strcpy(line, "");
  strcat(line, name[0]);
  strcat(line, ": ");
  toChar(scoreP1, line);
  lcd.setCursor(0,0);
  lcd.print(line);

  if(mode == 2)
    strcpy(line, "WIN ");
  else
    strcpy(line, "");
  strcat(line, name[1]);
  strcat(line, ": ");
  toChar(scoreP2, line);
  lcd.setCursor(0,1);
  lcd.print(line);
}

//Functia genereaza un byte de date de forma xaaa0000, unde x poate fi 0 sau 1, iar dintre a, doar unul poate fi 1, restul 0
//x indica jucatorul al carui rand este, iar a indica led-ul care trebuie aprins
//Daca aceasta functia a fost apelata ca rezultat al unui raspuns corect din partea unui jucator, se va trimite prin SPI 241
//pentru a indica placutei slave acest lucru, ca aceasta sa poata genera un sunet
//Altfel data trimisa prin SPI va fi 240, indicand o asteptare pentru placuta slave
//Functia memoreaza momentul la care a fost apelata
void generateData(int correct)
{
  lastAnswer = millis();
  data = turn * 128 + vectorBits[((random(3) + millis()) % 3)];
  turn = (turn + 1) % 2;
  if(correct)
  {
    dataSent = 241;
    _241ToSent = 9;
  }else
    dataSent = 240;
  dataReceived = 0;
}

//Functia porneste jocul: variabila "running" se face 1, se genereaza date pentru slave, se memoreaza momentul inceperii
//si se reseteaza scorurile
void startGame()
{
  running = 1;
  
  generateData(0);

  startTime = millis();

  scoreP1 = 0;
  scoreP2 = 0;

  printScores(0);
}

//Functie care printeaza in terminal datele primite si trimise pe SPI
void debug()
{
  if(millis() - lastDataSent > 200 && !nameReading)
  {
    lastDataSent = millis();
    Serial.println(dataSent);
    Serial.println(dataReceived);
  }
}

void setup() {
  Serial.begin(BAUD);

  analogWrite(6, 75); //LCD Contrast

  lcd.begin(16, 2);
  lcd.print("Welcome!");

  servoCeas.attach(SERVO_PIN);

  SPI.begin();                            
  SPI.setClockDivider(SPI_CLOCK_DIV8); 
  digitalWrite(SS, LOW); 

  dataSent = 255;
}

void loop() {

  //Verifica daca trimite date pe SPI
  if(millis() - lastTransfer > transferInterval)
  {
    lastTransfer = millis();
    previousData = dataReceived;
    dataReceived = SPI.transfer(dataSent);
  }

  //debug();

  //daca pe SPI s-a primit data cu valoarea egala cu 15 pentru o durata de timp jocul va intra in starea de citire a numelor jucatorilor
  if(dataReceived == 15 && dataReceived != previousData)
    lastStartValue = millis();
  if(dataReceived == 15 && millis() - lastStartValue > startDebounce && !ending && !nameReading)
  {
    
    nameReading = 1;
    nameIndex = 0;
    index = 0;
    Serial.println("\nPlayer 1:");
  }

  //Se citesc numele jucatorilor (4 caractere per nume), se salveaza in variablia "name" si dupa ce au fost ambele citite, jocul incepe
  if(nameReading)
  {
    if(Serial.available()) 
    {
      citireLitera(nameIndex);

      if(index == 4)
      {
        if(!nameIndex)
        {
          nameIndex++;
          index = 0;
          Serial.println("\nPlayer 2:");
        }else
        {
          nameReading = 0;
          startGame();
        }
      }
    }
  }

  if(running)
  {
    if(millis() - lastAnswer < breakTime)
    {
      //Daca au fost generate date dupa un raspuns corect, se va trimite de cateva ori prin SPI 241, ca slave-ul sa poata genera un sunet
      //Altfel se trimite 240, indicand slave-ului sa astepte
      if(_241ToSent)
      {
        _241ToSent--;
        dataSent = 241;
      }else
        dataSent = 240;
    }else if(dataReceived != 0 && dataReceived != (data >> 4) && dataReceived < 16 && millis() - lastAnswer < (answerTime + breakTime))
    {
      //Daca sa primit de la slave un raspuns gresit, se va trimite catre slave 112, ca slave-ul sa poata genera un sunet
      dataSent = 112;
    }else
    {
      printScores(0);

      if(millis() - lastAnswer > (answerTime + breakTime) || dataReceived == (data >> 4))
      {
        //Daca s-a primit un raspuns corect, sau timpul a expirat, se calculeaza scorul primit, se afiseaza pe lcd prin parametrul 
        //"line" playerul si scorul pe care l-a primit, dupa care lcd-ul va afisa ambii jucatorii cu scorul actualizat
        strcpy(line,"");
        int scoreValue = (5000 - (millis() - lastAnswer - breakTime))/10;

        lcd.clear();
        lcd.setCursor(0,0);
        if(turn)
        {
          scoreP1 += max(0, scoreValue);
          strcpy(line, name[0]);
        }else
        {
          scoreP2 += max(0, scoreValue);
          strcpy(line, name[1]);
        }
        strcat(line, ": +");
        toChar(max(0, scoreValue), line);
        lcd.print(line);

        //Daca a fost un raspun corect, se apeleaza generateData cu parametrul 1, indicand faptul ca trebuie sa anunte slave-ul sa
        //genereze un sunet. Altfel parametrul va fi 0 si slave-ul nu va genera un sunet
        if(dataReceived == (data >> 4))
          generateData(1);
        else
          generateData(0);
      }else
        dataSent = data;
    }

    //Angle se calculeaza in functie de timpul de joc ramas
    angle = ((millis() - startTime)/1000) * 180 / 60;

    if(millis() - startTime > gameTime)
    {
      //Daca jocul se termina, se memoreaza momentul pentru afisarea scorurilor, se reseteaza unghiul pentru servo, si jocul nu mai ruleaza
      running = 0;
      angle = 90;
      endTime = millis();
      ending = 1;
    }

  }else
  {
    //Daca programul nu e in starea de rulare, pe SPI se va trimite valoarea 255 (idle pentru slave)
    //Daca sa ajuns aici dupa terminarea unui joc, atunci pentru o perioada de timp vor fi afisate scorurile finale
    //Altfel lcd-ul va afisa mesajul de "welocme!"

    dataSent = 255;

    if(scoreP1 && millis() - endTime < finishTime)
    {
      if(scoreP1 > scoreP2)
        printScores(1);
      else
        printScores(2);
    }else
    {
      ending = 0;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Welcome!");
    }
  }

  //Servo-ul pentru ceas se intoarce la angle
  servoCeas.write(angle);
  delay(20);
}