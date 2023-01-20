#include <arpa/inet.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

struct infoProcessus {
  int numProcessus;
  char adresseIp[50];
  int numPort;
  int couleur;
};

int existCouleur(int tab[], int c, int taille){
  for(int i=0;i<taille; i++){
    if(tab[i]==c) return 1;
  }
  return 0;
}

 int estMonVoisin(int jeton, struct infoProcessus mesVoisin[], int taille){
      for(int i=0;i<taille;i++){
        if(mesVoisin[i].numProcessus==jeton) return 1;
      }
      return 0;
 }

int main(int argc, char *argv[]) {
  if (argc < 4) {
    printf("On attend de vous 4 argument : nom_programme adresseIpServer "
           "numPortServer numPortClient\n");
    return -1;
  }
 
  int idSocket = socket(PF_INET, SOCK_DGRAM, 0);
  if (idSocket < 0) {
    perror("ERROR, failed to create socket \n");
    exit(0);
  }
  // nommage de la socket du client
  struct sockaddr_in adc;
  adc.sin_family = AF_INET;
  adc.sin_addr.s_addr = INADDR_ANY; 
  adc.sin_port = htons((short)atoi(argv[3]));
  int res = bind(idSocket, (struct sockaddr *)&adc, sizeof(adc));

  if (res == -1) {
    perror("ERROR:  au nommage de la socket \n");
  }

  // designer la socket du server
  struct sockaddr_in adresse;
  adresse.sin_family = AF_INET;
  adresse.sin_addr.s_addr = inet_addr(argv[1]);
  adresse.sin_port = htons((short)atoi(argv[2]));
  socklen_t flen = sizeof(struct sockaddr_in);

  int msg = 0;
  if (sendto(idSocket, &msg, sizeof(msg), 0, (struct sockaddr *)&adresse, flen) < 0) {
    printf("ERROR : SENDING MESSAGE");
    return -1;
  }
  printf("Client : message bien envoyé\n");
  int info[5];
  // client reçois son numero processus ainsi que son nombre de voisins
  if (recvfrom(idSocket, &info, sizeof(info), 0, NULL, NULL) < 0) {
    printf("ERREUR 1: RECEIVING MESSAGE FROM SERVER");
    return -1;
  }
  printf("mon numero est : %d , j'ai %d voisinsG et %d voisinsD \n", info[0],
         info[1], info[2]);
  //infoProcessus.numProcessus=info[0];
  int nbrVoisinsG = info[1];
  int nbrVoisinsD = info[2];
  int nbProcessus = info[4];
  
    int m=0;
     if (sendto(idSocket, &m, sizeof(m), 0, (struct sockaddr *)&adresse,
             flen) < 0) {
    printf("ERROR : SENDING MESSAGE");
    return -1;
  }
  //je reçois mes voisins
  struct infoProcessus mesVoisins[nbrVoisinsG];
    if (recvfrom(idSocket, &mesVoisins, sizeof(mesVoisins), 0,(struct sockaddr *)&adresse, &flen) < 0) {
      printf("ERREUR 2: RECEIVING MESSAGE FROM SERVER");
      return -1;
    }
    /*printf("tab de mes voisins gauches recu \n");
    for (int i = 0; i < nbrVoisinsG; i++) {
      printf("voisin %d adresse %s port %d\n", mesVoisins[i].numProcessus,
             mesVoisins[i].adresseIp, mesVoisins[i].numPort);
    }*/
    m=0;
     if (sendto(idSocket, &m, sizeof(m), 0, (struct sockaddr *)&adresse,
             flen) < 0) {
    printf("ERROR : SENDING MESSAGE");
    return -1;
  }
  //je reçois mes voisins droit
  struct infoProcessus mesVoisinsD[nbrVoisinsD];
    if (recvfrom(idSocket, &mesVoisinsD, sizeof(mesVoisinsD), 0,(struct sockaddr *)&adresse, &flen) < 0) {
      printf("ERREUR 2: RECEIVING MESSAGE FROM SERVER");
      return -1;
    }
   
  close(idSocket);

//TCP
  int ds = socket(PF_INET, SOCK_STREAM, 0);
    if (ds == -1) {
      perror("ERROR : CREATION SOCKET\n");
      exit(1);
    }
    //printf("SOCKET CREATED\n");

    // nommer la socket
    struct sockaddr_in ad;
    ad.sin_family = AF_INET;
    ad.sin_addr.s_addr = INADDR_ANY;
    ad.sin_port = htons((short)atoi(argv[3]));
    if (bind(ds, (struct sockaddr *)&ad, sizeof(ad)) < 0) {
      perror("ERROR : erreur au nommage \n");
      return 1;
    }
    // passe le socket en mode ecoute
    if (listen(ds, nbrVoisinsD) < 0) {
      printf("ERROR: ne peut pas passer en mode ecoute\n");
      exit(1);
    }
    //printf("je passe en mode ecoute\n");

      //struct infoProcessus voisinsServer[nbrVoisinsD];
      int nbVoisinsDroite = nbrVoisinsD;
      int acpt;
      int socket_ids[nbrVoisinsD];
      int a=0;
      if(nbrVoisinsD!=0){
        while(nbVoisinsDroite>0){
          struct sockaddr_in sClient;
          socklen_t lAdC = sizeof(struct sockaddr_in);
          acpt = accept(ds, (struct sockaddr *)&sClient, &lAdC);
          if (acpt < 0) {
            printf("ERROR : accept\n");
            exit(1);
          } 
          socket_ids[a]=acpt;
          a++;
          int num;
          if (recv(acpt, &num, sizeof(num), 0) < 0) {
            perror("ERROR: message not received\n");
          }
          printf("Message received from : %d\n", num);
          nbVoisinsDroite--;
      }
    }

    //client
    int monNumero;
    int couleur = info[3];
    if(nbrVoisinsG!=0){
      //close(ds);
    for (int i = 0; i < nbrVoisinsG; i++) {
        int dss = socket(PF_INET, SOCK_STREAM, 0);
        if (dss == -1) {
          perror("ERROR : CREATION SOCKET\n");
          exit(1);
        }
      struct sockaddr_in aD;
      aD.sin_family = AF_INET;
      aD.sin_addr.s_addr = inet_addr(mesVoisins[i].adresseIp);
      aD.sin_port = htons((short)mesVoisins[i].numPort);
      socklen_t lgA = sizeof(struct sockaddr_in);
      // demande de connexion
      sleep(2);
      if (connect(dss, (struct sockaddr *)&aD, lgA) < 0) {
        printf("ERROR: CONNEXION REFUSED\n");
      }
      monNumero = info[0];
      if (send(dss, &monNumero, sizeof(monNumero), 0) < 0) {
        printf("ERROR: MESSAGE NOT SENT TO : %d", mesVoisins[i].numProcessus);
        return -1;
      }
      printf("message sent to %d\n", mesVoisins[i].numProcessus);
      close(dss);
    }
    }
    
    //if(nbrVoisinsD!=0) 
    //close(ds);

//coloration
    // passe le socket en mode ecoute
    if (listen(ds, nbrVoisinsD) < 0) {
      printf("ERROR: ne peut pas passer en mode ecoute\n");
      exit(1);
    }
    printf("je passe en mode ecoute\n");

      int nbVoisinsDroitee = nbrVoisinsD;
      int l=0;
      int couleurDispo = 0;
      int taille = nbrVoisinsD+nbrVoisinsG;
      int tabCouleur[taille];
      if(nbrVoisinsD!=0){
       
              for(int i=0;i<nbrVoisinsD;i++){
                int color;
                if (recv(socket_ids[i], &color, sizeof(color), 0) < 0) {
                  perror("ERROR: message not received\n");
                }
                //printf("color received : %d\n", color); 
                tabCouleur[l] = color;
                l++;
              }

        while(existCouleur(tabCouleur,couleurDispo,taille)!=0) couleurDispo++;
        couleur = couleurDispo;
        //envoyer ma couleur a mes voisins
        for(int i=0;i<nbrVoisinsD;i++){
          if(send(socket_ids[i], &couleur, sizeof(int),0)<0){
            perror("ERROR : color not sent");
            return 1;
          }
          //printf("j'envoie ma couleur %d\n",couleur);
        }
        printf("Ma couleur est %d\n",couleur);
      }

    //client
    if(nbrVoisinsG!=0){
      //close(ds);
      for (int i = 0; i < nbrVoisinsD; i++) {
        if (send(socket_ids[i], &couleur, sizeof(couleur), 0) < 0) {
          printf("ERROR: COLOR NOT SENT \n");
          return -1;
        }
        printf("color %d sent\n", couleur);
        //je reçois la couleur de mon voisin
        int col;
        if(recv(socket_ids[i],&col, sizeof(col),0)<0){
          perror("ERROR : color not received\n");
          exit(1);
        }
        printf("color received %d\n",col);
        tabCouleur[l]=col;
        l++;
        printf("color %d received\n",tabCouleur[l]);
      }
      while(existCouleur(tabCouleur,couleurDispo,taille)!=0) couleurDispo++;
      couleur = couleurDispo;
      printf("ma couleur est %d\n",couleur);
    }

    //if(nbrVoisinsD!=0) 
    close(ds);


}













  

 

   