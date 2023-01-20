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
    /*printf("tab de mes voisins droit recu \n");
    for (int i = 0; i < nbrVoisinsD; i++) {
      printf("voisin %d adresse %s port %d\n", mesVoisinsD[i].numProcessus,
             mesVoisinsD[i].adresseIp, mesVoisinsD[i].numPort);
    }*/

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
        /*fd_set read_fd_set, set;
        FD_ZERO(&set);
        int highest_fd;
        for (int i = 0; i <nbrVoisinsD; i++) {
           highest_fd = socket_ids[i];
            FD_SET(socket_ids[i], &set);
            if (socket_ids[i] > highest_fd)
                highest_fd =socket_ids[i];
        }
        read_fd_set = set;
        int select_result;
        do {
            select_result = select(highest_fd + 1, &read_fd_set, NULL, NULL, NULL);
        } while ((select_result == -1) && (errno == EINTR));
  
        if (select_result < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < nbrVoisinsD; i++) {
            if (FD_ISSET(socket_ids[i], &read_fd_set)) {
                char read_buffer[nbrVoisinsD][30];
                int color=100;
                if (recv(socket_ids[i],  &color, sizeof(color), 0) < 0) {
                    perror("recv");
                }
                printf("color recu %d\n",color);
                tabCouleur[l]=color;
                l++;
                FD_CLR(socket_ids[i], &set);           
              }
            }*/

            /*while(nbVoisinsDroitee>0){
              /*struct sockaddr_in sClient;
              socklen_t lAdC = sizeof(struct sockaddr_in);
              acpt = accept(ds, (struct sockaddr *)&sClient, &lAdC);
              if (acpt < 0) {
                printf("ERROR : accept\n");
                exit(1);
              } 
              tabAccept[a]=acpt;
              a++;*/
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
          /*int dss = socket(PF_INET, SOCK_STREAM, 0);
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
        }*/
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













    /*********************************************COLORIAGE***********************************/
   /* 
  //je reçois jeton du server 
  int jeton;
  int totalVoisins = nbrVoisinsD+nbrVoisinsG;
  int s=0;
  struct infoProcessus tousMesVoisins[totalVoisins];
    for(int j=0;j<nbrVoisinsG;j++){
      tousMesVoisins[j]=mesVoisins[j];
    }
    for(int j=nbrVoisinsG;j<totalVoisins;j++){
      tousMesVoisins[j] = mesVoisinsD[s];
      s++;
    }
    for(int i=0;i<totalVoisins;i++){
      printf("voisin %d ip %s port %d\n",tousMesVoisins[i].numProcessus,tousMesVoisins[i].adresseIp,tousMesVoisins[i].numPort);
    }*/

  /*int sC = socket(PF_INET, SOCK_STREAM, 0);
    if (sC == -1) {
      perror("ERROR : CREATION SOCKET\n");
      exit(1);
    }
    printf("SOCKET CREATED\n");


    if (bind(sC, (struct sockaddr *)&ad, sizeof(ad)) < 0) {
      perror("ERROR : erreur au nommage \n");
      return 1;
    }*/
    // passe le socket en mode ecoute
    /*if (listen(ds, 1) < 0) {
      printf("ERROR: ne peut pas passer en mode ecoute\n");
      exit(1);
    }
    printf("je passe en mode ecoute\n");
    struct sockaddr_in sClient;
    socklen_t lAdC = sizeof(struct sockaddr_in);
    int accpt = accept(ds, (struct sockaddr *)&sClient, &lAdC);
    if (accpt < 0) {
      printf("ERROR : accept\n");
      exit(1);
    } 
    if (recv(accpt, &jeton, sizeof(jeton), 0) < 0) {
      perror("ERROR: jeton not received\n");
    }
    printf("jeton %d received \n", jeton);

  int l=0;
    while(jeton < nbProcessus){
      if(jeton==info[0]){ // ici jeton  va te dire si c'est ton tour d'envoyer à tout le monde ou non 
         // passe le socket en mode ecoute
        if (listen(ds, totalVoisins) < 0) {
          printf("ERROR: ne peut pas passer en mode ecoute\n");
          exit(1);
        }
        printf("je passe en mode ecoute\n");
        int totV=totalVoisins;
        int tabCouleur[totalVoisins];
        if(nbrVoisinsD==0){
        while(totV>0){
          struct sockaddr_in clients;
          socklen_t lAdC = sizeof(struct sockaddr_in);
          int acptt = accept(ds, (struct sockaddr *)&clients, &lAdC);
          if(acptt<0){
            printf("ERROR : accept\n");
            exit(1);
          } 
          printf("cccccc");
          int colorV;
          if (recv(acptt, &colorV, sizeof(colorV), 0) < 0) {
            perror("ERROR: color not received\n");
          }
          //ajouter les couleurs dans mon tableau 
          tabCouleur[l]=colorV;
          l++;
          totV--;
        }
        }else{
          while(totV>0){
            int colorV;
            if (recv(acpt, &colorV, sizeof(colorV), 0) < 0) {
              perror("ERROR: color not received\n");
            }
            //ajouter les couleurs dans mon tableau 
            tabCouleur[l]=colorV;
            l++;
            totV--;
         }

        }*/
        /*une fois j'ai reçu toutes les couleurs de mes voisins j'envoie au server un msg pour dire que j'ai fini ici le server va incrémenter le jeton et l'envoyer à tout le monde de nouveau */
        /*struct sockaddr_in serv;
        serv.sin_family = AF_INET;
        serv.sin_addr.s_addr = inet_addr(argv[1]);
        serv.sin_port = htons((short)atoi(argv[2]));
        socklen_t lgA = sizeof(struct sockaddr_in);
        
       sleep(2);
        if (connect(ds, (struct sockaddr *)&serv, lgA) < 0) {
          printf("ERROR: CONNEXION REFUSED\n");
        }*/
        /*monNumero = info[0];
        if (send(ds, &monNumero, sizeof(monNumero), 0) < 0) {
          printf("ERROR: MESSAGE NOT SENT TO ");
          return -1;
        }
        int couleurDispo=0;
        while(existCouleur(tabCouleur,couleurDispo,totalVoisins)!=0) couleurDispo++;
        couleur = couleurDispo;
        printf("message de terminaison envoyé\n");
        printf("ma couleur est %d\n",couleur);
       
      }else { // la variabe globale n'est pas egale à mon numero de processus donc c'est pas mon tour pour demander les couleurs des voisins et choisir une couleur
        //c'est la que je vais recevoir les demandes des couleurs de mon voisin
        if(estMonVoisin(jeton, tousMesVoisins, totalVoisins)==1){
          if(nbrVoisinsG==0){
            char ip[50]="";
            int portV;
            for(int i=0;i<totalVoisins;i++){
              if(tousMesVoisins[i].numProcessus==jeton){
                strcpy(ip,tousMesVoisins[i].adresseIp);
                portV = tousMesVoisins[i].numPort;
              }
            }
            
              struct sockaddr_in vois;
              vois.sin_family = AF_INET;
              vois.sin_addr.s_addr = inet_addr(ip);
              vois.sin_port = htons((short)portV);
              socklen_t lgA = sizeof(struct sockaddr_in);
              // demande de connexion
              sleep(10);
              if (connect(ds, (struct sockaddr *)&vois, lgA) < 0) {
                printf("ERROR: CONNEXION REFUSED\n");
              }
              if (send(ds, &couleur, sizeof(couleur), 0) < 0) {
                printf("ERROR: COLOR NOT SENT TO ");
                return -1;
              }
          }else{
             if (send(ds, &couleur, sizeof(couleur), 0) < 0) {
                printf("ERROR: COLOR NOT SENT TO ");
                return -1;
              }
          }
        }
      }*/
      //recevoir le nouveau jeton
            /*close(ds);
            int ds = socket(PF_INET, SOCK_STREAM, 0);
            if (ds == -1) {
              perror("ERROR : CREATION SOCKET\n");
              exit(1);
            }
            printf("SOCKET CREATED\n");

            if (bind(ds, (struct sockaddr *)&ad, sizeof(ad)) < 0) {
              perror("ERROR : erreur au nommage \n");
              return 1;
            }*/
            // passe le socket en mode ecoute
            
            /*if (listen(ds, 1) < 0) {
              printf("ERROR: ne peut pas passer en mode ecoute\n");
              exit(1);
            }
            printf("je passe en mode ecoute\n");
            struct sockaddr_in sClient;
            socklen_t lAdC = sizeof(struct sockaddr_in);
            acpt = accept(ds, (struct sockaddr *)&sClient, &lAdC);
            if (acpt < 0) {
              printf("ERROR : accept\n");
              exit(1);
            }*/ 
            /*if (recv(accpt, &jeton, sizeof(jeton), 0) < 0) {
              perror("ERROR: jeton 2 not received\n");
            }
            printf("jeton %d received \n", jeton);
    }

close(ds);*/
















  /*int l=0;
    struct infoProcessus mesInfo;
    int taille = nbrVoisinsD+nbrVoisinsG;
    int tousVoisins = taille;

    struct infoProcessus tousMesVoisins[taille];
    for(int j=0;j<nbrVoisinsG;j++){
      tousMesVoisins[j]=mesVoisins[j];
    }
    for(int j=nbrVoisinsG;j<taille;j++){
      tousMesVoisins[j] = mesVoisinsD[l];
      l++;
    }
    for(int k=0;k<taille;k++){
      printf("voisin : %d adresse %s port %d\n",tousMesVoisins[k].numProcessus,tousMesVoisins[k].adresseIp,tousMesVoisins[k].numPort);
    }

    if(monNumero==1){
      couleur = 1;
    }
    int tabCouleur[taille];
    for(int i=0;i<taille;i++){
      tabCouleur[i]=0;
    }*/

/*while(var<2){
    int z=0;
    int cpt=1;
      for(int i=0; i<taille;i++){
        int dSocket = socket(PF_INET, SOCK_DGRAM, 0);
        if (dSocket < 0) {
          perror("ERROR, failed to create socket \n");
          exit(0);
        }

        struct sockaddr_in aD;
        aD.sin_family = AF_INET;
        aD.sin_addr.s_addr = inet_addr(tousMesVoisins[i].adresseIp);
        aD.sin_port = htons((short)tousMesVoisins[i].numPort);
        socklen_t lgA = sizeof(struct sockaddr_in);
        // envoie de ma couleur a tout mes voisins
        if (sendto(dSocket, &couleur, sizeof(couleur), 0, (struct sockaddr *)&aD, lgA)<0) {
          printf("ERROR: COLOR NOT SENT TO : %d\n", tousMesVoisins[i].numProcessus);
        }
        printf("color %d sent to : %d port %d  adresse %s\n",couleur,tousMesVoisins[i].numProcessus,tousMesVoisins[i].numPort,tousMesVoisins[i].adresseIp);
        close(dSocket);
      }
      
       //reçevoir la couleur de chaque voisin
       int dSocket = socket(PF_INET, SOCK_DGRAM, 0);
        if (dSocket < 0) {
          perror("ERROR, failed to create socket \n");
          exit(0);
        }
        struct sockaddr_in add;
        add.sin_family = AF_INET;
        add.sin_addr.s_addr = INADDR_ANY; 
        add.sin_port = htons((short)atoi(argv[3]));
        res = bind(dSocket, (struct sockaddr *)&ad, sizeof(ad));

      //while(1){
        struct sockaddr_in sClient;
        socklen_t lAdC = sizeof(struct sockaddr_in);
        int couleurVoisin;
        if(recvfrom(dSocket, &couleurVoisin, sizeof(couleurVoisin), 0,(struct sockaddr *)&sClient, &lAdC)<0){
          printf("ERROR: COLOR NOT RECEIVED\n");
        }
        printf("color received %d \n",couleurVoisin);
        tabCouleur[z]=couleurVoisin;
        z++;
       
      }
      
      while(existCouleur(tabCouleur,cpt,taille)!=0) cpt++;
      couleur = cpt;
      if(existCouleur(tabCouleur,0,taille)==0) var++;
      close(dSocket);
*/











 

    /*if(nbrVoisinsD!=0){
      fd_set set, settmp ;
      FD_ZERO(&set) ; FD_SET(ds, &set) ;
      int max = ds ;
      while(1){
          settmp = set ;
          select(max+1, &settmp, NULL, NULL, NULL) ;
          for(int df= 2 ; df <= max ; df++){
              if (!FD_ISSET(df, &settmp)) continue ;
              if (df == ds) {
                  int dSC= accept(ds, NULL, NULL) ;
                  FD_SET(dSC, &set) ;
                  if (max < dSC) max = dSC ;
                  continue ;
              }
              int num;
              if (recv(df, &num, sizeof(num), 0) <= 0) {
                  perror("ERROR: message not received\n");
                  close(df); continue;
              }
              printf("Message received from : %d\n", num);
             
          }
          
      }
    }*/











  // on teste s'il est du coté droit seulement
  // ici on envoie des demandes de connexion à ces voisins
  /*************************ETAPE 3************************************/
 /* if (nbrVoisinsD != 0 && nbrVoisinsG == 0) { // le cas ou je suis server (a gauche) seulement
    // protocol tcp
    close(idSocket);

    int ds = socket(PF_INET, SOCK_STREAM, 0);
    if (ds == -1) {
      perror("ERROR : CREATION SOCKET\n");
      exit(1);
    }
    printf("SOCKET CREATED\n");

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
    printf("je passe en mode ecoute\n");
    int i = 0;
    while (i < nbrVoisinsD) {
      struct sockaddr_in sClient;
      socklen_t lAdC = sizeof(struct sockaddr_in);
      int acpt = accept(ds, (struct sockaddr *)&sClient, &lAdC);
      if (acpt < 0) {
        printf("ERROR : accept\n");
      } else {
        printf("accept reussi\n");
      }

      int num;
      if (recv(acpt, &num, sizeof(num), 0) < 0) {
        perror("ERROR: message not received\n");
      }
      printf("Message received from : %d\n", num);
      i++;
    }
    close(ds);
  } else if (nbrVoisinsG != 0 && nbrVoisinsD == 0) { // le cas je suis client (a droite)
                                 // je reçois mes voisins de la part du server
    struct infoProcessus mesVoisins[nbrVoisinsG];
    if (recvfrom(idSocket, &mesVoisins, sizeof(mesVoisins), 0,
                 (struct sockaddr *)&adresse, &flen) < 0) {
      printf("ERREUR 2: RECEIVING MESSAGE FROM SERVER");
      return -1;
    }
    printf("tab de mes voisins recu \n");
    for (int i = 0; i < nbrVoisinsG; i++) {
      printf("voisin %d adresse %s port %d\n", mesVoisins[i].numProcessus,
             mesVoisins[i].adresseIp, mesVoisins[i].numPort);
    }
    // protocol tcp
    close(idSocket);

    int ds = socket(PF_INET, SOCK_STREAM, 0);
    if (ds == -1) {
      perror("ERROR : CREATION SOCKET\n");
      exit(1);
    }
    printf("SOCKET CREATED\n");

    // nommage
    struct sockaddr_in add;
    add.sin_family = AF_INET;
    add.sin_addr.s_addr = INADDR_ANY;
    add.sin_port = htons((short)atoi(argv[3]));
    int res = bind(ds, (struct sockaddr *)&add, sizeof(add));

    if (res == -1) {
      perror("Client : erreur au nommage de la socket \n");
    }

    for (int i = 0; i < nbrVoisinsG; i++) {
      struct sockaddr_in aD;
      aD.sin_family = AF_INET;
      aD.sin_addr.s_addr = inet_addr(mesVoisins[i].adresseIp);
      aD.sin_port = htons((short)mesVoisins[i].numPort);
      socklen_t lgA = sizeof(struct sockaddr_in);
      // demande de connexion
      sleep(30);
      if (connect(ds, (struct sockaddr *)&aD, lgA) < 0) {
        printf("ERROR: CONNEXION REFUSED\n");
      }
      int monNumero = info[0];
      if (send(ds, &monNumero, sizeof(monNumero), 0) < 0) {
        printf("ERROR: MESSAGE NOT SENT TO : %d", mesVoisins[i].numProcessus);
        return -1;
      }
      printf("message sent %d to %d\n", monNumero, mesVoisins[i].numProcessus);
    }
    

    close(ds);
    // ici il est du cote gauche donc il reçoit des messages
  } else if (nbrVoisinsD != 0 && nbrVoisinsG != 0) {
      struct infoProcessus mesVoisins[nbrVoisinsG];
      if (recvfrom(idSocket, &mesVoisins, sizeof(mesVoisins), 0,
                   (struct sockaddr *)&adresse, &flen) < 0) {
        printf("ERREUR 2: RECEIVING MESSAGE FROM SERVER");
        return -1;
      }
      printf("tab de mes voisins recu \n");
      for (int i = 0; i < nbrVoisinsG; i++) {
        printf("voisin %d adresse %s port %d\n", mesVoisins[i].numProcessus,
               mesVoisins[i].adresseIp, mesVoisins[i].numPort);
      }
         close(idSocket);
      //le role du server
      int ds = socket(PF_INET, SOCK_STREAM, 0);
      if (ds == -1) {
        perror("ERROR : CREATION SOCKET\n");
        exit(1);
      }
      printf("SOCKET CREATED\n");
  
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
      printf("je passe en mode ecoute\n");
      int i = 0;
      while (i < nbrVoisinsD) {
        struct sockaddr_in sClient;
        socklen_t lAdC = sizeof(struct sockaddr_in);
        int acpt = accept(ds, (struct sockaddr *)&sClient, &lAdC);
        if (acpt < 0) {
          printf("ERROR : accept\n");
        } else {
          printf("accept reussi\n");
        }
  
        int num;
        if (recv(acpt, &num, sizeof(num), 0) < 0) {
          perror("ERROR: message not received\n");
        }
        printf("Message received from : %d\n", num);
        i++;
      }
  
      //le role du client
       for (int i = 0; i < nbrVoisinsG; i++) {
          struct sockaddr_in aD;
          aD.sin_family = AF_INET;
          aD.sin_addr.s_addr = inet_addr(mesVoisins[i].adresseIp);
          aD.sin_port = htons((short)mesVoisins[i].numPort);
          socklen_t lgA = sizeof(struct sockaddr_in);
          // demande de connexion
          sleep(30);
          if (connect(ds, (struct sockaddr *)&aD, lgA) < 0) {
            printf("ERROR: CONNEXION REFUSED\n");
          }
          int monNumero = info[0];
          if (send(ds, &monNumero, sizeof(monNumero), 0) < 0) {
            printf("ERROR: MESSAGE NOT SENT TO : %d", mesVoisins[i].numProcessus);
            return -1;
          }
          printf("message sent %d to %d\n", monNumero, mesVoisins[i].numProcessus);
      }
      
      
      close(ds);
        


    
   }*/
