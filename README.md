# Tetris
---

## Instalacija i pokretanje

Da biste pokrenuli igricu na vašem računaru, potrebno je
* Instalirati biblioteku OpenGL
```shell
sudo apt-get update
sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev
```
* Pomoću terminala zadati komandu za kloniranje i pozicionirati se u folder kao što je prikazano u nastavku.
```shell
git clone https://github.com/MATF-RG19/RG16-tetris
cd RG16-tetris
```
 * Zatim izvršiti komandu ```make```. Program pokrenuti komandom:
```sh
./tetris
```

## Opis

Cilj ove igrice je sakupiti što više poena.

## Komande
Taster | Događaj  
----- | ------ 
<kbd>S/s</kbd> | Početak igre / Nastavak igre
<kbd>P/p</kbd> | Pauza
<kbd>R/r</kbd> | Resetovanje igre
<kdb>:arrow_up:</kdbg> | Rotacija figure
<kdb>:arrow_right:</kdbg> | Pomeranje figure u desno
<kdb>:arrow_left:</kdbg> | Pomeranje figure u levo
<kdb>:arrow_down:</kdbg> | Brzo spuštanje
<kdb>Space</kdbg> | Brzo spuštanje
<kbd>Q/q</kbd> | Rotacija scene na desno
<kbd>W/w</kbd> | Rotacija scene ka levo
<kbd>Esc</kbd> | Izlaz iz igre

