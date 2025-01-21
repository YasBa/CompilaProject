#include "generation_pcode.h"
#include "semantique.h"

INSTRUCTION PCODE[TAILLECODE];
int PC = -1;

void GENERER1(Mnemoniques M) {
    if (PC >= TAILLECODE - 1)
        Error("Too many instructions");
    PC++;
    PCODE[PC].MNE = M;
    PCODE[PC].SUITE = 0;
}

void GENERER2(Mnemoniques M, int arg) {
    if (PC >= TAILLECODE - 1)
        Error("Too many instructions");
    PC++;
    PCODE[PC].MNE = M;
    PCODE[PC].SUITE = arg;
}

void SaveInstToFile(FILE *FICH_SORTIE, INSTRUCTION INST)
{
    switch (INST.MNE)
    {
    case LDA:
        fprintf(FICH_SORTIE, "%s \t %d \n", "LDA", INST.SUITE);
        break;
    case LDI:
        fprintf(FICH_SORTIE, "%s \t %d \n", "LDI", INST.SUITE);
        break;
    case INT:
        fprintf(FICH_SORTIE, "%s \t %d \n", "INT", INST.SUITE);
        break;
    case BZE:
        fprintf(FICH_SORTIE, "%s \t %d \n", "BZE", INST.SUITE);
        break;
    case BRN:
        fprintf(FICH_SORTIE, "%s \t %d \n", "BRN", INST.SUITE);
        break;
    case LDV:
        fprintf(FICH_SORTIE, "%s \n", "LDV");
        break;
    case ADD:
        fprintf(FICH_SORTIE, "%s \n", "ADD");
        break;
    case SUB:
        fprintf(FICH_SORTIE, "%s \n", "SUB");
        break;
    case MUL:
        fprintf(FICH_SORTIE, "%s \n", "MUL");
        break;
    case DIVI:
        fprintf(FICH_SORTIE, "%s \n", "DIVI");
        break;
    case LEQ:
        fprintf(FICH_SORTIE, "%s \n", "LEQ");
        break;
    case GEQ:
        fprintf(FICH_SORTIE, "%s \n", "GEQ");
        break;
    case NEQ:
        fprintf(FICH_SORTIE, "%s \n", "NEQ");
        break;
    case LSS:
        fprintf(FICH_SORTIE, "%s \n", "LSS");
        break;
    case GTR:
        fprintf(FICH_SORTIE, "%s \n", "GTR");
        break;
    case EQL:
        fprintf(FICH_SORTIE, "%s \n", "EQL");
        break;
    case HLT:
        fprintf(FICH_SORTIE, "%s \n", "HLT");
        break;
    case STO:
        fprintf(FICH_SORTIE, "%s \n", "STO");
        break;
    case INN:
        fprintf(FICH_SORTIE, "%s \n", "INN");
        break;
    case PRN:
        fprintf(FICH_SORTIE, "%s \n", "PRN");
        break;

    default:
        fprintf(FICH_SORTIE, "%s \n", "ERROR");
        break;
    }
}

void SavePCodeToFile(const char* FICH_SORTIE) {
    FILE* f = fopen(FICH_SORTIE, "w");
    if (!f) {
        perror("fopen");
        return;
    }
    for (int i = 0; i <= PC; i++) {
        SaveInstToFile(f, PCODE[i]);
    }
    fclose(f);
}
