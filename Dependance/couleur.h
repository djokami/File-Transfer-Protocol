#ifndef __COULEUR_H
#define __COULEUR_H

#define Rouge "\033[1;31m"
#define Rouge_gras "\033[0;31m"
#define Vert "\033[1;32m"
#define Vert_gras "\033[0;32m"
#define Jaune "\033[1;33m"
#define Jaune_gras "\033[0;33m"
#define Bleu "\033[1;34m"
#define Bleu_gras "\033[0;34m"
#define Magenta "\033[1;35m"
#define Magenta_gras "\033[0;35m"
#define Cyan "\033[1;36m"
#define Cyan_gras "\033[0;36m"
#define Gris "\033[0;37m"
#define Blanc "\033[1;37m"

#define afficher_rouge(X) printf("%s%s%s",Rouge,X,Blanc)
#define afficher_rouge_gras(X) printf("%s%s%s",Rouge_gras,X,Blanc)
#define afficher_vert(X) printf("%s%s%s",Vert,X,Blanc)
#define afficher_vert_gras(X) printf("%s%s%s",Vert_gras,X,Blanc)
#define afficher_jaune(X) printf("%s%s%s",Jaune,X,Blanc)
#define afficher_jaune_gras(X) printf("%s%s%s",Jaune_gras,X,Blanc)
#define afficher_bleu(X) printf("%s%s%s",Bleu,X,Blanc)
#define afficher_bleu_gras(X) printf("%s%s%s",Bleu_gras,X,Blanc)
#define afficher_magenta(X) printf("%s%s%s",Magenta,X,Blanc)
#define afficher_magenta_gras(X) printf("%s%s%s",Magenta_gras,X,Blanc)
#define afficher_cyan(X) printf("%s%s%s",Cyan,X,Blanc)
#define afficher_cyan_gras(X) printf("%s%s%s",Cyan_gras,X,Blanc)
#define afficher_gris(X) printf("%s%s%s",Gris,X,Blanc)


void rouge();

void rouge_gras();

void vert();

void vert_gras();

void jaune();

void jaune_gras();

void bleu();

void bleu_gras();

void magenta();

void magenta_gras();

void cyan();

void cyan_gras();

void gris();

void blanc();

#endif