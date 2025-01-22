#ifndef ANALYSEUR_SEMANTIQUE_H
#define ANALYSEUR_SEMANTIQUE_H

#include "commun.h"

typedef enum {
    TVAR,
    TCONST,
    TTYPE,
    TPROC,
    TFUNC
} TSYM;

typedef struct {
    char     Nom[32];
    TSYM TIDF;
    DataType type;
    int      Adresse;
    int      Value;
    float    FValue;
} T_TAB_IDF;

extern T_TAB_IDF TAB_IDFS[TAILLEIDFS];
extern int   NBR_IDFS;
extern int   OFFSET;

int IDexists(const char* nom);
int isVar(const char* nom);
int isConst(const char* nom);
int getAdresse(const char* nom);
int getConstValue(const char* nom);
float getConstFValue(const char* nom);
int isProcedure(const char* nom);
int isFunction(const char* nom);
int getProcFuncIndex(const char* nom);

void TypeDecl();
void ConstDecl();
void VarDecl();
DataType parseBaseType();

#endif
