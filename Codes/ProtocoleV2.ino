#include <VirtualWire.h>

//Code d'identification du groupe
#define GROUPE1 "G13"
#define GROUPE2 "G08"
#define GROUPE3 "G07"
#define ID_DEBUT_TRAME "EXIA" 
#define ID_PC "PCM"


//Ports 
int portReception = 6;//Port de réception radio
int portTransmission = 11; //Port d'envoi radio

//Exemple de trame reçue
                                                                                                                 
//Code d'identification d'un début de trame
char code_DebutTrame[5] = ID_DEBUT_TRAME ;

//Code d'identification d'une destination
char code_Desti[4] = ID_PC ;

//Tableau de correspondance de caractères
char encryptage[63][2] = {
                         //POUR LES MAJUSCULES
                          {'A','O'},
                          {'B','B'},
                          {'C','E'},
                          {'D','V'},
                          {'E','F'},
                          {'F','Z'},
                          {'G','J'},
                          {'H','K'},
                          {'I','W'},
                          {'J','A'},
                          {'K','P'},
                          {'L','I'},
                          {'M','L'},
                          {'N','H'},
                          {'O','Q'},
                          {'P','R'},
                          {'Q','U'},
                          {'R','N'},
                          {'S','M'},
                          {'T','D'},
                          {'U','G'},
                          {'V','Y'},
                          {'W','S'},
                          {'X','T'},
                          {'Y','X'},
                          {'Z','C'},
                        //POUR LES MINUSCULES
                          {'a','f'},
                          {'b','i'},
                          {'c','e'},
                          {'d','a'},
                          {'e','b'},
                          {'f','k'},
                          {'g','d'},
                          {'h','c'},
                          {'i','m'},
                          {'j','l'},
                          {'k','j'},
                          {'l','x'},
                          {'m','g'},
                          {'n','o'},
                          {'o','s'},
                          {'p','z'},
                          {'q','h'},
                          {'r','t'},
                          {'s','p'},
                          {'t','w'},
                          {'u','y'},
                          {'v','u'},
                          {'w','n'},
                          {'x','r'},
                          {'y','q'},
                          {'z','v'},
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
                          {'.','.'},
};
                    

//Crypte un message selon le tableau de comparaison défini
char encrypt(char char_trame){
  for(int i = 0;i< (sizeof(encryptage)/2) ; i ++){
    if( char_trame == encryptage[i][0]){
      char_trame= encryptage[i][1];
      return char_trame;
    } 
  }
}


//Decrypte un message selon le tableau de comparaison défini
char decrypt(char char_trame){
  for(int i = 0;i< (sizeof(encryptage)/2) ; i ++){
    if( char_trame == encryptage[i][1]){
      char_trame= encryptage[i][0];
      return char_trame;
    } 
  }
}




 void setup() {
  Serial.begin(9600);
  Serial.print("Chargement communication ...");
  vw_set_tx_pin(portTransmission); 
  vw_set_rx_pin(portReception);
  vw_setup(2000);
  vw_rx_start();
  Serial.println("Effectué !\n------------------------------");
}


void loop(){
  
  char trameRecue[27]={""};
  String message=getMessage();
  message.toCharArray(trameRecue,sizeof(trameRecue));


//char trameRecue[27]="FTWORELJ68556VQHHFFMff6131"; //Trame cryptée
//char trameRecue[27]="EXIAPCMG13001DONNEESaa1686"; //Trame décryptée
//char trameRecue[27]="QXxAPCMG1E001DONSEESaa1687"; //Trame aléatoire

  if(!trameRecue[25]) {
    Serial.println("Trame non valide");
    return;
  }
//Decrypte le message
  for(int k=0; k<sizeof(trameRecue) -1 ; k++) {
    trameRecue[k]=decrypt(trameRecue[k]);
  }




//Recherche et traitement des données   
  String data=traiterMessage(trameRecue);
  if  (data != "ERROR_IN_TRAME") {
   Serial.print("Données récupérées : ");
   Serial.println(data);
    
//Recrypte le message
  for(int k=0; k<sizeof(trameRecue) -1 ; k++) {
    trameRecue[k]=encrypt(trameRecue[k]);
  }

  accuseReception(trameRecue);
  Serial.println("--------------------------------------------------------\n\n");
  }
  delay(100);
}


String traiterMessage(String trame) {
 
  char    crcLu[4];                 //CalculCRC lu dans la trame
  char    donneetrame[31]={0};      //Données réellement transmises
  char    numeroTrame[3];           //ID de la trame
  int     crcCalcule;               //CRC lu dans la trame
  String  nGrp;                     //Numéro du groupe envoyant le message


  Serial.print("Trame decryptée : ");
  Serial.println(trame);


  
//Si la trame débute par un code de début de trame
 if ( trame[0] == code_DebutTrame[0] && trame[1] == code_DebutTrame[1] && trame[2] == code_DebutTrame[2] && trame[3] == code_DebutTrame[3] ){

//Remplissage des données
    for(int a=0; a < 9; a++){
     donneetrame[a]=trame[a+13];
    }

//Si la trame est bien destinée au PC maître  
    if (trame[4] == code_Desti[0] && trame[5] == code_Desti[1] && trame[6] == code_Desti[2] ) {

//Lecture du numéro de trame et de l'ID de groupe
      numeroTrame[0] = trame[10];
      numeroTrame[1] = trame[11];
      numeroTrame[2] = trame[12];

      nGrp=String(trame[7]) + String(trame[8]) + String(trame[9]);
      
      if(nGrp==(GROUPE1) || nGrp==(GROUPE2) || nGrp==(GROUPE3) ) { //&& trame[8]=='1' && trame[9]=='3') {


//Lecture du code CRC   
        for(int b=0; b<4 ;b++) {
          crcLu[b] = trame[b+22];
        }
        trame[22]='\0';//On place la fin du tableau au début du CalculCRC pour le calcul de l'autre CalculCRC plus tard


//Verification du CRC, puis retourne les données si concluant
        crcCalcule= CalculCRC(trame);
      
        if(atoi(crcLu) == crcCalcule) {
           Serial.println("Trame valide : Calcul du CRC correct -o- Code début conforme -o- Code groupe conforme"); 
           return (char*)donneetrame;
        }
        else {
           Serial.println("CRC différents");    
           Serial.print("CRC lu = ");
           Serial.println(atoi(crcLu));
           Serial.print("CRC calculé = ");
           Serial.println(crcCalcule);
           return "ERROR_IN_TRAME";
         } 
      }
      else {
        Serial.println("La trame ne contient pas un groupe valide\n");
        return "ERROR_IN_TRAME";
      }
    }
    else{
      Serial.println("La trame n'est pas destinée au PC maitre\n");
      return "ERROR_IN_TRAME";
    }
  }
  else {
    Serial.println("La trame ne commence pas par le code de début de trame\n");
    return "ERROR_IN_TRAME";
  }
}




//Envoyer un message
void send (char *message){
    vw_send((uint8_t *)message, strlen(message));
    Serial.print("[Accusé de reception]  Envoi de ");
    Serial.print(message);
    vw_wait_tx(); // Attente de l'envoi
    Serial.println("     -Succés-");
} 


//Reçevoir un message
String getMessage() {
  Serial.println("\nTentative de récupération d'une trame ...");
  byte message[VW_MAX_MESSAGE_LEN]; //Constante VW_MAX_MESSAGE_LEN definie par VirtualWire
  byte taille_message = VW_MAX_MESSAGE_LEN; 
//On attend de recevoir un message
  vw_wait_rx();
  Serial.println("Trame reçue");
  vw_get_message(message, &taille_message) ;
    //Serial.println((char*) message); // Affiche le message
    return (char*) message;
  
}


//Calcul du CRC en fonction des données reçues
int CalculCRC(String tampon) {
  int crcValeur=0;
  for (int i = 0; i < 22; i++) {
    crcValeur += ((int)(tampon[i])) * i ;
  }
  return crcValeur/10;
}





void accuseReception(String trame) {

  //char trameRecue[27]="FTWORELJ68556VQHHFFMff6131"; //Trame cryptée
  //char trameRecue[27]="EXIAPCMG13001DONNEESaa1686"; //Trame décryptée

  trame[4]='N';
  trame[5]='Q';
  trame[6]='B';
  for(int a=0; a<9 ; a++) {
    trame[a+13]='o';
  }


  //Rerypte le message
  for(int k=0; k<9 ; k++) {
    trame[k+13]=encrypt(trame[k+13]);
  }

  char trameTampon[27];
  trame.toCharArray(trameTampon,27);
  
  for( int i=0; i<2; i++ ) {
    send(trameTampon);
    delay(100);
  }
}






