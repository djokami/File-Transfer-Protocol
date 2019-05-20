
enum nomCommande {
    GET,
    REGET,
    PUT,
    RM,
    MKDIR,
    LS,
    CD,
    PWD,
    INCONNU,
    ERREUR
};

typedef struct commande{
    enum nomCommande type; //Le type de la commande
    char ** argument; //Au plus deux arguments dans les commandes
    char chaineCommande[100]; //La chaine de la commande
    int nombreArgument;
} commande;

//lit la commande entré par l'utilisateur dans la socket connfd
//retourne une variable de type commande
commande lireCommande(char *);