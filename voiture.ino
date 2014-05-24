/*
   Les pins 9 et 11 balancent du courant dans l'arduino, ce qui est notoirement uncool.
   Ajouter des diodes pour empecher ce genre de comportements malvenus.
   */

#define DIR_ALLUMER 9     // allumage du moteur de direction
#define DIR_GAUCHE 12     // HIGH tourne a gauche (noir)
#define DIR_DROITE 13     // HIGH tourne a droite (orange)
#define DIR_MESURE A5     // pin pour la mesure de l'angle des roues
#define DIR_PUISSANCE 100 // [0-255] vitesse a laquelle la direction tourne
#define DIR_DELAI 50      // delai entre deux mesure de l'angle des roues
#define PROP_ARRIERE 10   // jaune
#define PROP_AVANT 11     // blanc

#define TRIG_AVANT 7
#define ECHO_AVANT 6
#define TRIG_GAUCHE 5
#define ECHO_GAUCHE 4
#define TRIG_ARRIERE 3
#define ECHO_ARRIERE 2
#define TRIG_DROITE 1
#define ECHO_DROITE 0

#define DROITE 1
#define GAUCHE -1

int min_direction = 400;
int max_direction = 700;
int dir; // position cible des roues
int run; // compteur pour la loop

/* calibrer la direction :
 * faire tourner les roues à gauche jusqu'à ce que la mesure ne change plus
 * noter le résultat dans min_direction
 * faire pareil avec la droite.
 */
void calibrer() {
    Serial.println("debut de la calibration");
    int courant,precedent;
    // calibrer à droite
    digitalWrite(DIR_DROITE,HIGH);
    digitalWrite(DIR_GAUCHE,LOW);
    analogWrite(DIR_ALLUMER,DIR_PUISSANCE);
    precedent = analogRead(DIR_MESURE);
    while (true) {
        delay(DIR_DELAI);
        courant = analogRead(DIR_MESURE);
        if (courant-precedent > 0) {
            precedent = courant;
        }
        else {
            max_direction = courant;
            break;
        }
    }
    digitalWrite(DIR_ALLUMER,LOW);
    Serial.println(String("max_direction:")+max_direction);
    delay(1000);
    // calibrer à gauche
    digitalWrite(DIR_DROITE,LOW);
    digitalWrite(DIR_GAUCHE,HIGH);
    analogWrite(DIR_ALLUMER,DIR_PUISSANCE); // régler la vitesse ici
    while (true) {
        delay(DIR_DELAI);
        courant = analogRead(DIR_MESURE);
        if (courant-precedent < 0) {
            precedent = courant;
        } 
        else {
            min_direction = courant;
            break;
        }
    }
    analogWrite(DIR_ALLUMER,LOW);
    Serial.println(String("min_direction:")+min_direction);
    Serial.println("Calibration terminee");
}

/* changer la direction des roues pour se rapprocher autant que possible de @dir@ 
 * on ralentit à mesure qu'on se rapproche de la cible.
 */
//void orienter(int dir) {
    //int mes,diff;
    //if (dir > max_direction) {
        //dir = max_direction;
    //}
    //if (dir < min_direction) {
        //dir = min_direction;
    //}  
    //mes = analogRead(DIR_MESURE);
    //diff = dir - mes;
    //Serial.println("orientation:");    
    //Serial.println(String("\tmesure:\t") + mes);
    //Serial.println(String("\tobjectif:\t") + dir);
    //// bouger vers la droite
    //if (diff > 1) {
        //Serial.println("\tdirection:\tdroite");
        //digitalWrite(DIR_DROITE,HIGH);
        //digitalWrite(DIR_GAUCHE,LOW);
    //}
    //// bouger vers la gauche
    //if (diff < -1) {
        //Serial.println("\tdirection:\tgauche");
        //digitalWrite(DIR_DROITE,LOW);
        //digitalWrite(DIR_GAUCHE,HIGH);
    //}
    //// commencer le mouvement
    //analogWrite(DIR_ALLUMER, DIR_PUISSANCE);
    //while (abs(diff) > 2) {
        //mes = analogRead(DIR_MESURE);
        //diff = dir - mes;
    //}
    //// arreter le moteur
    //analogWrite(DIR_ALLUMER,0);
    //Serial.println("Stop !");
    //Serial.println(analogRead(DIR_MESURE));
//}

void tourner(int dir, int dur=1){
    if(dir > 0) {
        if (analogRead(DIR_MESURE) >= max_direction) return;
        digitalWrite(DIR_GAUCHE,LOW);
        digitalWrite(DIR_DROITE,HIGH);
    } else {
        if (analogRead(DIR_MESURE) <= min_direction) return;
        digitalWrite(DIR_DROITE,LOW);
        digitalWrite(DIR_GAUCHE,HIGH);
    }
    analogWrite(DIR_ALLUMER, DIR_PUISSANCE);
    delay(DIR_DELAI*dur);
    analogWrite(DIR_ALLUMER, 0);
}


/*   0   = stop
     255 = Full speed ahead !
     -255 = Full speed backward !
     */
void avancer(int vitesse) {
    // borner
    if (vitesse > 255) vitesse = 255;
    if (vitesse < -255) vitesse = -255;
    // marche avant
    if (vitesse > 0) {
        digitalWrite(PROP_ARRIERE, LOW);
        analogWrite(PROP_AVANT, vitesse);
    } 
    // marche arrière
    else if (vitesse < 0) {
        digitalWrite(PROP_AVANT, LOW);
        analogWrite(PROP_ARRIERE, -vitesse);
    } 
    // arret
    else {
        digitalWrite(PROP_AVANT, LOW);
        digitalWrite(PROP_ARRIERE, LOW);
    }
}

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
    unsigned int delay = pulseIn(echo, HIGH);
    // conversion
    return delay/58;
}

void milieu() {
    int milieu=512;
    int val = analogRead(DIR_MESURE);
    while (abs(val-milieu) > 20) {
        if (val < milieu) tourner(DROITE);
        else tourner(GAUCHE,1);
        val = analogRead(DIR_MESURE);
        Serial.println(val);
    }
}

void chicane() {
    avancer(256);
    delay(1000);
    tourner(DROITE,10);
    delay(1000);
    tourner(GAUCHE,10);
    avancer(0);
}

void setup() {
    Serial.begin(9600);
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
    //calibrer();
    milieu();
    run = 1;
}

void loop() { 
    //Serial.print("avant: ");
    //Serial.println(distance(TRIG_AVANT,ECHO_AVANT));
    //Serial.print("gauche: ");
    //Serial.println(distance(TRIG_GAUCHE,ECHO_GAUCHE));
    //Serial.print("droite: ");
    //Serial.println(distance(TRIG_DROITE,ECHO_DROITE));
    //Serial.print("arriere: ");
    //Serial.println(distance(TRIG_ARRIERE,ECHO_ARRIERE));
    //rouler(255);
    //delay(1000);
    chicane();
    delay(1000);
}

