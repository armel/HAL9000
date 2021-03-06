# HAL9000

Le projet HAL9000 permet de simuler l'ordinateur HAL de 2001 l'Odysée de l'Espace ! C'est tout ;)
Video dispo sur [Youtube](https://www.youtube.com/watch?v=s-WNS4vD0eY) ;)


# Architecture technique

## Présentation rapide

[M5Stack](https://m5stack.com/) est basé sur un ESP-32, dual core, pouvant être cadencé jusqu'à 240 MHz.  Néanmoins, le RRFRemote fonctionne parfaitement à 80 MHz. M5Stack dispose de 16 MB de mémoire flash. Comme tous les ESP, le Wifi est évidement intégré. L'écran 2 pouces IPS couleur, basé sur le chipset ILI9342C, affiche une résolution confortable de 320 x 240 pixels. Il est hyper lumineux. La batterie intégrée fait 110 mAh. Il est possible d'ajouter une batterie supplémentaire (de 700 ou 800mAh) si besoin. 

En terme de dimensions et de masse, c'est très compact : 54 x 54 x 18mm pour 47,2g. Se trimbale dans la poche sans problème ;) 

## Specs techniques détaillées :

Voici les specs techniques détaillées, pour les curieux :

| Resources |	Description |
| --------- | ------------ |
|ESP32| 240MHz dual core, 600 DMIPS, 520KB SRAM, Wi-Fi, dual mode Bluetooth
Flash| Memory	16MB|
|Power| Input	5V @ 500mA|
|Port|	TypeC x 1, GROVE(I2C+I/0+UART) x 1|
|Core|Bottom Port	PIN (G1，G2，G3，G16, G17, G18, G19, G21, G22, G23, G25, G26, G35, G36)|
|IPS Screen|	2 inch, 320x240 Colorful TFT LCD, ILI9342C, max brightness 853nit|
|Button|	Custom button x 3|
|Speaker|	1W-0928|
|Battery|	110mAh @ 3.7V|
|Antenna|	2.4G 3D Antenna|
|Operating Temperature|	32°F to 104°F ( 0°C to 40°C )|
|Net weight|	47.2g|
|Gross weight|	93g|
|Product Size|	54 x 54 x 18mm|
|Package Size	|95 x 65 x 25mm|
|Case Material|	Plastic ( PC )|

## En complément

Coté QSJ, compter autour de 25€. Vous disposez alors d'une plateforme de développement complète, totalement autonome, programmable en C et C++, MicroPython et UIFlow, depuis Linux, Windows ou MacOS, le tout dans un boitier compact et ultra ergonomique.


# Installation

## Pré-ambule

Le plus simple est d'installer [PlateformIO for VSCode](https://platformio.org/install/ide?install=vscode) sur votre PC (Linux ou Windows) ou sur votre Mac (Intel ou M1). C'est un environnement de développement multiplateforme et multilangage performant, en plus d'être agréable à utiliser.

Ensuite, toujours sur votre PC ou Mac, cloner le projet HAL9000 via la commande :

`https://github.com/armel/HAL9000.git`

## Compilation et flashage du M5Stack

Ouvrez le projet HAL9000 avec PlateformIO for VSCode.

Compilez et uploadez le projet sur votre M5Stack. C'est terminé.

## Préparation de la carte micro SD

Formatez une carte micro SD au format FAT32.
Copier l'ensemble des fichiers `.mjpg`, que vous trouverez dans le répertoire `media/video` à la racine de la carte micro SD.
Copier le fichier `HAL9000.jpg`, que vous trouverez dans le répertoire `media/image` à la racine de la carte micro SD.
Vous pouvez alors insérer la carte micro SD dans votre M5Stack.

That's all.

## Utilisation

Il existe 2 modes de fonctionnement. Il suffit de presser le bouton central pour basculer de l'un à l'autre. 

## Remarque

Ne fonctionne pas, pour le moment, sur le M5Stack Core2.