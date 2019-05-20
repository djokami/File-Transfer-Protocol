#include "../Dependance/csapp.h"
#include "../Dependance/couleur.h"
#include "../Dependance/lireCommande.h"
#include <time.h>

#define NomFichierLog "logClient.txt"

//Retourne la taille du fichier
int tailleFichier(int fd) {
    int t = lseek (fd, 0, SEEK_END); 
    lseek(fd,0,SEEK_SET);
    return t;
}

//la réponse du serveur quand on lui a demandé un fichier
//Argument: la connection socket, le nom du fichier à récupérer, si reprise de téléchargement du fichier
void recuperationFichierServeur(int clientfd,char * nomFichier,int recup) {
    char buf[MAXLINE];
    int nombreOctet = 0; // 0 octet lu
    int nombreOctetALire = 0;
    int fichierSortie; int longueurNomFichier; int tailleF;
    int fichierLog;
    int n;
    clock_t debutTemps, finTemps; double temps;
    rio_t rio;

    //Le log du client
    fichierLog = Open(NomFichierLog,O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);

    debutTemps = clock();
    longueurNomFichier = strlen(nomFichier);
    Rio_writen(fichierLog, nomFichier, longueurNomFichier); //Ecrit le nom du fichier dans le log
    Rio_writen(fichierLog, "\n", 1); //Ecrit le nom du fichier dans le log

    Rio_readinitb(&rio, clientfd);
    Rio_readnb(&rio, buf, 1); //Récupère l'entête

    if(buf[0]=='0') { //Erreur du transfert
        printf(Rouge"Fichier %s: impossible d'y accéder\n"Blanc,nomFichier);
    }
    else { //La connexion s'est bien passé
        Rio_readnb(&rio,&tailleF,sizeof(tailleF)); //Récupère la taille du fichier à télécharger

        if(recup) { //Si on récupère un fichier mal télechargé
            fichierSortie = open(nomFichier,O_WRONLY | O_CREAT,S_IRWXU); //Ouverture du fichier
            lseek(fichierSortie,0,SEEK_END); //on va à la fin du fichier pour ajouter le contenu manquant
        }
        else {
            fichierSortie = open(nomFichier,O_WRONLY | O_CREAT |O_TRUNC,S_IRWXU); //Création du fichier de sortie
        }

        do {
            //Calcule le nombre d'octets lus
            if(tailleF-nombreOctet>MAXLINE) {
                nombreOctetALire = MAXLINE;
            }
            else {
                nombreOctetALire = tailleF-nombreOctet;
            }

            n = Rio_readnb(&rio, buf, nombreOctetALire); //Récupère le contenu de la socket
            Rio_writen(fichierSortie, buf, n); //les écrit dans le fichier de sortie
            nombreOctet+=n;
        } while (n != 0); //Tant qu'il reste des données à lire

        Rio_writen(fichierLog, "OK\n", 3); //Le fichier a été correctement transféré

        close(fichierSortie);
        finTemps = clock();
        temps = ((double)finTemps-debutTemps)/CLOCKS_PER_SEC; //Temps du transfert
        printf("Fin du téléchargement du fichier %s!\n",nomFichier);
        printf("%d octets lus en %f s! (%.2f Koctets/s)\n",nombreOctet,temps, ((double)nombreOctet/1000)/temps);
    }
}
//Récupération du fichier qui a été mal téléchargé
void recuperationFichier(char * nomFichier,int clientfd) { 
    int fd = open(nomFichier,O_RDONLY,0);
    if(fd<=0) {
        int flog = open(NomFichierLog,O_CREAT |O_TRUNC,S_IRWXU); //Vide le fichier de log
        close(flog);
    }
    else {
        int taille = tailleFichier(fd); //La taille du fichier
        char chaineCommande[50];
        sprintf(chaineCommande,"reget %s %d\n",nomFichier,taille);
        Rio_writen(clientfd,chaineCommande,strlen(chaineCommande));//Envoie la commande au serveur
        recuperationFichierServeur(clientfd,nomFichier,1); //Récupère le fichier
    }
}

//Demande le fichier au serveur
void demandeFichier(commande cmd,int clientfd) {
    Rio_writen(clientfd,cmd.chaineCommande,strlen(cmd.chaineCommande)); //Envoie la commande au serveur
    recuperationFichierServeur(clientfd,cmd.argument[0],0); //Récupère le fichier
}


//Demande au serveur d'exécuter la commande demander
void demandeCommande(commande cmd,int clientfd) {
    char buf[MAXLINE];
    int nombreOctet = 0; // 0 octet lu
    int nombreOctetALire = 0;
    int taille;
    int n;
    rio_t rio;

    Rio_writen(clientfd,cmd.chaineCommande,strlen(cmd.chaineCommande)); //Envoie la commande au serveur    
    Rio_readinitb(&rio, clientfd);
    Rio_readnb(&rio, buf, 1); //Récupère l'entête

    if(buf[0]=='0') { //Erreur du transfert
        printf(Rouge"Erreur lors de l'exécution de %s\n"Blanc,cmd.chaineCommande);
    }
    else { //La connexion s'est bien passé
        Rio_readnb(&rio,&taille,sizeof(taille)); //Récupère le nombre d'octet à lire
        do {
            //Calcule le nombre d'octets à lire
            if(taille-nombreOctet>MAXLINE) {
                nombreOctetALire = MAXLINE;
            }
            else {
                nombreOctetALire = taille-nombreOctet;
            }
            if ((n = Rio_readnb(&rio, buf, nombreOctetALire))>0){ //Récupère le contenu de la socket
                printf("%s",buf);
            }
            nombreOctet+=n;
        } while (n != 0); //Tant qu'il reste des données à lire
    }
}

//Envoie le fichier au serveur
void envoyerFichier(commande cmd, int clientfd){
    size_t n;
    char buf[MAXLINE];
    rio_t rioL;
    int fd;
    char * nomFichier = cmd.argument[0];

    fd = open(nomFichier,O_RDONLY,0); //Ouvre le fichier demandé

    if(fd<0) { //Erreur ouverture du fichier
        printf(Rouge"Impossible d'ouvir le fichier: %s\n"Blanc,nomFichier);
    }
    else {        
        Rio_readinitb(&rioL, fd);
        int taille = tailleFichier(fd);
        Rio_writen(clientfd,cmd.chaineCommande,strlen(cmd.chaineCommande)); //Envoie la commande au serveur
        Rio_writen(clientfd,&taille,sizeof(taille)); //Envoie la taille du fichier à téléverser dans la socket 
        while ((n = Rio_readnb(&rioL, buf, MAXLINE)) != 0) { //Lecture du fichier
            Rio_writen(clientfd, buf, n); //L'envoie dans la socket
        }
    }
    close(fd); //Ferme le fichier
    return;
}

//Main du client
int main(int argc, char **argv) {
    int clientfd, port;
    commande cmd;
    int fichierLog;
    char *host, buf[MAXLINE]; 
    rio_t rioLog; 

    if (argc != 2) {
        fprintf(stderr, "Utilisation: %s <hôte> \n", argv[0]);
        exit(0);
    }
    host = argv[1];
    port = 2121;


    clientfd = Open_clientfd(host, port); //Connection au serveur
    printf("Client connecté au serveur\n"); 

    // Lecture du log
    char nomFichier[100];
    char statut[4];    
    fichierLog = open(NomFichierLog,O_RDONLY | O_CREAT ,S_IRWXU); //Ouverture du fichier de log
    if(fichierLog > 0) { //Le fichier de log existe
        Rio_readinitb(&rioLog, fichierLog);
        if (Rio_readlineb(&rioLog,nomFichier,100)>0) { //Lecture du nom de fichier
            if (Rio_readlineb(&rioLog,statut,4)<=1) {  //Le statut du précédent téléchargement
                nomFichier[strlen(nomFichier)-1]='\0';
                recuperationFichier(nomFichier,clientfd); //On récupère le précédent fichier qui se n'est pas téléchargé correctement
            }
        }
    }

    //Entrer les commandes
    printf(Bleu"ftp> "Blanc);
    while (Fgets(buf, MAXLINE, stdin) != NULL) { //Lecture du de la commande
        if(strcmp(buf,"bye\n")==0) { //Ferme la connexion
            Close(clientfd);
            exit(0);
        }

        cmd = lireCommande(buf); //Récupère la commande de l'utilisateur
        switch (cmd.type){
            case GET:
                demandeFichier(cmd,clientfd);
                break;
            case LS:case PWD:case CD:case RM: case MKDIR:
                demandeCommande(cmd,clientfd);
                break;
            case PUT:
                envoyerFichier(cmd,clientfd);
                break;
            default:
                printf(Rouge"Commande inconnue!\n"Blanc);
                break;
        }
        printf(Bleu"ftp> "Blanc);
    }

    Close(clientfd); //Fin de la connexion
    exit(0);
}
