/*
   Les pins 9 et 11 balancent du courant dans l'arduino, ce qui est notoirement uncool.
   Ajouter des diodes pour empecher ce genre de comportements malvenus.
   */

#define DIR_ALLUMER 9      // allumage du moteur de direction
#define DIR_GAUCHE 12      // HIGH tourne a gauche (noir)
#define DIR_DROITE 13      // HIGH tourne a droite (orange)
#define DIR_MESURE A5      // pin pour la mesure de l'angle des roues
#define DIR_PUISSANCE 200  // [0-255] vitesse a laquelle la direction tourne
#define PROP_ARRIERE 10    // jaune
#define PROP_AVANT 11      // blanc
#define PROP_PUISSANCE 255 // vitesse à laquelle on va

#define TRIG_AVANT 4
#define ECHO_AVANT 5
#define TRIG_GAUCHE 6
#define ECHO_GAUCHE 7
#define TRIG_ARRIERE A0
#define ECHO_ARRIERE A1
#define TRIG_DROITE A2
#define ECHO_DROITE A3

#define GAUCHE 300
#define CENTRE 450
#define DROITE 700

#define PROXIMITE 3000 // micro-secondes (~90cm)

int mesure; // direction mesurée
int direction = CENTRE; // vers ou on veut aller
bool roule = false;

void orienter() {
    mesure = analogRead(DIR_MESURE);
    if (mesure == direction) return;
    int diff = direction-mesure;
    if (diff > 0) {
        digitalWrite(DIR_DROITE,HIGH);
        digitalWrite(DIR_GAUCHE,LOW);
        digitalWrite(DIR_ALLUMER,HIGH);
        delay(map(diff,0,100,5,50));
        digitalWrite(DIR_ALLUMER,LOW);
    } else if (diff < 0) {
        digitalWrite(DIR_DROITE,LOW);
        digitalWrite(DIR_GAUCHE,HIGH);
        digitalWrite(DIR_ALLUMER,HIGH);
        delay(map(abs(diff),0,100,5,50));
        digitalWrite(DIR_ALLUMER,LOW);
    }
}

void propulsion(int vitesse) {
    // vérifier les bornes
    if (vitesse > 255) vitesse = 255;
    if (vitesse < -255) vitesse = -255;
    // marche avant
    if (vitesse > 0) {
        digitalWrite(PROP_ARRIERE, LOW);
        analogWrite(PROP_AVANT, vitesse);
        roule = true;
    } 
    // marche arrière
    else if (vitesse < 0) {
        digitalWrite(PROP_AVANT, LOW);
        analogWrite(PROP_ARRIERE, -vitesse);
        roule = true;
    } 
    // arret
    else {
        digitalWrite(PROP_AVANT, LOW);
        digitalWrite(PROP_ARRIERE, LOW);
        roule = false;
    }
}

#define AVANT (propulsion(PROP_PUISSANCE))
#define ARRIERE (propulsion(-PROP_PUISSANCE))
#define ARRET (propulsion(0))

/* mesure de la distance sur un capteur, en une seule impulsion
 * max : distance maximale
 */
int distance(byte trig, byte echo, int max) {
    // émission de l'impulsion
    digitalWrite(trig,LOW);
    delayMicroseconds(2);
    digitalWrite(trig,HIGH);
    delayMicroseconds(10);
    digitalWrite(trig,LOW);
    // captation du l'echo
    unsigned int delay = pulseIn(echo, HIGH, max);
    if (delay == 0) {
        return max;
    } else {
        return delay;
    }
}

/* mesure de la distance avec trois impulsions, en gardant la valeur médiane
 */
int distanceMediane(byte trig, byte echo, int max) {
    int d, dmin, dmax;
    d = distance(trig,echo,max);
    dmin = d;
    dmax = d;
    d = distance(trig,echo,max);
    if (d<dmin) dmin=d;
    else if (d>dmax) dmax=d;
    d = distance(trig,echo,max);
    if (d<dmin) return dmin;
    if (d>dmax) return dmax;
    return d;
}

#define DIST_AVANT (distanceMediane(TRIG_AVANT,ECHO_AVANT,PROXIMITE))
#define DIST_ARRIERE (distanceMediane(TRIG_ARRIERE,ECHO_ARRIERE,PROXIMITE))
#define DIST_GAUCHE (distanceMediane(TRIG_GAUCHE,ECHO_GAUCHE,PROXIMITE))
#define DIST_DROITE (distanceMediane(TRIG_DROITE,ECHO_DROITE,PROXIMITE))

void testCapteurs(){
    Serial.print("AVANT\t");
    Serial.println(DIST_AVANT);
    Serial.print("ARRIERE\t");
    Serial.println(DIST_ARRIERE);
    Serial.print("GAUCHE\t");
    Serial.println(DIST_GAUCHE);
    Serial.print("DROITE\t");
    Serial.println(DIST_DROITE);
}

void avancer(){
    int d = DIST_AVANT;
    Serial.print("avant ");
    Serial.println(d);
    if (roule && d < PROXIMITE) {
        propulsion(-255);
        delay(500);
        ARRET;
    }
    if (!roule) {
        delay(1000);
    }
    if (!roule && d >= PROXIMITE) {
        propulsion(255);
        delay(500);
        AVANT;
    }
}

void eviterLesMurs(){
    if (!roule) return;
    int g = DIST_GAUCHE;
    int d = DIST_DROITE;
    if (g < PROXIMITE || d < PROXIMITE) {
        // la voiture est proche d'au moins un mur. S'éloigner du mur le plus
        // proche. Si la voiture est dans un couloir, elle cherchera à se
        // placer à égale distance des deux murs.
        if (g < d) {
            Serial.println("mur a gauche !");
            direction = DROITE;
        } else if (d < g) {
            Serial.println("mur a droite !" );
            direction = GAUCHE;
        } else {
            direction = CENTRE;
        }
    } else {
        direction = CENTRE;
    }
}

void setup() {
    Serial.begin(9600);
    Serial.println("setup");
    pinMode(DIR_GAUCHE,OUTPUT);
    pinMode(DIR_DROITE,OUTPUT);
    pinMode(DIR_ALLUMER,OUTPUT);
    pinMode(PROP_AVANT,OUTPUT);
    pinMode(PROP_ARRIERE,OUTPUT);
    pinMode(TRIG_AVANT,OUTPUT);
    pinMode(ECHO_AVANT,INPUT);
    pinMode(TRIG_GAUCHE,OUTPUT);
    pinMode(ECHO_GAUCHE,INPUT);
    pinMode(TRIG_DROITE,OUTPUT);
    pinMode(ECHO_DROITE,INPUT);
    pinMode(TRIG_ARRIERE,OUTPUT);
    pinMode(ECHO_ARRIERE,INPUT);
}

void loop() {
    eviterLesMurs();
    orienter();
    Serial.print(direction);
    Serial.print(" ");
    Serial.println(mesure);
    orienter();
    avancer();
    delay(100);
}


