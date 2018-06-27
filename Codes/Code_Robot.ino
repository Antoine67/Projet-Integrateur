/* 
 * Code Arduino pour voiture robot
 *
 * Copyright (c) 2018 Cesi eXia
 * Auteurs     : Groupe 13
 * Création : 08/06/2018
 * Dernière modification : 15/06/2018
 *
 */


#include "Grove_I2C_Motor_Driver.h"

//- Constantes -//
#define I2C_ADDRESS 0x0f
#define CAPTEUR_DROITE 8
#define CAPTEUR_MILIEU_D 7
#define CAPTEUR_MILIEU_G 4
#define CAPTEUR_GAUCHE 6

int Capteur_1 = CAPTEUR_DROITE; //droite
int Capteur_2 = CAPTEUR_MILIEU_D; //milieu droite
int Capteur_3 = CAPTEUR_MILIEU_G; //milieu gauche
int Capteur_4 = CAPTEUR_GAUCHE; //gauche



void setup() {

  //Initialisations
  pinMode(Capteur_1, INPUT);
  pinMode(Capteur_2, INPUT);
  pinMode(Capteur_3, INPUT);
  pinMode(Capteur_4, INPUT);
  
  Motor.begin(I2C_ADDRESS);

  Serial.begin(9600);
  Serial.println("ICI");
}


  // **************************** Fonction principale *****************************//
  //                    Verifie l'état des différents capteurs et                  //
  //                         déplace le vehicule en fonction                       //
  // ******************************************************************************//


void loop() {

//Léger decalage vers la droite
  if (digitalRead(Capteur_2) == HIGH){
    decalerDroite();
  }

//Léger decalage vers la gauche
  else if(digitalRead(Capteur_3) == HIGH){
    decalerGauche();
  }
  
//Continuer tout droit
  else if(digitalRead(Capteur_1) == LOW && digitalRead(Capteur_2) == LOW && digitalRead(Capteur_3) == LOW && digitalRead(Capteur_4 ) == LOW){
    continuer();
  }
  
  
//Tourner à droite 
 else if (digitalRead(Capteur_1) == HIGH || digitalRead(Capteur_1) == HIGH && digitalRead(Capteur_2) == HIGH || digitalRead(Capteur_1) == HIGH && digitalRead(Capteur_2) == HIGH && digitalRead(Capteur_3) == HIGH){
      tournerDroite();
 }
 

//Tourner à gauche
 else if (digitalRead(Capteur_4) == HIGH || digitalRead(Capteur_4) == HIGH && digitalRead(Capteur_3) == HIGH || digitalRead(Capteur_4) == HIGH && digitalRead(Capteur_3) == HIGH && digitalRead(Capteur_2) == HIGH) {

    tournerGauche();
  }
}




  // ***************************** Fonctions de déplacements ****************************** //
  //              - continuer()         - tournerGauche()         - tournerDroite()         //
  //              - arreter()           - decalerGauche()         - decalerDroite()         //
  //                                                                                        //
  // ************************************************************************************** //


void continuer() {
  Motor.speed(MOTOR2, 60);
  Motor.speed(MOTOR1, 60);
}


void tournerDroite() {
  Motor.speed(MOTOR2, -40);
  Motor.speed(MOTOR1,75);
  delay(150);
  while(digitalRead(Capteur_1) == LOW){
    Motor.speed(MOTOR2, -40);
    Motor.speed(MOTOR1,75);
  }
  while(digitalRead(Capteur_2) == LOW){
    Motor.speed(MOTOR2, -40);
    Motor.speed(MOTOR1,75);}
}

void tournerGauche() {
  Motor.speed(MOTOR2, 75);
  Motor.speed(MOTOR1,-40);
  delay(150);
  while(digitalRead(Capteur_4) == LOW) {
     Motor.speed(MOTOR2, 75);
     Motor.speed(MOTOR1,-40);
  }  
  while(digitalRead(Capteur_3) == LOW) {
     Motor.speed(MOTOR2, 75);
     Motor.speed(MOTOR1,-40);}
}

void decalerGauche() {
   Motor.speed(MOTOR2, 75);
    Motor.speed(MOTOR1, -30);
}

void decalerDroite() {
  Motor.speed(MOTOR2, -30);
    Motor.speed(MOTOR1, 75);
}



void arreter() {
  Motor.stop(MOTOR1);
  Motor.stop(MOTOR2);
  exit(0);
}


/*
 * int alea=random(-1,2);
 
  void intersection(int alea) {
  if (alea) {
    tournerDroite(true);
  }
  else {
    tournerGauche(true);
  }
}
 */

