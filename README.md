# La suite

 - dessouder les 4 relais et reconstruire le pont H sur la plaque de prototypage. On gagnerait beaucoup de place et le montage serait plus lisible.
 - refaire tout le montage sur une plaque d'essai pour le fixer.

# Ressources

 - http://fr.wikipedia.org/wiki/Pont_en_H (pour la propulsion)
 - http://www.ebay.fr/itm/like/190854819245?lpid=97 (plaque d'essai)

# Direction

 - jaune : +5v 
 - blanc : lecture resistance variable
 - noir : moteur 1 (droite ?)
 - orange : moteur 2 (gauche ?)
 - bleu : rien
 - vert : masse

# Propulsion

masse sur le brun -> marche avant

* C2654 (NPN) http://pdf.datasheetcatalog.com/datasheet2/6/0eepasxw2cg3exokewh41s7hkdyy.pdf
* A1129 (PNP) http://pdf.datasheetcatalog.com/datasheet2/7/0tkra9paieu2w691yf7y3f8i6qky.pdf

http://www.mcmanis.com/chuck/robotics/tutorial/h-bridge/bjt_theory.html

C'est certainement la raison pour laquelle mon pont H ne fonctionne pas ...

Montage d'un pont H en PNP/NPN : http://wiki.hackgyver.org/images/07_Pont_en_H.pdf

## debug

- tester la masse
- dessouder le circuit de la voiture et le tester séparément.
    - si il ne marche pas, dessouder les composants et recommencer
    - sinon, étudier les autres sources de panne possibles.

