/*
   Les pins 9 et 11 balancent du courant dans l'arduino, ce qui est notoirement uncool.
   Ajouter des diodes pour empecher ce genre de comportements malvenus.
   */

#define DIR_ALLUMER 9      // allumage du moteur de direction
#define DIR_GAUCHE 12      // HIGH tourne a gauche (noir)
#define DIR_DROITE 13      // HIGH tourne a droite (orange)
#define DIR_MESURE A5      // pin pour la mesure de l'angle des roues
#define DIR_PUISSANCE 255  // [0-255] vitesse a laquelle la direction tourne
#define PROP_ARRIERE 10    // jaune
#define PROP_AVANT 11      // blanc
#define PROP_PUISSANCE 150 // vitesse à laquelle on va

#define TRIG_AVANT 4
#define ECHO_AVANT 5
#define TRIG_GAUCHE 6
#define ECHO_GAUCHE 7
#define TRIG_ARRIERE A0
#define ECHO_ARRIERE A1
#define TRIG_DROITE A2
#define ECHO_DROITE A3

#define MIN_DIRECTION 412
#define MID_DIRECTION 512
#define MAX_DIRECTION 612

bool roule = false;

/* changer la direction des roues pour se rapprocher autant que possible de @dir@ 
 * on ralentit à mesure qu'on se rapproche de la cible.
 */
void direction(int dir) {
    int mes,diff;
    // vérifier les bornes
    if (dir > MAX_DIRECTION) {
        dir = MAX_DIRECTION;
    }
    if (dir < MIN_DIRECTION) {
        dir = MIN_DIRECTION;
    }
    // initier le mouvement des roues
    mes = analogRead(DIR_MESURE);
    diff = dir - mes;
    while (abs(diff) > 2) {
        if (diff > 0) {
            // Serial.println("\tdirection:\tdroite");
            digitalWrite(DIR_DROITE,HIGH);
            digitalWrite(DIR_GAUCHE,LOW);
        } else {
            // Serial.println("\tdirection:\tgauche");
            digitalWrite(DIR_DROITE,LOW);
            digitalWrite(DIR_GAUCHE,HIGH);
        }
        analogWrite(DIR_ALLUMER, DIR_PUISSANCE);
        mes = analogRead(DIR_MESURE);
        diff = dir - mes;
    }
    // arreter le moteur
    analogWrite(DIR_ALLUMER,0);
    // Serial.println("Stop !");
    // Serial.println(analogRead(DIR_MESURE));
}

#define GAUCHE (direction(MIN_DIRECTION))
#define DROITE (direction(MAX_DIRECTION))
#define CENTRE (direction(MID_DIRECTION))

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
        digitalWrite(PROP_AVANT, HIGH);
        digitalWrite(PROP_ARRIERE, HIGH);
        //delay(200);
        //digitalWrite(PROP_AVANT, LOW);
        //digitalWrite(PROP_ARRIERE, LOW);
        roule = false;
    }
}

#define AVANT (propulsion(PROP_PUISSANCE))
#define ARRIERE (propulsion(-PROP_PUISSANCE))
#define ARRET (propulsion(0))

int distance(byte trig, byte echo) {
    //Serial.print("trig:");
    //Serial.println(trig);
    //Serial.print("echo:");
    //Serial.println(echo);
    // émission de l'impulsion
    digitalWrite(trig,LOW);
    delayMicroseconds(2);
    digitalWrite(trig,HIGH);
    delayMicroseconds(10);
    digitalWrite(trig,LOW);
    // captation du l'echo
    unsigned int delay = pulseIn(echo, HIGH, 3000);
    // conversion
    if (delay == 0) {
        return 1000;
    } else {
        return delay/58;
    }
}

#define DIST_AVANT (distance(TRIG_AVANT,ECHO_AVANT))
#define DIST_ARRIERE (distance(TRIG_ARRIERE,ECHO_ARRIERE))
#define DIST_GAUCHE (distance(TRIG_GAUCHE,ECHO_GAUCHE))
#define DIST_DROITE (distance(TRIG_DROITE,ECHO_DROITE))

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

void avancer(int prox){
    int d = DIST_AVANT;
    Serial.print("avant ");
    Serial.println(d);
    if (roule && d < prox) {
        ARRET;
    }
    if (!roule) {
        delay(1000);
    }
    if (!roule && d > prox) {
        propulsion(255);
        delay(500);
        AVANT;
    }
}

void eviterLesMurs(int prox){
    if (!roule) return;
    int g = DIST_GAUCHE;
    int d = DIST_DROITE;
    Serial.print("gauche ");
    Serial.println(g);
    Serial.print("droite ");
    Serial.println(d);
    if (g < prox || d < prox) {
        // la voiture est proche d'au moins un mur. S'éloigner du mur le plus
        // proche. Si la voiture est dans un couloir, elle cherchera à se
        // placer à égale distance des deux murs.
        if (d > g) {
            Serial.println("mur a gauche !");
            DROITE;
            delay(100);
            CENTRE;
        } else if (d < g) {
            Serial.println("mur a droite !" );
            GAUCHE;
            delay(100);
            CENTRE;
        } else {
            Serial.println("mur a la meme distance !");
        }
    }
    Serial.println("murs super loins");
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
    CENTRE;
}

// la distance à partir de laquelle on fait quelque chose
#define PROX_COTE 40
#define PROX_AVANT 50

void loop() {
    avancer(PROX_AVANT);
    //delay(10);
    eviterLesMurs(PROX_COTE);
}
