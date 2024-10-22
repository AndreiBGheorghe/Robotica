
# Robotică

Proiect în cadrul universității la Robotică

Acest proiect presupune simularea unei stații de încărcare pentru un vehicul electric folosind un sistem cu LED-uri și două butoane. Scopul principal este de a imita comportamentul unei stații reale, unde:

- **Stația poate fi "liberă" sau "ocupată"**, indicată printr-un LED RGB (verde pentru "liber", roșu pentru "ocupat").
- **Încărcarea este reprezentată vizual prin 4 LED-uri** care semnifică procentele de încărcare (25%, 50%, 75%, 100%).
- **Apăsarea scurtă a butonului de start** inițiază procesul de încărcare, iar LED-urile se vor aprinde treptat, clipind până la finalizarea fiecărei etape.
- **Apăsarea lungă a butonului de stop** (minim 1 secundă) întrerupe încărcarea, face toate LED-urile să clipească de 3 ori și resetează stația la starea "liberă" (verde).
- Implementarea trebuie să țină cont de fenomenul de **debouncing**, pentru a asigura că apăsările butoanelor sunt citite corect.
- Proiectul trebuie realizat fizic și trebuie să includă o schemă electrică și o demonstrație funcțională a montajului.

## Componentele utilizate

- **4x LED-uri standard**:  
  Reprezintă nivelurile de încărcare (25%, 50%, 75%, 100%). LED-urile vor clipi în timpul încărcării pentru a indica activitatea, iar când un nivel este complet încărcat, LED-ul rămâne aprins continuu.

- **1x LED RGB**:  
  Simulează starea stației. LED-ul este verde când stația este liberă și devine roșu când stația este ocupată în timpul încărcării.

- **2x Butoane**:  
  - **Buton Start**: Inițiază procesul de încărcare atunci când este apăsat scurt, dar nu face nimic dacă încărcarea este deja în desfășurare.  
  - **Buton Stop**: Apăsarea lungă oprește procesul de încărcare și resetează stația.

- **9x Rezistoare**:  
  - 7x 330Ω pentru limitarea curentului prin LED-urile standard.  
  - 2x 1KΩ pentru limitarea curentului prin butoane.

- **Breadboard**:  
  Folosită pentru a conecta componentele într-un circuit fizic

- **Fire de conexiune**:  
  Fire folosite pentru a realiza legăturile electrice între componente și Arduino.

Acest setup va simula un proces de încărcare gradual și realist al unei stații de încărcare pentru vehicule electrice, oferind și funcționalități de oprire de urgență și resetare a stației.

## Linkuri utile

### Imagini cu montajul fizic:
![IMG_6384](https://github.com/user-attachments/assets/49e594c1-fc00-47fd-a6dc-30beeb680367)
![IMG_6376](https://github.com/user-attachments/assets/b612f40a-f1b1-4d88-99b3-65e7161100df)
![IMG_6377](https://github.com/user-attachments/assets/1023fcd1-688b-4c90-9f13-253281378602)

### Schema electrică din Wokwi:
![Schema electrică](https://github.com/user-attachments/assets/5026d21a-316e-46bd-a482-eddafa90f058)

### Link către un video cu funcționalitatea montajului fizic:
[Video Funcționalitate](https://youtu.be/VQHp67Lzuac)
