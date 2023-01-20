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

struct infoProcessus {
  int numProcessus;
  char adresseIp[50];
  int numPort;
  int couleur;
};
int getNbVoisinsGauche(int couples[][2], int nbCouples, int numProc) {
  int nbVoisins = 0;
  for (int i = 0; i < nbCouples; i++) {
    if (couples[i][1] == numProc)
      nbVoisins++;
  }
  return nbVoisins;
}

int getNbVoisinsDroite(int couples[][2], int nbCouples, int numProc) {
  int nbVoisins = 0;
  for (int i = 0; i < nbCouples; i++) {
    if (couples[i][0] == numProc)
      nbVoisins++;
  }
  return nbVoisins;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("waiting for 2 arguments : programName fileName \n");
    return -1;
  }

  //*******************************************LECTURE DU
  // FICHIER***********************************************
  FILE *fichier;
  fichier = fopen(argv[1], "r");
  int taille_max = 255;
  char ligne[taille_max];
  char *strToken;
  int nbProcessus, i, j, nbCouples;
  char t[256];
  char texte[1000000];

  if (fichier == NULL) {
    printf("Error while opening file");
    exit(1);
  }

  // Récupérer le nombre de processus et le nombre de liaisons
  while (fichier != NULL && fgets(ligne, taille_max, fichier) != NULL) {
    // On recupere chaque ligne du fichier entré en paramêtre
    if (ligne[0] == 'p') {
      strToken = strtok(ligne, " ");
      strToken = strtok(NULL, " ");
      strToken = strtok(NULL, " ");
      nbProcessus = atoi(strToken);
      printf("Le nombre de processus : %d\n", nbProcessus);
      strToken = strtok(NULL, " ");
      nbCouples = atoi(strToken);
      printf("Le nombre de liaisons : %d\n", nbCouples);
      break;
    }
  }

  // on recupere les couples (les aretes)
  int couples[nbCouples / 2][2];
  int cpt = 0;
  while (fichier != NULL && fgets(ligne, taille_max, fichier) != NULL) {
    // On récupère chaque ligne du fichier entré en paramêtre
    if (ligne[0] == 'e') {
      strToken = strtok(ligne, " ");
      strToken = strtok(NULL, " ");
      i = atoi(strToken);
      strToken = strtok(NULL, " ");
      j = atoi(strToken);
      couples[cpt][0] = i;
      couples[cpt][1] = j;
      cpt++;
    }
  }
  fclose(fichier);

  int idSocket = socket(PF_INET, SOCK_DGRAM, 0);
  if (idSocket < 0) {
    perror("ERROR, failed to create socket \n");
    exit(0);
  }
  printf("creation de la socket reussie\n");

  // nommer la socket du serveur
  struct sockaddr_in adresse;
  adresse.sin_family = AF_INET;
  adresse.sin_addr.s_addr = INADDR_ANY; // A PASSER EN PARAM APRES
  adresse.sin_port = htons((short)3000);
  int resBind = bind(idSocket, (struct sockaddr *)&adresse, sizeof(adresse));
  if (resBind < 0) {
    printf("ERROR BIND\n");
    return -1;
  }
  printf("serveur : nommage reussi\n");

  i = 0;
  // struct infoProcessus infoProc;
  struct infoProcessus tabAllProcs[nbProcessus];
  /*********************ETAPE 1****************/
  while (i < nbProcessus) {
    struct sockaddr_in client;
    socklen_t flen = sizeof(struct sockaddr_in);
    int msg;
    if (recvfrom(idSocket, &msg, sizeof(msg), 0, (struct sockaddr *)&client,
                 &flen) < 0) {
      printf("ERREUR : RECEIVING MESSAGE FROM CLIENT\n");
      return -1;
    }
    // infoProc.numProcessus = i;
    tabAllProcs[i].numProcessus = i + 1;
    strcpy(tabAllProcs[i].adresseIp, inet_ntoa(client.sin_addr));
    tabAllProcs[i].numPort = ntohs(client.sin_port);
    tabAllProcs[i].couleur = 0;

    printf("receiving msg from proc %d, ip adresse %s port %d\n", i + 1,
           inet_ntoa(client.sin_addr), ntohs(client.sin_port));
    i++;
  }

  /*********************ETAPE 2****************/
  int nbVoisinsG = 0;
  int nbVoisinsD = 0;

  int numProc = 0;
  char ack[20];
  int info[5];
  int k,d;

  //envoyer aux clients
  for (int i = 0; i < nbProcessus; i++) {
    numProc = tabAllProcs[i].numProcessus;
    nbVoisinsG = getNbVoisinsGauche(couples, nbCouples / 2, numProc);
    nbVoisinsD = getNbVoisinsDroite(couples, nbCouples / 2, numProc);
        k = 0;d=0;
        struct sockaddr_in proc;
        proc.sin_family = PF_INET;
        proc.sin_addr.s_addr = inet_addr(tabAllProcs[i].adresseIp);
        proc.sin_port = htons((short)tabAllProcs[i].numPort);
        numProc = tabAllProcs[i].numProcessus;

        // envoi au processus son numero
        info[0] = numProc;
        info[1] = nbVoisinsG;
        info[2] = nbVoisinsD;
        info[3] = 0;
        info[4] = nbProcessus;
        if (sendto(idSocket, &info, sizeof(info), 0, (struct sockaddr *)&proc,
                  sizeof(struct sockaddr_in)) < 0) {
          printf("ERROR : SENDING NUMPROC AND NBVOISINS TO CLIENT\n");
          return -1;
        }
        printf("Message envoyé a %d\n", numProc);
        int m;
        socklen_t fle = sizeof(struct sockaddr_in);
        if (recvfrom(idSocket, &m, sizeof(m), 0, (struct sockaddr *)&proc,&fle) < 0) {
          printf("ERREUR : RECEIVING MESSAGE FROM CLIENT\n");
          return -1;
        }
        struct infoProcessus tabVoisins[nbVoisinsG];
        struct infoProcessus tabVoisinsD[nbVoisinsD];
        for (int j = 0; j < nbCouples / 2; j++) {
          if (numProc == couples[j][1]) {
              int index = couples[j][0] - 1;
              tabVoisins[k].numProcessus = couples[j][0];
              tabVoisins[k].numPort = tabAllProcs[index].numPort;
              strcpy(tabVoisins[k].adresseIp, tabAllProcs[index].adresseIp);
              tabVoisins[k].couleur = 0;
              k++;
          }
           if (numProc == couples[j][0]) {
              int index = couples[j][1] - 1;
              tabVoisinsD[d].numProcessus = couples[j][1];
              tabVoisinsD[d].numPort = tabAllProcs[index].numPort;
              strcpy(tabVoisinsD[k].adresseIp, tabAllProcs[index].adresseIp);
              d++;
          }

        }
        // on envoie au processus les informations de tous
        // ses voisins
        if (sendto(idSocket, &tabVoisins, sizeof(tabVoisins), 0,
                    (struct sockaddr *)&proc, sizeof(struct sockaddr_in)) < 0) {
          printf("ERROR : SENDING MESSAGE TO CLIENT %d\n", numProc);
          return -1;
        }
         if (recvfrom(idSocket, &m, sizeof(m), 0, (struct sockaddr *)&proc,&fle) < 0) {
          printf("ERREUR : RECEIVING MESSAGE FROM CLIENT\n");
          return -1;
        }
        if (sendto(idSocket, &tabVoisinsD, sizeof(tabVoisinsD), 0,
                    (struct sockaddr *)&proc, sizeof(struct sockaddr_in)) < 0) {
          printf("ERROR : SENDING MESSAGE TO CLIENT %d\n", numProc);
          return -1;
        }

        /*printf("tab voisins envoye a %d \n", numProc);
        for (int m = 0; m < nbVoisinsG; m++) {
          printf("numero %d , adresse %s , port %d\n", tabVoisins[m].numProcessus,
          tabVoisins[m].adresseIp, tabVoisins[m].numPort);
        }*/
}
  close(idSocket);

  }