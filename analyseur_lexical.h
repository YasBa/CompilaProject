#ifndef ANALYSE_LEXICAL_H
#define ANALYSE_LEXICAL_H

#include "commun.h"

void LireCar();

void SymSuiv();

void Test_Symbole(CODES_LEX t);

typedef enum
{
    ID_ERR,
    PROGRAM_ERR,
    CONST_ERR,
    VAR_ERR,
    BEGIN_ERR,
    END_ERR,
    IF_ERR,
    THEN_ERR,
    WHILE_ERR,
    DO_ERR,
    READ_ERR,
    WRITE_ERR,
    PV_ERR,
    PT_ERR,
    PLUS_ERR,
    MOINS_ERR,
    MULT_ERR,
    DIV_ERR,
    VIR_ERR,
    AFF_ERR,
    INF_ERR,
    INFEG_ERR,
    SUP_ERR,
    SUPEG_ERR,
    DIFF_ERR,
    PO_ERR,
    PF_ERR,
    FIN_ERR,
    ERREUR_ERR,
    EOF_ERR,
    EG_ERR,
    CONST_VAR_BEGIN_ERR,
    VAR_BEGIN_ERR,
    REPEAT_ERR,
    UNTIL_ERR,
    FOR_ERR,
    ELSE_ERR,
    CASE_ERR,
    OF_ERR,
    INTO_ERR,
    DOWNTO_ERR,
    DDOT_ERR,
    INST_PCODE_ERR,
    INTEGER_ERR,
    FLOAT_ERR,
    BOOLEAN_ERR,
    STRING_ERR,
    ARRAY_ERR,
    INTEGER_DATA_ERR,
    FLOAT_DATA_ERR,
    BOOLEAN_DATA_ERR,
    STRING_DATA_ERR,
    ARRAY_DATA_ERR,
    QMARK_ERR,
    SQUARE_BRACKET_OPEN_ERR,
    SQUARE_BRACKET_CLOSE_ERR,
    ECRIRE_LN_ERR,
} CODES_ERR;

const char *getErrorMessage(CODES_ERR error_code);

#endif
