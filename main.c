#include "global.h"
#include "analyse_lexical.h"
#include "syntaxique.h"
#include "semantique.h"
#include "generation_pcode.h"
#include "interpreteur.h"

int main(int argc, char* argv[])
{
    if(argc < 2){
        printf("Usage: %s <source_file> [pcode_file]\n", argv[0]);
        return 1;
    }

    fsource = fopen(argv[1], "r");
    if(!fsource){
        perror("fopen source");
        return 1;
    }

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

    printf("Compilation reussie. \n");

    if(argc > 2){
        SavePCodeToFile(argv[2]);
        fclose(fsource);
    } else {
        fclose(fsource);
    }

    INTER_PCODE();

    return 0;
}
