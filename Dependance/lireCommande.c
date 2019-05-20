#include "csapp.h"
#include "lireCommande.h"


//Lit la commande présente dans la chaine de caractères
commande lireCommande(char * chaineCommande) {
    commande cmd;
    int n=strlen(chaineCommande);
    char argument[3][40];
    strcpy(&(cmd.chaineCommande[0]), chaineCommande); //On recopie la commande dans la structure
    cmd.chaineCommande[n]=0; //La commande en chaine de caractère
    cmd.nombreArgument=0; //Le nombre d'argument de la commande

    char commandeType[30];
    for(int a=0;a<10;a++)
        commandeType[a] = 0;
    int i=0; int j=0;
    while(i<n && chaineCommande[i]==' ') { //On lit les espaces
        i++;
    }
    while(i<n && chaineCommande[i]!=' ' && chaineCommande[i]!='\n') { //On lit la commande
        commandeType[j] = chaineCommande[i];
        i++; j++;
    }

    //On met le type dans la structure
    if(!strcmp(commandeType,"get")) {
        cmd.type = GET;
    }
    else if(!strcmp(commandeType,"reget")) {
        cmd.type = REGET;
    }
    else if(!strcmp(commandeType,"put")) {
        cmd.type = PUT;
    }
    else if(!strcmp(commandeType,"rm")) {
        cmd.type = RM;
    }
    else if(!strcmp(commandeType,"mkdir")) {
        cmd.type = MKDIR;
    }
    else if(!strcmp(commandeType,"ls")) {
        cmd.type = LS;
    }
    else if(!strcmp(commandeType,"cd")) {
        cmd.type = CD;
    }
    else if(!strcmp(commandeType,"pwd")) {
        cmd.type = PWD;
    }
    else {
        cmd.type = INCONNU;
    }

    while(i<n && chaineCommande[i]==' ') { //On lit les espaces
        i++;
    }
    j=0;
    while(i<n && (chaineCommande[i]!=' ' && chaineCommande[i]!='\n')) { //On lit l'argument
        argument[0][j] = chaineCommande[i];
        i++;j++;
    }
    if(j)
        cmd.nombreArgument+=1;
    argument[0][j]=0; //Fin de la chaine

    while(i<n && chaineCommande[i]==' ') { //On lit les espaces
        i++;
    }
    j=0;
    while(i<n && (chaineCommande[i]!=' ' && chaineCommande[i]!='\n')) { //On lit le 2ème argument
        argument[1][j] = chaineCommande[i];
        i++;j++;
    }
    if(j)
        cmd.nombreArgument+=1;
    argument[1][j]=0;

    //On copie les arguments
    cmd.argument = malloc(sizeof(char *)*(cmd.nombreArgument+1));
    for(int r=0;r<=cmd.nombreArgument;r++) {
        cmd.argument[r] = malloc(sizeof(char)*(strlen(argument[r])+1));
        strcpy(cmd.argument[r],argument[r]);
    }
    cmd.argument[cmd.nombreArgument] = NULL;


    //Teste si l'utilisateur à mis les arguments des commandes
    if((cmd.type == GET || cmd.type == CD || cmd.type == MKDIR || cmd.type == RM || cmd.type == PUT) && cmd.nombreArgument==0) {
        cmd.type = ERREUR;
    }
    else if ((cmd.type == LS || cmd.type == PWD) && cmd.nombreArgument>0) {
        cmd.type = ERREUR;
    }


    return cmd;
}