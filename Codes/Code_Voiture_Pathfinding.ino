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

// *****************************Constantes****************************** //


//Différents ports utilisés
#define Capteur_Gauche 6
#define Capteur_Milieu_G 4
#define Capteur_Milieu_D 7
#define Capteur_Droite 8
#define I2C_ADRESSE 0x0f

#define DESTINATION 6
// ********************************************************************** //


void setup() {
  
// Initialisations
  Serial.begin(9600);
  pinMode(Capteur_Gauche,INPUT);
  pinMode(Capteur_Milieu_G,INPUT);
  pinMode(Capteur_Milieu_D,INPUT);
  pinMode(Capteur_Droite,INPUT);
  Motor.begin(I2C_ADRESSE);

//Determination de l'itineraire
  initItineraire(DESTINATION);

}


  // **********************Fonction determination d'itineraire*********************//
  //                Initialise le tableau 'itineraire' en fonction                 //
  //                           de la valeur de l'arrivee                           //
  // ******************************************************************************//


//Le tableau qui va stocker l'itineraire à suivre
int itineraire[15];

void initItineraire(int arrivee) {
 
 /* Correspondances des valeurs dans l'itineraire :
  * 0 => Stop
  * 1 => Gauche
  * 2 => Haut
  * 3 => Droite
  * 4 => Bas (Jamais en théorie)
  *  */
  
  switch(arrivee){
    case(1):
      itineraire[0]=2;
      itineraire[1]=1;
      itineraire[2]=2;
      itineraire[3]=1;
      itineraire[4]=0;
      break;
      
      
    case(2):
      itineraire[0]=2;
      itineraire[1]=1;
      itineraire[2]=2;
      itineraire[3]=0;
      break;
      
      
    case(3):
      itineraire[0]=2;
      itineraire[1]=1;
      itineraire[2]=0;
      break;
      
      
      
    case(4):
      itineraire[0]=2;
      itineraire[1]=1;
      itineraire[2]=1;
      itineraire[3]=0;
      break;
      
      
    case(5):
      itineraire[0]=2;
      itineraire[1]=1;
      itineraire[2]=1;
      itineraire[3]=3;
      itineraire[4]=0;
      break;
      
    case(6):
      itineraire[0]=2;
      itineraire[1]=1;
      itineraire[2]=2;
      itineraire[3]=1;
      itineraire[4]=1;
      itineraire[5]=0;
      break;
      
    case(7):
      itineraire[0]=2;
      itineraire[1]=1;
      itineraire[2]=2;
      itineraire[3]=1;
      itineraire[4]=2;
      itineraire[5]=0;
      break;
      
    case(8):
      itineraire[0]=3;
      itineraire[1]=3;
      itineraire[2]=2;
      itineraire[3]=2;
      itineraire[4]=3;
      itineraire[5]=0;
      break;
      
    case(9):
      itineraire[0]=3;
      itineraire[1]=3;
      itineraire[2]=2;
      itineraire[3]=2;
      itineraire[4]=0;
      break;
      
    case(10):
      itineraire[0]=3;
      itineraire[1]=3;
      itineraire[2]=2;
      itineraire[3]=0;
      break;
      
    case(11):
      itineraire[0]=3;
      itineraire[1]=3;
      itineraire[2]=0;
      break;
      
    case(12):
      itineraire[0]=3;
      itineraire[1]=3;
      itineraire[2]=3;
      itineraire[3]=0;
      break;
      
    case(13):
      itineraire[0]=0;
      break;
      
    case(14):
      itineraire[0]=2;
      itineraire[1]=0;
      break;
      
    case(15):
      itineraire[0]=2;
      itineraire[1]=3;
      itineraire[2]=0;
      break;
      
    case(16):
      itineraire[0]=3;
      itineraire[1]=0;
      break;
  }
}



  // *****************************Fonction principale******************************//
  //                    Verifie l'état des différents capteurs et                  //
  //                         déplace le vehicule en fonction                       //
  // ******************************************************************************//

//On initialise la première étape à 0, car c'est un tableau
int etape =0;

void loop(){
  
//Aucun capteur -> tout droit
  if(digitalRead(Capteur_Droite) == LOW && digitalRead(Capteur_Milieu_D) == LOW && digitalRead(Capteur_Milieu_G) == LOW && digitalRead(Capteur_Gauche) == LOW){
    continuer();
  }
//Recalibrage vers la droite
  else if (digitalRead(Capteur_Milieu_D) == HIGH){
    decalerGauche();
  }
  
//Recalibrage vers la gauche
  else if(digitalRead(Capteur_Milieu_G) == HIGH){
    decalerDroite();
  }



//Application de l'itineraire :
//Capteur gauche ou droite ->réaliser l'etape actuelle et passer à l'etape suivante de l'itinéraire
  if ((digitalRead(Capteur_Gauche)==HIGH || digitalRead(Capteur_Droite) == HIGH) && etape<15)
  {
  //Etape = 2 , donc continuer tout droit
    if (itineraire[etape]==2){
      
    //Continuer tout droit tant que le capteur qui a detecté l'intersection ne s'est pas eteint
      while (digitalRead(Capteur_Gauche)==HIGH || digitalRead(Capteur_Droite) == HIGH) {
        continuer();

      //Recalibrage vers la gauche
        if (digitalRead(Capteur_Milieu_D) == HIGH){
          decalerGauche();
        }
  
      //Recalibrage vers la droite
        else if(digitalRead(Capteur_Milieu_G) == HIGH){
          decalerDroite();
        }
      }
        
    }
  //Etape = 3 , donc tourner à droite
    else if (itineraire[etape]==3)
    {
      tournerDroite();
      
    }
  //Etape = 1 , donc tourner à gauche
    else if (itineraire[etape]==1)
    {
      tournerGauche();
    }

  //Etape = 0 , donc arrêt
    else if (itineraire[etape]==0){
      arreter();
    }
    etape+=1;
  }
}


  // *****************************Fonctions de déplacements******************************//
  //              - continuer()         - tournerGauche()         - tournerDroite()      //
  //              - arreter()           - decalerGauche()         - decalerDroite()      //
  //                                                                                     //
  // ************************************************************************************//

void continuer() {
  Motor.speed(MOTOR2, 70);
  Motor.speed(MOTOR1, 70);
}


void tournerDroite() {
  Motor.speed(MOTOR2, -40);
      Motor.speed(MOTOR1,75);
      delay(150);
    while(digitalRead(Capteur_Droite) == LOW){
      Motor.speed(MOTOR2, -40);
      Motor.speed(MOTOR1,75);
      }
     while(digitalRead(Capteur_Milieu_D) == LOW){
      Motor.speed(MOTOR2, -40);
      Motor.speed(MOTOR1,75);}
}


void tournerGauche() {
  Motor.speed(MOTOR2, 75);
     Motor.speed(MOTOR1,-40);
     delay(150);
  while(digitalRead(Capteur_Gauche) == LOW) {
     Motor.speed(MOTOR2, 75);
     Motor.speed(MOTOR1,-40);
  }  
while(digitalRead(Capteur_Milieu_G) == LOW) {
     Motor.speed(MOTOR2, 75);
     Motor.speed(MOTOR1,-40);}
}



void decalerDroite() {
  Motor.speed(MOTOR2, 75);
    Motor.speed(MOTOR1, -30);
}



void decalerGauche() {
  Motor.speed(MOTOR2, -30);
    Motor.speed(MOTOR1, 75);
}


void arreter() {
  Motor.stop(MOTOR1);
  Motor.stop(MOTOR2);
  exit(0);
}

