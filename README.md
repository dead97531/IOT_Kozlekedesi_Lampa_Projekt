# Közlekedési Csomópont Szimulátor



Ez a projekt egy intelligens közlekedési csomópont szimulációja, amely a modern IoT architektúrát valósítja meg. A rendszer egy közlekedési lámpát, gyalogos átkelőt, környezeti fényérzékelést és távoli vezérelhetőséget foglal magába.



## A rendszer három fő rétegből áll:



**Végponti eszköz (Edge Device)**: Arduino Uno, amely a szenzorokat olvassa és a lámpákat vezérli.



**Kommunikációs réteg**: Soros adatátvitel (UART) egy definiált protokollon keresztül.



**Irányítóközpont (Gateway/Dashboard)**: Python alapú grafikus felület a távfelügyelethez és vezérléshez.



## Szükséges szoftverek:



* SimulIDE

* Python 3.x

* Virtual Studio Code

* Free Virtual Serial Ports



## Virtuális portok létrehozása:



A Free Virtual Serial Ports alkalmazással létre kell hozni egy local bridge-et, COM1 és COM2 névvel.



## SimulIDE beállítása:



1. Meg kell nyitni a .sim1 fájlt. (Open circuit gomb)
2. Jobb klikk az Arduinón -> mega328-109 -> loadfirmware



## Python:



pip install pyserial



## Működés:



A rendszer alapvetően **Automata** módban indul:



1. **Nappal:** 9 másodperces ciklusokban váltja a szabad jelzést. A kijelzőn visszaszámlálás látható, amely jelzi, mennyi idő van hátra a váltásig.

2. **Éjszaka (vagy gombnyomásra):** A rendszer energiatakarékos módba lép, a lámpák kikapcsolnak, és csak a sárga jelzés villog figyelmeztetésként.



## IoT Funkciók

A rendszer "9600 baud" sebességgel kommunikál UART porton.

* **Adatküldés:** "F:fenyero|S:statusz" formátumban (pl. "F:512|S:A").

* **Parancs fogadás:** "E" (Éjszaka), "N" (Nappal), "A" (Automata).

