// Kimenetek
const int autoPiros = 8;
const int autoSarga = 9;
const int autoZold = 10;
const int hangszoro = 11;
const int gyalogosPiros = 12;
const int gyalogosZold = 13;

// Bemenetek
const int ldrPin = A3; // Fényérzékelő

// IoT Gombok
const int btnEjszaka = A1; // Kényszerített Éjszaka gomb
const int btnNappal = A2;  // Kényszerített Nappal gomb

// Kijelző
const int segA = 3;
const int segB = 4;
const int segC = 5;
const int segD = 6;
const int segE = 7;
const int segF = A5;
const int segG = A4;

const byte szamok[10][7] = { 
  { 1, 1, 1, 1, 1, 1, 0 }, { 0, 1, 1, 0, 0, 0, 0 }, { 1, 1, 0, 1, 1, 0, 1 }, 
  { 1, 1, 1, 1, 0, 0, 1 }, { 0, 1, 1, 0, 0, 1, 1 }, { 1, 0, 1, 1, 0, 1, 1 }, 
  { 1, 0, 1, 1, 1, 1, 1 }, { 1, 1, 1, 0, 0, 0, 0 }, { 1, 1, 1, 1, 1, 1, 1 }, 
  { 1, 1, 1, 1, 0, 1, 1 } 
};

int fenyero = 0;
const int soteT_KUSZOB = 300;
char iotStatusz = 'A';  // A=Auto, E=Ejszaka, N=Nappal
unsigned long utolsoAdatKuldes = 0;

void setup() {
  // Kimenetek beállítása
  pinMode(autoPiros, OUTPUT); pinMode(autoSarga, OUTPUT); pinMode(autoZold, OUTPUT);
  pinMode(gyalogosPiros, OUTPUT); pinMode(gyalogosZold, OUTPUT);
  pinMode(hangszoro, OUTPUT);
  
  pinMode(segA, OUTPUT); pinMode(segB, OUTPUT); pinMode(segC, OUTPUT);
  pinMode(segD, OUTPUT); pinMode(segE, OUTPUT); pinMode(segF, OUTPUT); pinMode(segG, OUTPUT);

  // Bemenetek beállítása
  pinMode(ldrPin, INPUT);
  
  pinMode(btnEjszaka, INPUT);
  pinMode(btnNappal, INPUT);

  Serial.begin(9600);
  
  // Alapállapot
  szamTorles();
}

void loop() {
  // 1. Kommunikáció (Serial + Gombok)
  kezelParancsokat();

  // 2. Adatküldés (Pythonnak)
  adatKuldes();

  // 3.
  if (iotStatusz == 'E') {
    ejszakaiMod();
  } 
  else if (iotStatusz == 'N') {
    automataCiklus(); // Kényszerített nappal -> Ciklus fut
  }
  else { // "A" = Automata
    if (fenyero < soteT_KUSZOB) {
      ejszakaiMod();
    } else {
      automataCiklus();
    }
  }
}

void kezelParancsokat() {
  // Serial parancsok
  if (Serial.available() > 0) {
    char bejovo = Serial.read();
    if (bejovo == 'E') iotStatusz = 'E';
    if (bejovo == 'N') iotStatusz = 'N'; // Nappalt kényszeríteni
    if (bejovo == 'A') iotStatusz = 'A'; // Vissza automatára
  }

  // Fizikai Gombok
  if (digitalRead(btnEjszaka) == HIGH) {
    iotStatusz = 'E';
    Serial.println("EVENT: Gomb -> Ejszakai Mod");
    delay(300);
  }
  
  if (digitalRead(btnNappal) == HIGH) {
    iotStatusz = 'N';
    Serial.println("EVENT: Gomb -> Nappali Mod");
    delay(300);
  }
}

void adatKuldes() {
  fenyero = analogRead(ldrPin);
  if (millis() - utolsoAdatKuldes > 500) {
    Serial.print("F:"); Serial.print(fenyero);
    Serial.print("|S:"); Serial.println(iotStatusz);
    utolsoAdatKuldes = millis();
  }
}

void ejszakaiMod() {
  // Minden piros/zöld kikapcsolva
  digitalWrite(autoZold, LOW); digitalWrite(autoPiros, LOW);
  digitalWrite(gyalogosZold, LOW); digitalWrite(gyalogosPiros, LOW);
  szamTorles();
  
  // Sárga villog
  digitalWrite(autoSarga, HIGH); delay(500);
  digitalWrite(autoSarga, LOW); delay(500);
}

void automataCiklus() {
  Serial.println("EVENT: Fázis - AUTÓ ZÖLD (9mp)");
  
  // 1. FÁZIS: AUTÓ ZÖLD (Visszaszámlálással)
  digitalWrite(autoPiros, LOW);
  digitalWrite(autoSarga, LOW);
  digitalWrite(autoZold, HIGH);
  digitalWrite(gyalogosZold, LOW);
  digitalWrite(gyalogosPiros, HIGH);
  
  // 9 másodperc visszaszámlálás (Autósoknak)
  for (int i = 9; i >= 0; i--) {
    // Ellenőrzés, hogye nem lett-e éjszaka hirtelen
    if (ellenorizMegszakitast()) return; 
    
    kiirSzam(i);
    delay(1000);
  }
  szamTorles();

  // 2. FÁZIS: SÁRGA
  Serial.println("EVENT: Fázis - SÁRGA");
  digitalWrite(autoZold, LOW);
  digitalWrite(autoSarga, HIGH);
  delay(2000);

  // 3. FÁZIS: AUTÓ PIROS / GYALOGOS ZÖLD
  Serial.println("EVENT: Fázis - AUTÓ PIROS (9mp)");
  digitalWrite(autoSarga, LOW);
  digitalWrite(autoPiros, HIGH);
  
  delay(1000); // Szünet
  
  digitalWrite(gyalogosPiros, LOW);
  digitalWrite(gyalogosZold, HIGH);
  
  // 9 másodperc visszaszámlálás (Gyalogosoknak)
  for (int i = 9; i >= 0; i--) {
    if (ellenorizMegszakitast()) return;
    
    kiirSzam(i);
    tone(hangszoro, 800, 100); // Csippanás a gyalogosnak
    delay(1000);
  }
  szamTorles();

  // 4. FÁZIS: Gyalogos villog
  for (int i = 0; i < 3; i++) {
    digitalWrite(gyalogosZold, LOW); delay(300);
    digitalWrite(gyalogosZold, HIGH); 
    tone(hangszoro, 1200, 100);
    delay(300);
  }
  digitalWrite(gyalogosZold, LOW);
  digitalWrite(gyalogosPiros, HIGH);
  
  delay(1000);
  
  // 5. FÁZIS: Vissza a zöldre (Sárgán át)
  digitalWrite(autoSarga, HIGH); delay(1000);
  digitalWrite(autoSarga, LOW); 
}
bool ellenorizMegszakitast() {
  // Fényérzékelő ellenőrzése
  fenyero = analogRead(ldrPin);
  if (iotStatusz == 'A' && fenyero < soteT_KUSZOB) {
     return true;
  }
  kezelParancsokat();
  if (iotStatusz == 'E') return true;
  
  return false;
}

void kiirSzam(int szam) {
  digitalWrite(segA, szamok[szam][0]); digitalWrite(segB, szamok[szam][1]);
  digitalWrite(segC, szamok[szam][2]); digitalWrite(segD, szamok[szam][3]);
  digitalWrite(segE, szamok[szam][4]); digitalWrite(segF, szamok[szam][5]);
  digitalWrite(segG, szamok[szam][6]);
}

void szamTorles() {
  digitalWrite(segA, LOW); digitalWrite(segB, LOW); digitalWrite(segC, LOW);
  digitalWrite(segD, LOW); digitalWrite(segE, LOW); digitalWrite(segF, LOW);
  digitalWrite(segG, LOW);
}