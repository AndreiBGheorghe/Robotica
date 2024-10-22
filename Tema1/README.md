# Tema 1: EV Charger

## Descrierea task-ului
Această temă implică simularea unei stații de încărcare pentru vehicule electrice, folosind un sistem cu LED-uri și două butoane.
Scopul este să se reproducă comportamentul unei stații reale, unde:
* Stația poate fi liberă (Verde) sau ocupată (Roșu), starea fiind indicată de un LED RGB.
* Încărcarea este vizualizată prin 4 LED-uri, reprezentând diferite niveluri de încărcare (LED1 - 25%, LED2 - 50%, LED3 - 75%, LED4 - 100%).
* Apăsarea scurtă a butonului de start declanșează procesul de încărcare, iar LED-urile se aprind pe rând, clipind de 3 ori la finalizarea fiecărei etape.
* Apăsarea butonului de start în timpul procesului de încărcare nu va face nimic.
* Apăsarea lungă a butonului de stop (cel puțin 1 secundă) întrerupe încărcarea, face toate LED-urile să clipească de 3 ori și resetează stația la starea liberă (verde).
* Implementarea ia în considerare debouncing-ul, pentru a asigura o citire corectă a apăsărilor butoanelor.

## Componentele utilizate
* 4x LED-uri albastre - afișează nivelul de încărcare, fiecare reprezentând 25%. (25%, 50%, 75%, 100%)
* 1x LED RGB - afișeaza starea stației, dacă LED-ul este verde înseamnă că stația este liberă, iar pentru roșu, ocupată.
* 2x Butoane - au ca scop pornirea sau oprirea forșată a stației de încărcare.
* 7x Rezistoare 220Ω - sunt folosite la LED-uri.
* 2x Rezistoare 1KΩ - sunt folosite la butoane.
* 1x Breadboard - pe acesta este făcut întreg montajul fizic.
* 14x Cabluri de legătură - sunt folosite la conectarea elementelor montajului.

## Imagini cu montajul fizic:
![IMG1](imagini/img1.png)
![IMG2](imagini/img2.png)
![IMG3](imagini/img3.png)

## Schema electrică:
![IMG4](imagini/img4.PNG)

## Video cu funcționalitatea montajului fizic:
[Link video](https://youtu.be/XKGqLv5ohrE?si=sBJzTCmdPMsX9j7X)
