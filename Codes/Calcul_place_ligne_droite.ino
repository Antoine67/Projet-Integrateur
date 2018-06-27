/*
   Code Arduino pour voiture robot

   Copyright (c) 2018 Cesi eXia
   Auteurs     : Groupe 13 + Antoine le schlag
   Création : 08/06/2018
   Dernière modification : 19/06/2018

*/

#include <VirtualWire.h>
#include "Grove_I2C_Motor_Driver.h"

//- Constantes -//
#define I2C_ADDRESS 0x0f
#define CAPTEUR_DROITE 8
#define CAPTEUR_MILIEU_D 7
#define CAPTEUR_MILIEU_G 4
#define CAPTEUR_GAUCHE 6
#define EMETTEUR 5

#define CAPTEUR_IR 9
#define ADC_REF 5


//DEPLACEMENTS
int Capteur_1 = CAPTEUR_DROITE; //droite
int Capteur_2 = CAPTEUR_MILIEU_D; //milieu droite
int Capteur_3 = CAPTEUR_MILIEU_G; //milieu gauche
int Capteur_4 = CAPTEUR_GAUCHE; //gauche
char Ordre ;

//RADIO
int transmit_pin = 3; //port de communication radio
char donneesEnvoi[500] = ""; //Les données à envoyer via les ondes radios

//IR
bool etat;
float temps = 0;


//VITESSE
unsigned int Mesure = 0; //Compte le nombre de mesures réalisées
float trmin = 0; //Vitesse du disque en tour/min
unsigned long tempsTour = millis(); //Temps que met le disque pour réaliser exactement un tour
volatile int tour = 0; //Compte le nombre de tours du disque
float vitesse = 0.5; //Vitesse du robot
const int n = 10; //Nombre de tours du disque avant de faire le calcul de vitesse
float dT;
const int pinCapteur = 2; //on utilise le pin 2 pour le calcul de la vitesse




//CALCUL TAILLE DE LA PLACE
int numerotrame;
bool debutPlace = true; //Permet de vérifier qu'on débute/fini de détécter une place
unsigned long tempsDebutPlace; //Temps auquel on a repérer la place
String msgEnvoi = "EXIAPCMG01" ; //Variable tampon permettant de conserver temporairement le message à stocker
String msgTransfert = ""; //Variable tampon permettant de stocker le message à envoyer
unsigned long tempsDebut; //Temps d'execution du début du programme
int CrC_valeur;
float taillePlace;


//Tableau de correspondance de caractères
char encryptage[62][2] = {
                         //POUR LES MAJUSCULES
                          {'A','N'},
                          {'B','O'},
                          {'C','P'},
                          {'D','Q'},
                          {'E','R'},
                          {'F','S'},
                          {'G','T'},
                          {'H','U'},
                          {'I','V'},
                          {'J','W'},
                          {'K','X'},
                          {'L','Y'},
                          {'M','Z'},
                          {'N','A'},
                          {'O','B'},
                          {'P','C'},
                          {'Q','D'},
                          {'R','E'},
                          {'S','F'},
                          {'T','G'},
                          {'U','H'},
                          {'V','I'},
                          {'W','J'},
                          {'X','K'},
                          {'Y','L'},
                          {'Z','M'},
                        //POUR LES MINUSCULES
                          {'a','n'},
                          {'b','o'},
                          {'c','p'},
                          {'d','q'},
                          {'e','r'},
                          {'f','s'},
                          {'g','t'},
                          {'h','u'},
                          {'i','v'},
                          {'j','w'},
                          {'k','x'},
                          {'l','y'},
                          {'m','z'},
                          {'n','a'},
                          {'o','b'},
                          {'p','c'},
                          {'q','d'},
                          {'r','e'},
                          {'s','F'},
                          {'t','g'},
                          {'u','h'},
                          {'v','i'},
                          {'w','j'},
                          {'x','k'},
                          {'y','l'},
                          {'z','m'},
                        //POUR LES NUMEROS
                          {'0','5'},
                          {'1','6'},
                          {'2','7'},
                          {'3','8'},
                          {'4','9'},
                          {'5','0'},
                          {'6','1'},
                          {'7','2'},
                          {'8','3'},
                          {'9','4'},
};


void setup() {

  //Initialisations
  pinMode(Capteur_1, INPUT);
  pinMode(Capteur_2, INPUT);
  pinMode(Capteur_3, INPUT);
  pinMode(Capteur_4, INPUT);
  pinMode(CAPTEUR_IR, INPUT);

  Motor.begin(I2C_ADDRESS);
  Serial.begin(9600);

  tempsDebut = millis(); //Temps du début du programme

  //Envoi des données
  vw_set_tx_pin(transmit_pin);
  vw_setup(2000);  // initialisation de la bibliothèque VirtualWire à 2000 bauds
  Serial.println("Communication opérationel");

  attachInterrupt(0, nouveauTourInterrupt, CHANGE); //Début des interruptions

  char donnees[40] = "Debut                          "; //Envoie le message à avant de rentrer dans la boucle loop
  send(donnees);
  Serial.println("Message 'Début' envoyé ");
  Serial.println("distance = vitesse x temps donc :");
  Serial.println("Temps x Vitesse = Taille Place \n");


}


// **************************** Fonction principale *****************************//
//                    Verifie l'état des différents capteurs et                  //
//                         déplace le vehicule en fonction                       //
// ******************************************************************************//

int inter = 1;
unsigned long tempsInter = 0;
void loop() {

  // On arrete au bout de 4s de fonctionnement
  unsigned long tempsActuel = millis();
  if ( tempsActuel - tempsDebut > 4000) {
    Serial.println("Arrêt du véhicule");
    arreter();

    //Transformation du String en tableau de caractère
    /*Serial.println("\nDonnées à envoyer :");
      Serial.println(msgTransfert);
      msgTransfert.toCharArray(donneesEnvoi,500);*/


    //Envoi les données à plusieurs reprises, afin d'être sûr de les reçevoir
    //envoyerDonnees();

    //Léger délai avant la fin du programme pour s'assurer que tout a eu le temps de s'effectuer
    delay(1000);
    exit(1);
  }

  //Léger decalage vers la droite
  else if (digitalRead(Capteur_2) == HIGH && digitalRead(Capteur_1) == LOW && digitalRead(Capteur_4) == LOW ) {
    decalerDroite();
  }

  //Léger decalage vers la gauche
  else if (digitalRead(Capteur_3) == HIGH && digitalRead(Capteur_1) == LOW && digitalRead(Capteur_4) == LOW ) {
    decalerGauche();
  }

  //Continuer tout droit
  else if (digitalRead(Capteur_1) == LOW && digitalRead(Capteur_2) == LOW && digitalRead(Capteur_3) == LOW && digitalRead(Capteur_4 ) == LOW) {
    continuer();
  }

  if (digitalRead(Capteur_1 == HIGH) || digitalRead(Capteur_4 == HIGH)) {
    tempsActuel = millis();
    if (tempsActuel - tempsInter > 1500) {
      tempsInter = millis();
      inter++;
    }

  }


  calculVitesse(); // Vitesse du véhicule

  //Récupératuin de l'état du capteur IR
  etat = getEtat();
  //Serial.println(etat);


  //Calcul du temps de détéction d'une place
  //Rentre ici s'il y a un emplacement libre
  if (etat == 0) { //Capteur ne detecte rien
    if (debutPlace == true) {
      tempsDebutPlace = millis(); //On récupère le temps de détection
      debutPlace = false;
    }
  }

  //Rentre ici qd le robot arrive à la fin d'une place
  else if (debutPlace == false ) {

    //Determination du temps passé à parcourir la place
    tempsActuel = millis();
    temps = tempsActuel - tempsDebutPlace;
    temps = temps * 0.001; //En seconde


    //Calcul de la taile de cette place
    taillePlace = temps * vitesse;

    //Affichage en console du calcul réalisé
    /*Serial.print(temps);
      Serial.print(" x ");
      Serial.print(vitesse);
      Serial.print(" = ~");
      Serial.println(taillePlace);*/

    //Stocke cette taille à la suite d'un String
    numerotrame ++;
    msgEnvoi += "00";
    msgEnvoi += numerotrame;
    msgEnvoi += String(vitesse);
    msgEnvoi += int(Ordre);
    if (taillePlace > 0.3) {
      msgEnvoi += String(taillePlace);
    }
    msgEnvoi += Crc(msgEnvoi);
    for(int i = 0; i< sizeof(msgEnvoi);i ++){
        for(int k=0; k<sizeof(msgEnvoi) ; k++) {
    msgEnvoi[k]=encrypt(msgEnvoi[k]);
    }
    msgTransfert += msgEnvoi + "\n";
    //Conversion des données en tablaeau de char puis envoi
    msgTransfert.toCharArray(donneesEnvoi, 500);
    send(donneesEnvoi);
    Serial.println("- - - - - - - -");
    Serial.println(msgTransfert);

    // On reinitialise les variables à reutiliser
    msgEnvoi = "";
    temps = 0;
    numerotrame = 0;
    debutPlace = true;
  }

}

}

String Crc(String tampon) {
  char tampon_char[sizeof(tampon)];
  tampon.toCharArray(tampon_char, sizeof(tampon));

  for (int i = 0; i < sizeof(tampon_char); i++) {
    CrC_valeur += int(tampon_char[i]) * i ;
  }
  return String(CrC_valeur);
}
char encrypt(char trame){
  for(int i = 0;i<= (sizeof(encryptage)/2) ; i ++){
    if( trame == encryptage[i][1]){
      trame= encryptage[i-1][2];
      return trame;
    } 
  }
}

char decrypt(char trame){
  for(int i = 0;i< (sizeof(encryptage)/2) ; i ++){
    if( trame == encryptage[i][2]){
      trame = encryptage[i+1][1];
      return trame;
    }
  }
}
// ***************************** Fonctions de déplacements ****************************** //
//              - continuer()         - tournerGauche()         - tournerDroite()         //
//              - arreter()           - decalerGauche()         - decalerDroite()         //
//                                                                                        //
// ************************************************************************************** //

//Fais avancer le vehicule tout droit
void continuer() {
  Motor.speed(MOTOR2, 60);
  Motor.speed(MOTOR1, 60);

}


//Effectue un virage vers la droite
void tournerDroite() {
  Motor.speed(MOTOR2, -40);
  Motor.speed(MOTOR1, 75);
  delay(150);
  while (digitalRead(Capteur_1) == LOW) {
    Motor.speed(MOTOR2, -40);
    Motor.speed(MOTOR1, 75);
  }
  while (digitalRead(Capteur_2) == LOW) {
    Motor.speed(MOTOR2, -40);
    Motor.speed(MOTOR1, 75);
  }
  Ordre = 'R';
}


//Effectue un virage vers la gauche
void tournerGauche() {
  Motor.speed(MOTOR2, 75);
  Motor.speed(MOTOR1, -40);
  delay(150);
  while (digitalRead(Capteur_4) == LOW) {
    Motor.speed(MOTOR2, 75);
    Motor.speed(MOTOR1, -40);
  }
  while (digitalRead(Capteur_3) == LOW) {
    Motor.speed(MOTOR2, 75);
    Motor.speed(MOTOR1, -40);
  }
  Ordre = 'L';
}


//Permet de recadrer le vehicule vers la gauche
void decalerGauche() {
  Motor.speed(MOTOR2, 75);
  Motor.speed(MOTOR1, -30);
}


//Permet de recadrer le vehicule vers la droite
void decalerDroite() {
  Motor.speed(MOTOR2, -30);
  Motor.speed(MOTOR1, 75);
}


//Permet d'arrêter le vehicule
void arreter() {
  Motor.stop(MOTOR1);
  Motor.stop(MOTOR2);
}







// ************************ Fonction d'envoi des données ************************//
//                     Permet l'envoie de données via des ondes                  //
//                                       radios                                  //
// ******************************************************************************//



//Envoie plusieurs fois les données
void envoyerDonnees() {
  //Envoie 5 fois les données afin d'être sûr d'être reçues
  for (int j = 0; j < 5 ; j++) {
    send(donneesEnvoi);
    delay(500);
  }
}

//Envoi des données via les ondes radios
void send (char *message) {
  vw_send((uint8_t *)message, strlen(message));//Envoi du message
  Serial.println("Envoi des données");
  vw_wait_tx(); //Attendre que le message soit envoyé
}



// ********************** Fonction de calculs de distance ***********************//
//                   Envoie des ondes IR et calcul la distance                   //
//                        en fonction du temps de retour                         //
// ******************************************************************************//



float getEtat()
{
  if (digitalRead(CAPTEUR_IR) == HIGH) {
    return 1;
  }
  else {
    return 0;
  }
}




// ********************** Fonction de calculs de vitesse ************************//
//                      Calcul la vitesse grâce au module                        //
//                               vitesse installé                                //
// ******************************************************************************//


//Permet de calculer la vitesse et d'actualiser la variable associée
void calculVitesse() {

  // Si il y a eu n tours du disque
  if ((tour / 20)  >= n) {

    //Arrêt de l'interruption pendant le calcul
    detachInterrupt(0);



    dT = millis() - tempsTour;//Entre ces deux temps il y a eu exactement 1 tour
    trmin = ((((unsigned long)60000 * n) / (dT)) / 3.4); // Temps écoulé pour faire n tours du disque en minute 1min=60 000ms
    Mesure++;
    vitesse = ((3.141592 * 0.06 * trmin) / 60);//Calcul de la vitesse en fonction des tours par minute

    //Réinitialisation du tempsTour et du nombre de tours
    tempsTour = millis();
    tour = 0;

    //On retablit les interruptions
    attachInterrupt(0, nouveauTourInterrupt, FALLING);
  }
}


//Dès qu'il y a un nouveau tour on appelle  la fonction
void nouveauTourInterrupt() {
  tour++;
}
