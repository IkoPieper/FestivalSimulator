--------------------
--- Installieren ---
--------------------

--- Windows ---

FestivalSimulator.zip entpacken


--- Linux ---

FestivalSimulator.zip entpacken

Folgende Pakete installieren:
libsdl2, libsdl2-ttf, libsdl2-mixer


-----------------
--- Ausf�hren ---
-----------------

--- Windows ---

Doppelklick auf FestivalSimulator.exe


--- Linux ---

Doppelklick auf FestivalSimulator.sh



----------------------
--- F�r Entwickler ---
----------------------

Selbst kompilieren:

Pakete: gcc, libsdl2-dev, libsdl2-ttf-dev, libsdl2-mixer-dev

Dann:
make
./FestivalSimulator

Main-Funktion ist in festival.c

(Fast) Alles soll �ber die object_t Strukturen geregelt werden.
Der Held ist ein Objekt, der Hintergrund ist ein Objekt, andere Figuren,
Zelte, B�hnen, ...., selbst der Bildschirm ist ein Objekt.
Grunds�tzlich kann jedes Objekt alles, was auch andere Objekte k�nnen.
