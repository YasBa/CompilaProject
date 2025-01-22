#ifndef COMMUN_H
#define COMMUN_H

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

extern DataValue MEM[TAILLEMEM];
extern DataType MEM_TYPE[TAILLEMEM];
extern int SP;
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
    ARRAY_TOKEN,
    SQUARE_BRACKET_OPEN_TOKEN,
    SQUARE_BRACKET_CLOSE_TOKEN,
    ERREUR_TOKEN
} CODES_LEX;

typedef struct
{
    CODES_LEX CODE;
    char nom[64];
} TSym_Cour;

extern TSym_Cour SYM_COUR;
extern TSym_Cour symPre;
extern FILE *fsource;
extern int car_cour;
extern int line_num;

typedef enum
{
    INT,
    ADD,
    SUB,
    MUL,
    DIVI,
    EQL,
    NEQ,
    GTR,
    LSS,
    GEQ,
    LEQ,
    PRN,
    INN,
    LDI,
    LDA,
    LDV,
    STO,
    BRN,
    BZE,
    HLT,
    CALL,
    RET,
    LDL,
    STL,
    LDF,
    STO_IND,
    PUSH_PARAMS_COUNT
} Mnemoniques;

typedef struct
{
    Mnemoniques MNE;
    int SUITE;
} INSTRUCTION;

extern INSTRUCTION PCODE[TAILLECODE];
extern int PC;

void Error(const char *msg);

#endif
