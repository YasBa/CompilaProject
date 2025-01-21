#include "syntaxique.h"
#include "analyse_lexical.h"
#include "semantique.h"
#include "generation_pcode.h"

typedef struct
{
    char name[32];
    DataType type;
    int index;
} LocalParam;

static LocalParam localParams[50];
static int localCount = 0;

static void initLocalParams()
{
    localCount = 0;
}

static int addLocalParam(const char *nom, DataType t)
{
    strcpy(localParams[localCount].name, nom);
    localParams[localCount].type = t;
    localParams[localCount].index = localCount;
    localCount++;
    return localCount - 1;
}

static int findLocalParamIndex(const char *nom)
{
    for (int i = 0; i < localCount; i++)
    {
        if (!strcmp(localParams[i].name, nom))
            return localParams[i].index;
    }
    return -1;
}

static int isResultKeyword(const char *nm)
{
    return (insideAFunction && strcmp(nm, "result") == 0);
}

static void parseParamList(int indexProcFunc);
static void parseArguments(int indexProcFunc);

void Program()
{
    Test_Symbole(PROGRAM_TOKEN);
    Test_Symbole(ID_TOKEN);
    Test_Symbole(PV_TOKEN);

    while (SYM_COUR.CODE == CONST_TOKEN ||
           SYM_COUR.CODE == TYPE_TOKEN ||
           SYM_COUR.CODE == VAR_TOKEN)
    {
        switch (SYM_COUR.CODE)
        {
        case CONST_TOKEN:
            Test_Symbole(CONST_TOKEN);
            ConstDecl();
            break;
        case TYPE_TOKEN:
            Test_Symbole(TYPE_TOKEN);
            TypeDecl();
            break;
        case VAR_TOKEN:
            Test_Symbole(VAR_TOKEN);
            VarDecl();
            break;
        default:
            break;
        }
    }

    GENERER2(BRN, 0);
    int jumpOverProcsIndex = PC;

    ProcFuncPart();

    int afterProcs = PC + 1;
    PCODE[jumpOverProcsIndex].SUITE = afterProcs;

    Bloc();
    Test_Symbole(PT_TOKEN);
    GENERER1(HLT);
}

void Bloc()
{
    while (SYM_COUR.CODE == CONST_TOKEN ||
           SYM_COUR.CODE == TYPE_TOKEN ||
           SYM_COUR.CODE == VAR_TOKEN)
    {
        switch (SYM_COUR.CODE)
        {
        case CONST_TOKEN:
            Test_Symbole(CONST_TOKEN);
            ConstDecl();
            break;
        case TYPE_TOKEN:
            Test_Symbole(TYPE_TOKEN);
            TypeDecl();
            break;
        case VAR_TOKEN:
            Test_Symbole(VAR_TOKEN);
            VarDecl();
            break;
        default:
            break;
        }
    }

    Test_Symbole(BEGIN_TOKEN);
    Insts();
    Test_Symbole(END_TOKEN);
}

void Insts()
{
    Inst();
    while (SYM_COUR.CODE == PV_TOKEN)
    {
        Test_Symbole(PV_TOKEN);
        if (SYM_COUR.CODE == END_TOKEN || SYM_COUR.CODE == UNTIL_TOKEN || SYM_COUR.CODE == ELSE_TOKEN)
        {
            break;
        }
        Inst();
    }
}

void Inst()
{
    switch (SYM_COUR.CODE)
    {
    case ID_TOKEN:
        CallOrAssign();
        break;

    case IF_TOKEN:
    {
        Test_Symbole(IF_TOKEN);
        Cond();
        Test_Symbole(THEN_TOKEN);
        int jumpIf = PC + 1;
        GENERER2(BZE, 0);
        Inst();
        if (SYM_COUR.CODE == ELSE_TOKEN)
        {
            int jumpElse = PC + 1;
            GENERER2(BRN, 0);
            PCODE[jumpIf].SUITE = PC + 1;
            Test_Symbole(ELSE_TOKEN);
            Inst();
            PCODE[jumpElse].SUITE = PC + 1;
        }
        else
        {
            PCODE[jumpIf].SUITE = PC + 1;
        }
    }
    break;

    case WHILE_TOKEN:
    {
        Test_Symbole(WHILE_TOKEN);
        int condAddr = PC + 1;
        Cond();
        Test_Symbole(DO_TOKEN);
        int jumpOut = PC + 1;
        GENERER2(BZE, 0);
        Inst();
        GENERER2(BRN, condAddr);
        PCODE[jumpOut].SUITE = PC + 1;
    }
    break;

    case REPEAT_TOKEN:
        RepeatInst();
        break;

    case FOR_TOKEN:
        ForInst();
        break;

    case CASE_TOKEN:
        CaseInst();
        break;

    case BEGIN_TOKEN:
        Test_Symbole(BEGIN_TOKEN);
        Insts();
        Test_Symbole(END_TOKEN);
        break;

    case WRITE_TOKEN:
    {
        Test_Symbole(WRITE_TOKEN);
        Test_Symbole(PRG_TOKEN);
        do
        {
            Exp();
            GENERER1(PRN);
            if (SYM_COUR.CODE == VIR_TOKEN)
                Test_Symbole(VIR_TOKEN);
            else
                break;
        } while (1);
        Test_Symbole(PRD_TOKEN);
    }
    break;

    case READ_TOKEN:
    {
        Test_Symbole(READ_TOKEN);
        Test_Symbole(PRG_TOKEN);
        do
        {
            char nm[32];
            strcpy(nm, SYM_COUR.nom);
            Test_Symbole(ID_TOKEN);
            int ad = getAdresse(nm);
            GENERER2(LDI, ad);
            GENERER1(INN);
            if (SYM_COUR.CODE == VIR_TOKEN)
                Test_Symbole(VIR_TOKEN);
            else
                break;
        } while (1);
        Test_Symbole(PRD_TOKEN);
    }
    break;

    default:
        Error("Instruction inconnue");
    }
}

void Cond()
{
    Exp();
    CODES_LEX cl = SYM_COUR.CODE;
    if (cl == EGAL_TOKEN || cl == DIFF_TOKEN ||
        cl == INF_TOKEN || cl == INFEG_TOKEN ||
        cl == SUP_TOKEN || cl == SUPEG_TOKEN)
    {
        Test_Symbole(cl);
        Exp();
        switch (cl)
        {
        case EGAL_TOKEN:
            GENERER1(EQL);
            break;
        case DIFF_TOKEN:
            GENERER1(NEQ);
            break;
        case INF_TOKEN:
            GENERER1(LSS);
            break;
        case INFEG_TOKEN:
            GENERER1(LEQ);
            break;
        case SUP_TOKEN:
            GENERER1(GTR);
            break;
        case SUPEG_TOKEN:
            GENERER1(GEQ);
            break;
        default:
            break;
        }
    }
    else
    {
        Error("Opérateur relationnel attendu!");
    }
}

void Exp()
{
    Term();
    while (SYM_COUR.CODE == PLUS_TOKEN || SYM_COUR.CODE == MOINS_TOKEN)
    {
        CODES_LEX cl = SYM_COUR.CODE;
        Test_Symbole(cl);
        Term();
        if (cl == PLUS_TOKEN)
            GENERER1(ADD);
        else
            GENERER1(SUB);
    }
}

void Term()
{
    Fact();
    while (SYM_COUR.CODE == MULTI_TOKEN || SYM_COUR.CODE == DIV_TOKEN)
    {
        CODES_LEX cl = SYM_COUR.CODE;
        Test_Symbole(cl);
        Fact();
        if (cl == MULTI_TOKEN)
            GENERER1(MUL);
        else
            GENERER1(DIVI);
    }
}

void Fact()
{
    switch (SYM_COUR.CODE)
    {
    case ID_TOKEN:
    {
        char nm[32];
        strcpy(nm, SYM_COUR.nom);
        Test_Symbole(ID_TOKEN);

        if (isFunction(nm))
        {
            int idxF = getProcFuncIndex(nm);
            if (SYM_COUR.CODE == PRG_TOKEN)
            {
                Test_Symbole(PRG_TOKEN);
                parseArguments(idxF);
                Test_Symbole(PRD_TOKEN);
            }
            else
            {
                GENERER2(PUSH_PARAMS_COUNT, 0);
            }
            GENERER2(CALL, TAB_IDFS[idxF].Adresse);
        }
        else if (isProcedure(nm))
        {
<<<<<<< HEAD
            Error("Cannot call a procedure in an expression context");
=======
            // En Pascal, on ne peut pas appeler une procédure dans une expression
            Error("Impossible d'appeler une procédure dans un contexte d'expression");
>>>>>>> d2ee875ffcd679402f58d31a485fff20d72e7d01
        }
        else
        {
            int localIdx = findLocalParamIndex(nm);
            if (insideAFunction && localIdx == 0 && !strcmp(nm, currentFunctionName))
            {
                GENERER2(LDL, 0);
            }
            else if (localIdx >= 0)
            {
                GENERER2(LDL, localIdx);
                GENERER1(LDV);
            }
            else if (isVar(nm))
            {
                int adr = getAdresse(nm);
                GENERER2(LDA, adr);
                GENERER1(LDV);
            }
            else if (isConst(nm))
            {
                DataType ctype = TYPE_UNDEF;
                for (int i = 0; i < NBR_IDFS; i++)
                {
                    if (!strcmp(TAB_IDFS[i].Nom, nm) && TAB_IDFS[i].TIDF == TCONST)
                    {
                        ctype = TAB_IDFS[i].type;
                        break;
                    }
                }
                if (ctype == TYPE_INT)
                {
                    int c = getConstValue(nm);
                    GENERER2(LDI, c);
                }
                else if (ctype == TYPE_REAL)
                {
                    float c = getConstFValue(nm);
                    int int_repr;
                    memcpy(&int_repr, &c, sizeof(float));
                    GENERER2(LDF, int_repr);
                }
                else
                {
                    Error("Type de constante insupporté");
                }
            }
            else
            {
                Error("Identifiant inconnu");
            }
        }
    }
    break;

    case ENT_TOKEN:
    {
        int v = atoi(SYM_COUR.nom);
        Test_Symbole(ENT_TOKEN);
        GENERER2(LDI, v);
    }
    break;

    case FLOAT_TOKEN:
    {
        float vf = atof(SYM_COUR.nom);
        Test_Symbole(FLOAT_TOKEN);
        int int_repr;
        memcpy(&int_repr, &vf, sizeof(float));
        GENERER2(LDF, int_repr);
    }
    break;

    case PRG_TOKEN:
        Test_Symbole(PRG_TOKEN);
        Exp();
        Test_Symbole(PRD_TOKEN);
        break;

    default:
        Error("Facteur invalide");
        break;
    }
}

void RepeatInst()
{
    Test_Symbole(REPEAT_TOKEN);
    int start = PC + 1;
    Insts();
    Test_Symbole(UNTIL_TOKEN);
    Cond();
    GENERER2(BZE, start);
}

void ForInst()
{
    Test_Symbole(FOR_TOKEN);
    if (SYM_COUR.CODE != ID_TOKEN)
        Error("Identifiant attendu après FOR");
    char varFor[32];
    strcpy(varFor, SYM_COUR.nom);
    Test_Symbole(ID_TOKEN);
    Test_Symbole(AFFECT_TOKEN);
    Exp();
    int addrVar = getAdresse(varFor);
    GENERER2(STO, addrVar);

    int sens = 0;
    if (SYM_COUR.CODE == TO_TOKEN)
    {
        sens = 0;
        Test_Symbole(TO_TOKEN);
    }
    else if (SYM_COUR.CODE == DOWNTO_TOKEN)
    {
        sens = 1;
        Test_Symbole(DOWNTO_TOKEN);
    }
    else
    {
        Error("TO ou DOWNTO attendu");
    }

    Exp();
    int slotFin = OFFSET++;
    GENERER2(STO, slotFin);
    Test_Symbole(DO_TOKEN);

    int condAddr = PC + 1;
    GENERER2(LDA, addrVar);
    GENERER1(LDV);
    GENERER2(LDA, slotFin);
    GENERER1(LDV);
    if (sens == 0)
        GENERER1(LEQ);
    else
        GENERER1(GEQ);

    int jumpEnd = PC + 1;
    GENERER2(BZE, 0);

    Inst();

    GENERER2(LDA, addrVar);
    GENERER1(LDV);
    GENERER2(LDI, 1);
    if (sens == 0)
        GENERER1(ADD);
    else
        GENERER1(SUB);
    GENERER2(STO, addrVar);
    GENERER2(BRN, condAddr);
    PCODE[jumpEnd].SUITE = PC + 1;
}

void CaseInst()
{
    Test_Symbole(CASE_TOKEN);
    Exp();
    int tmpSlot = OFFSET++;
    GENERER2(STO, tmpSlot);
    Test_Symbole(OF_TOKEN);

    int jumpEndLabels[50];
    int nEnd = 0;

    while (SYM_COUR.CODE == ENT_TOKEN)
    {
        int labelVal = atoi(SYM_COUR.nom);
        Test_Symbole(ENT_TOKEN);
        Test_Symbole(COLON_TOKEN);

        GENERER2(LDA, tmpSlot);
        GENERER1(LDV);
        GENERER2(LDI, labelVal);
        GENERER1(EQL);

        int jumpIfNot = PC + 1;
        GENERER2(BZE, 0);
        Inst();
        int jumpAfter = PC + 1;
        GENERER2(BRN, 0);
        jumpEndLabels[nEnd++] = jumpAfter;
        PCODE[jumpIfNot].SUITE = PC + 1;

        if (SYM_COUR.CODE == PV_TOKEN)
            Test_Symbole(PV_TOKEN);
        else
            break;
    }
    if (SYM_COUR.CODE == ELSE_TOKEN)
    {
        Test_Symbole(ELSE_TOKEN);
        Inst();
        int jumpAfterElse = PC + 1;
        GENERER2(BRN, 0);
        jumpEndLabels[nEnd++] = jumpAfterElse;
        if (SYM_COUR.CODE == PV_TOKEN)
            Test_Symbole(PV_TOKEN);
    }
    Test_Symbole(END_TOKEN);
    int endOfCase = PC + 1;
    for (int i = 0; nEnd; i++)
    {
        PCODE[jumpEndLabels[i]].SUITE = endOfCase;
    }
}

void ProcFuncPart()
{
    while (SYM_COUR.CODE == PROCEDURE_TOKEN || SYM_COUR.CODE == FUNCTION_TOKEN)
    {
        if (SYM_COUR.CODE == PROCEDURE_TOKEN)
            ProcDecl();
        else
            FuncDecl();
    }
}

void ProcDecl()
{
    initLocalParams();

    Test_Symbole(PROCEDURE_TOKEN);
    char procName[32];
    strcpy(procName, SYM_COUR.nom);
    Test_Symbole(ID_TOKEN);

    if (IDexists(procName))
        Error("Procedure name already used");
    int idx = NBR_IDFS;

    strcpy(TAB_IDFS[idx].Nom, procName);
    TAB_IDFS[idx].TIDF = TPROC;
    TAB_IDFS[idx].type = TYPE_UNDEF;
    TAB_IDFS[idx].Adresse = -1;
    TAB_IDFS[idx].Value = 0;
    NBR_IDFS++;

    parseParamList(idx);
    Test_Symbole(PV_TOKEN);

    int startPC = PC + 1;
    TAB_IDFS[idx].Adresse = startPC;

    for (int p = 0; p < TAB_IDFS[idx].Value; p++)
    {
        GENERER2(LDL, p);
        GENERER2(STL, p);
    }

    Bloc();
    Test_Symbole(PV_TOKEN);

    GENERER2(RET, TAB_IDFS[idx].Value);
}

void FuncDecl()
{
    initLocalParams();

    Test_Symbole(FUNCTION_TOKEN);
    char fnName[32];
    strcpy(fnName, SYM_COUR.nom);
    Test_Symbole(ID_TOKEN);

    int idx = NBR_IDFS;
    strcpy(TAB_IDFS[idx].Nom, fnName);
    TAB_IDFS[idx].TIDF = TFUNC;
    TAB_IDFS[idx].Adresse = -1;
    TAB_IDFS[idx].Value = 0;
    TAB_IDFS[idx].type = TYPE_INT;
    NBR_IDFS++;

    parseParamList(idx);
    Test_Symbole(COLON_TOKEN);
    DataType retType = parseBaseType();
    TAB_IDFS[idx].type = retType;
    Test_Symbole(PV_TOKEN);

    insideAFunction = 1;
    strcpy(currentFunctionName, fnName);

    int startPC = PC + 1;
    TAB_IDFS[idx].Adresse = startPC;

    for (int p = 0; p < TAB_IDFS[idx].Value; p++)
    {
        GENERER2(LDL, p);
        GENERER2(STL, p);
    }

    Bloc();
    Test_Symbole(PV_TOKEN);

    GENERER2(LDL, 0);
    GENERER2(RET, TAB_IDFS[idx].Value);

    insideAFunction = 0;
    currentFunctionName[0] = '\0';
}

void CallOrAssign()
{
    char name[32];
    strcpy(name, SYM_COUR.nom);
    Test_Symbole(ID_TOKEN);

    if (insideAFunction && strcmp(name, currentFunctionName) == 0)
    {
        Test_Symbole(AFFECT_TOKEN);
        Exp();
        GENERER2(STL, 0);
        return;
    }

    int isProc = isProcedure(name);
    int isFunc = isFunction(name);

    if (isProc || isFunc)
    {
        int idxPF = getProcFuncIndex(name);

        if (SYM_COUR.CODE == PRG_TOKEN)
        {
            Test_Symbole(PRG_TOKEN);
            parseArguments(idxPF);
            Test_Symbole(PRD_TOKEN);
        }
        else
        {
            GENERER2(PUSH_PARAMS_COUNT, 0);
        }
        GENERER2(CALL, TAB_IDFS[idxPF].Adresse);

        if (isFunc)
            GENERER2(STO, -9999);
    }
    else
    {
        Test_Symbole(AFFECT_TOKEN);
        Exp();
        int localIdx = findLocalParamIndex(name);
        if (localIdx >= 0)
        {
            GENERER2(LDL, localIdx);
            GENERER1(STO_IND);
        }
        else
        {
            int adr = getAdresse(name);
            GENERER2(STO, adr);
        }
    }
}

static void parseParamList(int indexProcFunc)
{
    int total = 0;
    if (SYM_COUR.CODE == PRG_TOKEN)
    {
        Test_Symbole(PRG_TOKEN);
        while (SYM_COUR.CODE == ID_TOKEN)
        {
            char groupIDS[10][32];
            int gCount = 0;
            while (SYM_COUR.CODE == ID_TOKEN)
            {
                strcpy(groupIDS[gCount], SYM_COUR.nom);
                gCount++;
                Test_Symbole(ID_TOKEN);
                if (SYM_COUR.CODE == VIR_TOKEN)
                    Test_Symbole(VIR_TOKEN);
                else
                    break;
            }
            Test_Symbole(COLON_TOKEN);
            DataType ptype = parseBaseType();

            for (int i = 0; i < gCount; i++)
            {
                addLocalParam(groupIDS[i], ptype);
                total++;
            }

            if (SYM_COUR.CODE == PV_TOKEN)
                Test_Symbole(PV_TOKEN);
            else
                break;
        }
        Test_Symbole(PRD_TOKEN);
    }
    TAB_IDFS[indexProcFunc].Value = total;
}

static void parseArguments(int indexProcFunc)
{
    int nbParams = TAB_IDFS[indexProcFunc].Value;
    int count = 0;

    if (SYM_COUR.CODE != PRD_TOKEN)
    {
        while (1)
        {
            if (SYM_COUR.CODE == ID_TOKEN)
            {
                int addr = getAdresse(SYM_COUR.nom);
                GENERER2(LDA, addr);
                Test_Symbole(ID_TOKEN);
            }
            else
            {
                Exp();
            }
            count++;

            if (SYM_COUR.CODE == VIR_TOKEN)
                Test_Symbole(VIR_TOKEN);
            else
                break;
        }
    }

    if (count != nbParams)
    {
        char buf[128];
        sprintf(buf, "Nombre incorrect de paramètres. Attendu %d, obtenu %d",
                nbParams, count);
        Error(buf);
    }

    GENERER2(PUSH_PARAMS_COUNT, count);
}
