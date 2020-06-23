--------------------
--- Installation ---
--------------------

--- Windows ---

Extract the zip-File: FestivalSimulator.zip


--- Linux ---

Extract the zip-File: FestivalSimulator.zip

Install the packages:
libsdl2-2.0, libsdl2-ttf-2.0, and libsdl2-mixer-2.0

Try to install newer versions if the 2.0-versions are not avaiable.

---

If you cannot find the above packages using your favorite package / app 
manager, try the following:

Start a Terminal. 

- On Debian-based systems (Debian / Ubuntu / Linux Mint / MX Linux / 
  elementary / Zorin OS / Pop!_OS / ...) type the following command and 
  confirm it with the Enter-Key:

    sudo apt-get install libsdl2-2.0 libsdl2-ttf-2.0 libsdl2-mixer-2.0

  If the packages are still not found you can search for newer versions 
  using the command:

    apt search libsdl2

- On Red Hat-based systems (Red Hat / Fedora / CentOS / ...) type the 
  following command and confirm it with the Enter-Key:

    sudo yum install SDL2 SDL2_ttf SDL2_mixer

- On Arch-based systems (Arch Linux / Manjaro / ...) type the following 
    command and confirm it with the Enter-Key:

    sudo pacman -S sdl2 sdl2_ttf sdl2_mixer


-----------------------
--- Run the program ---
-----------------------

--- Windows ---

Double click on FestivalSimulator.exe


--- Linux ---

Double click on FestivalSimulator.sh



----------------------
--- Für Entwickler ---
----------------------

Selbst kompilieren:

Pakete: gcc, libsdl2-dev, libsdl2-ttf-dev, libsdl2-mixer-dev

Dann:
make
./FestivalSimulator

Main-Funktion ist in festival.c

(Fast) Alles soll über die object_t Strukturen geregelt werden.
Der Held ist ein Objekt, der Hintergrund ist ein Objekt, andere Figuren,
Zelte, Bühnen, ...., selbst der Bildschirm ist ein Objekt.
Grundsätzlich kann jedes Objekt alles, was auch andere Objekte können.
