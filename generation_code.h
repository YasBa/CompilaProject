#ifndef GENERATION_CODE_H
#define GENERATION_CODE_H

#include "commun.h"

void GENERER1(Mnemoniques M);

void GENERER2(Mnemoniques M, int arg);

void SavePCodeToFile(const char* FICH_SORTIE);
void SaveInstToFile(FILE *FICH_SORTIE, INSTRUCTION INST);

void chargerPCode(const char* FICH_SORTIE);

#endif
