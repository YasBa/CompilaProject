#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAILLECODE 500
#define TAILLEMEM 500
#define TAILLEIDFS 200
#define VAR_BASE 200

typedef enum
{
    TYPE_INT,
    TYPE_REAL,
    TYPE_BOOL,
    TYPE_STRING,
    TYPE_UNDEF
} DataType;

typedef union
{
    int i;
    float f;
} DataValue;

// -------------------------------
// Variables globales et tableaux
// -------------------------------

// Tableau global pour stocker les valeurs (mémoire du programme)
extern DataValue MEM[TAILLEMEM];
// Tableau qui définit le type de chaque élément dans MEM
extern DataType MEM_TYPE[TAILLEMEM];
// Pointeur de pile (indique le sommet de la pile)
extern int SP;
// Pointeur de base pour les appels de fonctions/procédures
extern int BP;

typedef enum
{
    PROGRAM_TOKEN,
    VAR_TOKEN,
    CONST_TOKEN,
    TYPE_TOKEN,
    IF_TOKEN,
    THEN_TOKEN,
    ELSE_TOKEN,
    WHILE_TOKEN,
    DO_TOKEN,
    REPEAT_TOKEN,
    UNTIL_TOKEN,
    FOR_TOKEN,
    TO_TOKEN,
    DOWNTO_TOKEN,
    CASE_TOKEN,
    OF_TOKEN,
    BEGIN_TOKEN,
    END_TOKEN,
    WRITE_TOKEN,
    READ_TOKEN,
    PROCEDURE_TOKEN,
    FUNCTION_TOKEN,
    AFFECT_TOKEN,
    PV_TOKEN,
    PT_TOKEN,
    PLUS_TOKEN,
    MOINS_TOKEN,
    MULTI_TOKEN,
    DIV_TOKEN,
    EGAL_TOKEN,
    DIFF_TOKEN,
    SUP_TOKEN,
    INF_TOKEN,
    SUPEG_TOKEN,
    INFEG_TOKEN,
    COLON_TOKEN,
    PRG_TOKEN,
    PRD_TOKEN,
    VIR_TOKEN,
    ENT_TOKEN,
    FLOAT_TOKEN,
    ID_TOKEN,
    INT_TOKEN,
    BOOL_TOKEN,
    STRING_TOKEN,
    DIEZE_TOKEN,
    ERREUR_TOKEN
} CODES_LEX;

// Structure représentant un token courant du compilateur
typedef struct
{
    CODES_LEX CODE; // Classe ou type du token
    char nom[64];   // Nom ou valeur du token sous forme de chaîne
} TSym_Cour;        // Utilisé pour stocker le token en cours d'analyse

extern TSym_Cour SYM_COUR; // Le token actuellement analysé
extern TSym_Cour symPre;   // Le token précédent (pour suivi)
extern FILE *fsource;      // Pointeur vers le fichier source à compiler
extern int car_cour;       // Caractère courant dans le fichier source
extern int line_num;       // Numéro de la ligne actuellement lue

// -------------------------------
// Définition des instructions du P-code
// -------------------------------

// Enumération listant toutes les instructions possibles dans le P-code
typedef enum
{
    INT,
    ADD,              // Addition
    SUB,              // Soustraction
    MUL,              // Multiplication
    DIVI,             // Division entière
    EQL,              // Égalité (==)
    NEQ,              // Différence (!=)
    GTR,              // Plus grand que (>)
    LSS,              // Plus petit que (<)
    GEQ,              // Supérieur ou égal (>=)
    LEQ,              // Inférieur ou égal (<=)
    PRN,              // Instruction d'impression (print)
    INN,              // Instruction de saisie (input)
    LDI,              // Charger une valeur immédiate (literal)
    LDA,              // Charger une adresse
    LDV,              // Charger la valeur depuis une adresse
    STO,              // Stocker une valeur dans une adresse
    BRN,              // Branchement inconditionnel (jump)
    BZE,              // Branchement conditionnel si zéro (if zero, jump)
    HLT,              // Arrêter le programme (halt)
    CALL,             // Appel d'une procédure ou fonction (call)
    RET,              // Retour d'une procédure ou fonction (return)
    LDL,              // Charger une variable locale
    STL,              // Stocker dans une variable locale
    LDF,              // Charger une fonction
    STO_IND,          // Stocker via une adresse indirecte
    PUSH_PARAMS_COUNT // Pousser le nombre de paramètres sur la pile
} Mnemoniques;        // Définit toutes les opérations possibles en P-code

// Structure qui représente une instruction du P-code
typedef struct
{
    Mnemoniques MNE; // Le mnémonique (opération) de l'instruction
    int SUITE;       // L'argument ou suite d'instruction associé
} INSTRUCTION;       // Utilisé pour créer chaque instruction du P-code

extern INSTRUCTION PCODE[TAILLECODE]; // Tableau global contenant les instructions du P-code
extern int PC;                        // Compteur ou pointeur courant dans le tableau PCODE

// Déclaration d'une fonction pour afficher une erreur et peut-être arrêter le programme
void Error(const char *msg); // Affiche le message d'erreur passé en paramètre

#endif
