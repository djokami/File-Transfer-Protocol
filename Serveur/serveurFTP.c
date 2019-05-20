#include "../Dependance/csapp.h"
#include "../Dependance/couleur.h"
#include "../Dependance/lireCommande.h"

#define MAX_NAME_LEN 256
#define TAILLE_POOL 10


//Retourne la taille du fichier
int tailleFichier(int fd) {
    int t = lseek(fd, 0, SEEK_END);
    lseek(fd,0,SEEK_SET); //Se remet au début du fichier
    return t; 
}


// //Lire le fichier passé en paramètre et le met dans la socket
//Paramètre: la socket de connexion, le nom du fichier, l'endroit à partir du quel on doit lire le fichier
// 0 pour lire à partir du début 
void recuperationFichier(int connfd, char * nomFichier, int decalage) {
    rio_t rioL;
    char buf[MAXLINE];
    int n;
    int fd = open(nomFichier,O_RDONLY,0);
    if(fd<0) { //Erreur ouverture du fichier
        printf(Rouge"Impossible d'ouvir le fichier: %s\n"Blanc,nomFichier);
        Rio_writen(connfd, "0", 1); //Erreur
    }
    else {
        Rio_readinitb(&rioL, fd);
        Rio_writen(connfd, "1", 1); //Pas erreur
        int taille= tailleFichier(fd);
        taille = (taille-decalage);
        Rio_writen(connfd,&taille ,sizeof(int)); //Le nombre d'octets qui restent à lire
        lseek(fd,decalage,SEEK_SET); //Va à l'endroit où on s'est arrêté
        while ((n = Rio_readnb(&rioL, buf, MAXLINE)) > 0) { //Lecture du fichier
            Rio_writen(connfd, buf, n); //L'envoie dans la socket
        }
    }
    close(fd);
}


//récupère le fichier envoyé par l'utilisateur
void telechargerFichierClient(int connfd, char * nomFichier) {
    int fichier; int n;
    int tailleF; int nombreOctetALire=0; int nombreOctet=0;
    rio_t rio;
    char buf[MAXLINE];

    fichier = open(nomFichier,O_WRONLY|O_CREAT | O_TRUNC, S_IRWXU);//Fichier de destination
    rio_readinitb(&rio,connfd);
    rio_readnb(&rio,&tailleF,sizeof(tailleF)); //Récupère le nombre d'octets à lire
    printf("%d\n",tailleF);

    do {
        //Calcule le nombre d'octets à lire
        if(tailleF-nombreOctet>MAXLINE) {
            nombreOctetALire = MAXLINE;
        }
        else {
            nombreOctetALire = tailleF-nombreOctet;
        }

        n = Rio_readnb(&rio, buf, nombreOctetALire); //Récupère le contenu de la socket
        if(n) {
            Rio_writen(fichier, buf, n); //les écrit dans le fichier de sortie
        }
        nombreOctet+=n;
    } while (n != 0); //Tant qu'il reste des données à lire
}

//Execute une commande cmd avec c'est argument
void executerCmd(int connfd,char *cmd,char **argument) {
    int canal[2];
    pipe(canal);

    if(Fork()==0) { //Fils
        char * arg[4] ;
        int i=1;
        close(canal[0]); //Ferme la sortie
        dup2(canal[1],1); //Entrée standard dans l'entrée du canal
        close(canal[1]);
        arg[0]=cmd; //Le premier argument est le nom de la commande
        while(argument[i-1]!=NULL) {
            arg[i] = argument[i-1];
            i++;
        }
        arg[i]=NULL;
    	execvp(cmd,arg);
        printf(Rouge"La commande %s a échoué\n"Blanc,cmd);
        Rio_writen(connfd, "0", 1); //Erreur
        exit(0);
    }
    //Père
    close(canal[1]);
    rio_t rio;
    Rio_readinitb(&rio,canal[0]);

    int tailleSortie = 0;
    int n=0;
    char buf[MAXLINE];
    while((n=Rio_readnb(&rio,buf,MAXLINE))>0 ) { //Lit le résultat de la commande
        tailleSortie +=n; //Nombre d'octets qu'on envoie dans la socket
    }
    Rio_writen(connfd, "1", 1); //La commande s'est bien passé
    if(tailleSortie==0){ //Rien à écrire dans la socket
        Rio_writen(connfd,&tailleSortie,sizeof(tailleSortie));
    } 
    else { //On écrit le résultat dans la socket
        buf[tailleSortie]=0;
        tailleSortie++; //On compte le caractère de fin de chaine
        Rio_writen(connfd,&tailleSortie,sizeof(tailleSortie));
        Rio_writen(connfd,buf,tailleSortie);
    }
    
    close(canal[0]);
	return;
}

//Faire la commande cd
void executerCd(int connfd,char * chemin) {
    int tailleSortie = 0;
    if (chdir(chemin)==0) { //Réussi
        Rio_writen(connfd, "1", 1); //La commande s'est bien passé
        Rio_writen(connfd,&tailleSortie,sizeof(tailleSortie));
    }
    else {
        printf(Rouge"La commande cd %s a échoué\n"Blanc,chemin);
        Rio_writen(connfd, "0", 1); //Erreur
    }
}


// Lit la commande envoyée dans la socket et appelle les fonctions pour exécuter la commande
void executerCommande(int connfd) {
    commande cmd;
    char buf[MAXLINE];
    int n=1;
    rio_t rioL;
    rio_readinitb(&rioL,connfd);
    while(n) {
        if ((n = Rio_readlineb(&rioL, buf, MAXLINE)) != 0) { //Lecture de la demande du client
            printf("Commande demandé: %s\n",buf);
            cmd = lireCommande(buf); //Interprete la commande
            switch (cmd.type) {
                case GET:
                    recuperationFichier(connfd,cmd.argument[0],0);
                    break;
                case REGET:
                    recuperationFichier(connfd,cmd.argument[0],atoi(cmd.argument[1]));
                    break;
                case LS:
                    executerCmd(connfd,"ls",cmd.argument);
                    break;
                case CD:
                    executerCd(connfd,cmd.argument[0]);
                    break;
                case PWD:
                    executerCmd(connfd,"pwd",cmd.argument);
                    break;
                case MKDIR:
                    executerCmd(connfd,"mkdir",cmd.argument);
                    break;
                case RM:
                    executerCmd(connfd,"rm",cmd.argument);
                    break;
                case PUT: //On récupère le fichier du client
                    telechargerFichierClient(connfd,cmd.argument[0]);
                    break;
                default:
                    break;
            }
        }
    }
}





//Affiche les informations de la nouvelle connection
void afficherInfoConnexionCreee(int connfd,struct sockaddr_in clientaddr,socklen_t clientlen) {
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    /* determine the name of the client */
    Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAX_NAME_LEN, 0, 0, 0);
    /* determine the textual representation of the client's IP address */
    Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,INET_ADDRSTRLEN);
    printf("Serveur connecté à %s (%s) ", client_hostname,client_ip_string);
    getsockname(connfd, (SA *) &clientaddr, &clientlen);
    printf("numéro de port local : %d ", ntohs(clientaddr.sin_port));
    getpeername(connfd,(SA *) &clientaddr, &clientlen);
    printf("numéro de port distant : %d\n", ntohs(clientaddr.sin_port));
}

//Affiche les informations de la connection qui se ferme
void afficherInfoConnexionFermee(int connfd,struct sockaddr_in clientaddr,socklen_t clientlen) {
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    /* determine the name of the client */
    Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAX_NAME_LEN, 0, 0, 0);
    /* determine the textual representation of the client's IP address */
    Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,INET_ADDRSTRLEN);
    printf("Fermeture de la connection: %s (%s) ", client_hostname,client_ip_string);
    getsockname(connfd, (SA *) &clientaddr, &clientlen);
    printf("numéro de port local : %d ", ntohs(clientaddr.sin_port));
    getpeername(connfd,(SA *) &clientaddr, &clientlen);
    printf("numéro de port distant : %d\n", ntohs(clientaddr.sin_port));
}

int main(){
    int listenfd, connfd, port=2121;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
        
    clientlen = (socklen_t)sizeof(clientaddr);
    listenfd = Open_listenfd(port);
    printf("Serveur créé sur le port: %d\n",port);

    for(int i=0;i<TAILLE_POOL-1;i++) { //on crée les fils
        if(Fork()==0) { //les fils
            while (1) {
                connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

                afficherInfoConnexionCreee(connfd,clientaddr,clientlen);
                executerCommande(connfd);
                afficherInfoConnexionFermee(connfd,clientaddr,clientlen);
                Close(connfd);
            }
            exit(0);
        }
    }
    while (1) { //Le père
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
 
        afficherInfoConnexionCreee(connfd,clientaddr,clientlen);
        executerCommande(connfd);
        afficherInfoConnexionFermee(connfd,clientaddr,clientlen);

        Close(connfd);
    }
    exit(0);
}

