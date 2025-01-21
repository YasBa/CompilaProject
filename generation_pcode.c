#include "generation_pcode.h"
#include "semantique.h"

// Tableau global de P-code qui va stocker toutes les instructions générées
INSTRUCTION PCODE[TAILLECODE];
// PC (Program Counter) : index de la dernière instruction écrite dans PCODE
// Initialisé à -1 car aucune instruction n'a encore été écrite
int PC = -1;

// ---------------------------------------------------------------------
// Ecrire1 : Écrit une instruction sans argument dans le tableau PCODE
// ---------------------------------------------------------------------
// Paramètre M : le mnémonique (instruction) à ajouter
void GENERER1(Mnemoniques M) {
    // Vérifie qu'on n'a pas dépassé la taille maximale du tableau
    if (PC >= TAILLECODE - 1)
        Error("Too many instructions");
    PC++;                   // Incrémente le compteur de programme
    PCODE[PC].MNE = M;      // Stocke le mnémonique dans l'instruction courante
    PCODE[PC].SUITE = 0;    // Aucun argument n'est nécessaire, on met 0
}

// ---------------------------------------------------------------------
// Ecrire2 : Écrit une instruction avec un argument entier dans PCODE
// ---------------------------------------------------------------------
// Paramètre M   : le mnémonique (instruction) à ajouter
// Paramètre arg : l'argument entier associé à l'instruction
void GENERER2(Mnemoniques M, int arg) {
    // Vérifie qu'on n'a pas dépassé la capacité du tableau
    if (PC >= TAILLECODE - 1)
        Error("Too many instructions");
    PC++;                   // Passe à la prochaine position
    PCODE[PC].MNE = M;      // Assigne le mnémonique
    PCODE[PC].SUITE = arg;  // Stocke l'argument associé à l'instruction
}

// ---------------------------------------------------------------------
// afficherPCode : Affiche toutes les instructions du P-code
// ---------------------------------------------------------------------
// Parcourt le tableau PCODE et imprime pour chaque instruction son index,
// le mnémonique et l'argument
// void afficherPCode() {
//     for (int i = 0; i <= PC; i++) {
//         printf("%3d : (%d, %d)\n", i, PCODE[i].MNE, PCODE[i].SUITE);
//     }
// }

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
        // Erreur(INST_PCODE_ERR, "SaveInstToFile");
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
        // Écrit le mnémonique et l'argument de chaque instruction sur une ligne
        SaveInstToFile(f, PCODE[i]);

    }
    fclose(f);
}

// ---------------------------------------------------------------------
// chargerPCode : Charge le P-code à partir d'un fichier
// ---------------------------------------------------------------------
// Paramètre FICH_SORTIE : le nom du fichier d'où charger le P-code
// Lit chaque instruction du fichier et les stocke dans PCODE, puis ferme le fichier.
// void chargerPCode(const char* FICH_SORTIE) {
//     FILE* f = fopen(FICH_SORTIE, "r");
//     if (!f) {
//         perror("fopen");
//         exit(EXIT_FAILURE);
//     }
//     PC = -1;  // Réinitialise le compteur de programme
//     while (!feof(f)) {
//         int m, s;
//         // Lit un mnémonique et son argument depuis le fichier
//         if (fscanf(f, "%d %d", &m, &s) == 2) {
//             PC++;
//             if (PC >= TAILLECODE)
//                 Error("P-code too large");
//             PCODE[PC].MNE = (Mnemoniques)m; // Convertit en type Mnemoniques
//             PCODE[PC].SUITE = s;
//         }
//     }
//     fclose(f);
// }
