#include "commun.h"
#include "analyseur_lexical.h"
#include "analyseur_syntaxique.h"
#include "analyseur_semantique.h"
#include "generation_code.h"
#include "interpreteur.h"

int main(int argc, char* argv[])
{
    if(argc < 2){
        printf("Usage: %s <fichier_source> [pcode_file]\n", argv[0]);
        return 1;
    }

    fsource = fopen(argv[1], "r");
    if(!fsource){
        perror("fopen source");
        return 1;
    }

    printf("Debut de Compilation... \n");

    LireCar();
    SymSuiv();
    Program();

    for(int i = 0; i < NBR_IDFS; i++){
        if(TAB_IDFS[i].TIDF == TVAR){
            int adr = TAB_IDFS[i].Adresse;
            if(adr >= 0 && adr < TAILLEMEM){
                MEM_TYPE[adr] = TAB_IDFS[i].type;
            }
        }
    }

    printf("\nCompilation reussie. \n");

    if(argc > 2){
        SavePCodeToFile(argv[2]);
        fclose(fsource);
    } else {
        fclose(fsource);
    }

    printf("\nDebut d'interpretation du Pseudo-code... \n");

    INTER_PCODE();

    return 0;
}
