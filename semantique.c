#include "semantique.h"
#include "analyse_lexical.h"

T_TAB_IDF TAB_IDFS[TAILLEIDFS];

int NBR_IDFS = 0;

int OFFSET = VAR_BASE;

int IDexists(const char *nom)
{
    for (int i = 0; i < NBR_IDFS; i++)
    {
        if (!strcmp(TAB_IDFS[i].Nom, nom))
            return 1;
    }
    return 0;
}

int isVar(const char *nom)
{
    for (int i = 0; i < NBR_IDFS; i++)
    {
        if (!strcmp(TAB_IDFS[i].Nom, nom) && TAB_IDFS[i].TIDF == TVAR)
            return 1;
    }
    return 0;
}

int isConst(const char *nom)
{
    for (int i = 0; i < NBR_IDFS; i++)
    {
        if (!strcmp(TAB_IDFS[i].Nom, nom) && TAB_IDFS[i].TIDF == TCONST)
            return 1;
    }
    return 0;
}

int getAdresse(const char *nom)
{
    for (int i = 0; i < NBR_IDFS; i++)
    {
        if (!strcmp(TAB_IDFS[i].Nom, nom) && TAB_IDFS[i].TIDF == TVAR)
        {
            return TAB_IDFS[i].Adresse;
        }
    }
    Error("Variable not found");
    return -1;
}

int getConstValue(const char *nom)
{
    for (int i = 0; i < NBR_IDFS; i++)
    {
        if (!strcmp(TAB_IDFS[i].Nom, nom) && TAB_IDFS[i].TIDF == TCONST &&
            TAB_IDFS[i].type == TYPE_INT)
            return TAB_IDFS[i].Value;
    }
    return 0;
}

float getConstFValue(const char *nom)
{
    for (int i = 0; i < NBR_IDFS; i++)
    {
        if (!strcmp(TAB_IDFS[i].Nom, nom) && TAB_IDFS[i].TIDF == TCONST &&
            TAB_IDFS[i].type == TYPE_REAL)
            return TAB_IDFS[i].FValue;
    }
    return 0.0f;
}

int isProcedure(const char *nom)
{
    for (int i = 0; i < NBR_IDFS; i++)
    {
        if (!strcmp(TAB_IDFS[i].Nom, nom) && TAB_IDFS[i].TIDF == TPROC)
            return 1;
    }
    return 0;
}

int isFunction(const char *nom)
{
    for (int i = 0; i < NBR_IDFS; i++)
    {
        if (!strcmp(TAB_IDFS[i].Nom, nom) && TAB_IDFS[i].TIDF == TFUNC)
            return 1; // C'est une fonction
    }
    return 0;
}

int getProcFuncIndex(const char *nom)
{
    for (int i = 0; i < NBR_IDFS; i++)
    {
        if (!strcmp(TAB_IDFS[i].Nom, nom) &&
            (TAB_IDFS[i].TIDF == TPROC || TAB_IDFS[i].TIDF == TFUNC))
            return i;
    }
    Error("Procedure/Function not found");
    return -1;
}

void TypeDecl()
{

    while (SYM_COUR.CODE == ID_TOKEN)
    {
        char listIDS[10][32];
        int n = 0;

        do
        {
            if (n >= 10)
                Error("Too many IDs in type alias line");
            strcpy(listIDS[n], SYM_COUR.nom);
            n++;
            Test_Symbole(ID_TOKEN);
            if (SYM_COUR.CODE == VIR_TOKEN)
                Test_Symbole(VIR_TOKEN);
            else
                break;
        } while (SYM_COUR.CODE == ID_TOKEN);

        Test_Symbole(EGAL_TOKEN);

        DataType d = TYPE_UNDEF;

        if (!strcmp(SYM_COUR.nom, "integer"))
        {
            d = TYPE_INT;
            Test_Symbole(ID_TOKEN);
        }
        else if (!strcmp(SYM_COUR.nom, "real"))
        {
            d = TYPE_REAL;
            Test_Symbole(ID_TOKEN);
        }
        else if (!strcmp(SYM_COUR.nom, "boolean"))
        {
            d = TYPE_BOOL;
            Test_Symbole(ID_TOKEN);
        }
        else if (!strcmp(SYM_COUR.nom, "string"))
        {
            d = TYPE_STRING;
            Test_Symbole(ID_TOKEN);
        }
        else
        {
            Error("Unknown base type in type alias");
        }

        for (int i = 0; i < n; i++)
        {
            if (IDexists(listIDS[i]))
                Error("Alias name already used");
            strcpy(TAB_IDFS[NBR_IDFS].Nom, listIDS[i]);
            TAB_IDFS[NBR_IDFS].TIDF = TTYPE;
            TAB_IDFS[NBR_IDFS].type = d;
            TAB_IDFS[NBR_IDFS].Adresse = -1;
            NBR_IDFS++;
        }

        if (SYM_COUR.CODE == PV_TOKEN)
            Test_Symbole(PV_TOKEN);
        else
            break;
    }
}

void ConstDecl()
{

    while (SYM_COUR.CODE == ID_TOKEN)
    {
        char nom[32];
        strcpy(nom, SYM_COUR.nom);
        Test_Symbole(ID_TOKEN);
        Test_Symbole(EGAL_TOKEN);

        if (IDexists(nom))
            Error("Const name declared twice");
        strcpy(TAB_IDFS[NBR_IDFS].Nom, nom);
        TAB_IDFS[NBR_IDFS].TIDF = TCONST;
        TAB_IDFS[NBR_IDFS].Adresse = -1;

        if (SYM_COUR.CODE == ENT_TOKEN)
        {
            TAB_IDFS[NBR_IDFS].Value = atoi(SYM_COUR.nom);
            TAB_IDFS[NBR_IDFS].type = TYPE_INT;
            Test_Symbole(ENT_TOKEN);
        }
        else if (SYM_COUR.CODE == FLOAT_TOKEN)
        {
            TAB_IDFS[NBR_IDFS].FValue = atof(SYM_COUR.nom);
            TAB_IDFS[NBR_IDFS].type = TYPE_REAL;
            Test_Symbole(FLOAT_TOKEN);
        }
        else
        {
            Error("Const must be numeric");
        }
        NBR_IDFS++;
        Test_Symbole(PV_TOKEN);
    }
}

void VarDecl()
{

    while (SYM_COUR.CODE == ID_TOKEN)
    {
        char listIDS[10][32];
        int n = 0;

        do
        {
            if (n >= 10)
                Error("Too many IDs in var line");
            strcpy(listIDS[n], SYM_COUR.nom);
            n++;
            Test_Symbole(ID_TOKEN);
            if (SYM_COUR.CODE == VIR_TOKEN)
                Test_Symbole(VIR_TOKEN);
            else
                break;
        } while (SYM_COUR.CODE == ID_TOKEN);

        DataType declaredType = TYPE_INT;
        if (SYM_COUR.CODE == COLON_TOKEN)
        {
            Test_Symbole(COLON_TOKEN);
            declaredType = parseBaseType();
        }

        Test_Symbole(PV_TOKEN);

        for (int i = 0; i < n; i++)
        {
            if (IDexists(listIDS[i]))
                Error("Var name used");
            strcpy(TAB_IDFS[NBR_IDFS].Nom, listIDS[i]);
            TAB_IDFS[NBR_IDFS].TIDF = TVAR;
            TAB_IDFS[NBR_IDFS].type = declaredType;

            TAB_IDFS[NBR_IDFS].Adresse = OFFSET++;
            printf("Declared variable: %s, Type: %d, Address: %d\n",
                   TAB_IDFS[NBR_IDFS].Nom, TAB_IDFS[NBR_IDFS].type, TAB_IDFS[NBR_IDFS].Adresse);
            NBR_IDFS++;
        }
    }
}

DataType parseBaseType()
{
    if (SYM_COUR.CODE == INT_TOKEN)
    {
        Test_Symbole(INT_TOKEN);
        return TYPE_INT;
    }
    else if (SYM_COUR.CODE == FLOAT_TOKEN)
    {
        Test_Symbole(FLOAT_TOKEN);
        return TYPE_REAL;
    }
    else if (SYM_COUR.CODE == BOOL_TOKEN)
    {
        Test_Symbole(BOOL_TOKEN);
        return TYPE_BOOL;
    }
    else if (SYM_COUR.CODE == STRING_TOKEN)
    {
        Test_Symbole(STRING_TOKEN);
        return TYPE_STRING;
    }
    else
    {
        Error("Unknown type in var declaration");
        return TYPE_UNDEF;
    }
}
