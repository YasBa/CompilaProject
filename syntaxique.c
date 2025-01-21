#include "syntaxique.h"
#include "analyse_lexical.h"
#include "semantique.h"
#include "generation_pcode.h"

// Structure pour gérer les paramètres locaux d'une procédure/fonction
typedef struct
{
    char name[32];     // Nom du paramètre
    DataType type;     // Type du paramètre (int, real, etc.)
    int index;         // Index local où ce paramètre est placé
} LocalParam;

// Tableau pour stocker les paramètres locaux (taille max 50)
static LocalParam localParams[50];
// Compteur indiquant le nombre de paramètres locaux enregistrés
static int localCount = 0;

// Initialise le compteur de paramètres locaux à 0
static void initLocalParams()
{
    localCount = 0; // Remise à zéro des paramètres locaux
}

// Ajoute un paramètre local dans le tableau et retourne son index
static int addLocalParam(const char *nom, DataType t)
{
    strcpy(localParams[localCount].name, nom); // Copie le nom du paramètre
    localParams[localCount].type = t;            // Enregistre le type du paramètre
    localParams[localCount].index = localCount;    // L'index correspond au compteur actuel
    localCount++;                                // Incrémente le compteur
    return localCount - 1;                       // Retourne l'index du paramètre ajouté
}

// Cherche et retourne l'index d'un paramètre local à partir de son nom
static int findLocalParamIndex(const char *nom)
{
    for (int i = 0; i < localCount; i++)
    {
        if (!strcmp(localParams[i].name, nom))   // Compare le nom recherché avec ceux existants
            return localParams[i].index;         // Retourne l'index si trouvé
    }
    return -1; // Retourne -1 si aucun paramètre local n'est trouvé
}

// Vérifie si l'identifiant correspond au mot-clé "result" dans une fonction
static int isResultKeyword(const char *nm)
{
    // Si on est dans une fonction et que le nom est "result" (en minuscules)
    return (insideAFunction && strcmp(nm, "result") == 0);
}

// ==============================
// Déclarations anticipées
// ==============================

// Analyse la liste de paramètres d'une procédure ou fonction
static void parseParamList(int indexProcFunc);
// Analyse la liste des arguments dans un appel de proc/fonction
static void parseArguments(int indexProcFunc);

// ---------------------------------------------------------------------
// Fonction principale du programme
// ---------------------------------------------------------------------
void Program()
{
    Test_Symbole(PROGRAM_TOKEN); // Doit commencer par le token "program"
    Test_Symbole(ID_TOKEN);      // Ensuite, un identifiant (le nom du programme)
    Test_Symbole(PV_TOKEN);      // Puis un point-virgule

    // Boucle pour traiter plusieurs sections const/type/var
    while (SYM_COUR.CODE == CONST_TOKEN ||
           SYM_COUR.CODE == TYPE_TOKEN ||
           SYM_COUR.CODE == VAR_TOKEN)
    {
        switch (SYM_COUR.CODE)
        {
        case CONST_TOKEN:
            Test_Symbole(CONST_TOKEN);
            ConstDecl(); // Déclaration des constantes
            break;
        case TYPE_TOKEN:
            Test_Symbole(TYPE_TOKEN);
            TypeDecl();  // Déclaration des types
            break;
        case VAR_TOKEN:
            Test_Symbole(VAR_TOKEN);
            VarDecl();   // Déclaration des variables
            break;
        default:
            break;
        }
    }

    // Génère un branchement (BRN) provisoire vers le code principal
    GENERER2(BRN, 0);
    int jumpOverProcsIndex = PC; // Sauvegarde de l'adresse du saut

    // Génère le code pour les procédures et fonctions
    ProcFuncPart();

    // Fixe la cible du saut pour pointer après le code des proc/fonctions
    int afterProcs = PC + 1;
    PCODE[jumpOverProcsIndex].SUITE = afterProcs;

    // Analyse le bloc principal du programme
    Bloc();
    Test_Symbole(PT_TOKEN); // Doit terminer par un point
    GENERER1(HLT);     // Génère l'instruction d'arrêt (halt)
}

// ---------------------------------------------------------------------
// Analyse un bloc : const/type/var locaux + instructions
// ---------------------------------------------------------------------
void Bloc()
{
    // Gestion des déclarations locales (const, type, var)
    while (SYM_COUR.CODE == CONST_TOKEN ||
           SYM_COUR.CODE == TYPE_TOKEN ||
           SYM_COUR.CODE == VAR_TOKEN)
    {
        switch (SYM_COUR.CODE)
        {
        case CONST_TOKEN:
            Test_Symbole(CONST_TOKEN);
            ConstDecl(); // Déclaration locale de constantes
            break;
        case TYPE_TOKEN:
            Test_Symbole(TYPE_TOKEN);
            TypeDecl();  // Déclaration locale de types
            break;
        case VAR_TOKEN:
            Test_Symbole(VAR_TOKEN);
            VarDecl();   // Déclaration locale de variables
            break;
        default:
            break;
        }
    }

    Test_Symbole(BEGIN_TOKEN); // Début du bloc d'instructions (mot-clé 'begin')
    Insts();            // Analyse la séquence d'instructions
    Test_Symbole(END_TOKEN); // Fin du bloc (mot-clé 'end')
}

// ---------------------------------------------------------------------
// Analyse une séquence d'instructions séparées par des ';'
// ---------------------------------------------------------------------
void Insts()
{
    Inst(); // Analyse la première instruction
    while (SYM_COUR.CODE == PV_TOKEN) // Tant qu'il y a un point-virgule
    {
        Test_Symbole(PV_TOKEN); // Consomme le point-virgule
        if (SYM_COUR.CODE == END_TOKEN || SYM_COUR.CODE == UNTIL_TOKEN || SYM_COUR.CODE == ELSE_TOKEN)
        {
            break; // Arrête si fin du bloc ou condition d'arrêt d'une boucle/structure
        }
        Inst(); // Analyse l'instruction suivante
    }
}

// ---------------------------------------------------------------------
// Analyse une instruction selon son type
// ---------------------------------------------------------------------
void Inst()
{
    switch (SYM_COUR.CODE)
    {
    // Si c'est un identifiant, cela peut être un appel de proc/fonction ou une affectation
    case ID_TOKEN:
        CallOrAssign();
        break;

    case IF_TOKEN:
    {
        Test_Symbole(IF_TOKEN); // Consomme le token "if"
        Cond();            // Analyse la condition
        Test_Symbole(THEN_TOKEN); // Attend le token "then"
        int jumpIf = PC + 1; // Adresse pour le saut conditionnel
        GENERER2(BZE, 0);     // Génère un branchement si la condition est fausse
        Inst();            // Analyse l'instruction du bloc "then"
        if (SYM_COUR.CODE == ELSE_TOKEN)
        {
            int jumpElse = PC + 1; // Prépare un saut pour le bloc "else"
            GENERER2(BRN, 0);       // Génère un branchement non conditionnel
            PCODE[jumpIf].SUITE = PC + 1; // Fixe la cible du saut du "then"
            Test_Symbole(ELSE_TOKEN);   // Consomme le token "else"
            Inst();              // Analyse le bloc "else"
            PCODE[jumpElse].SUITE = PC + 1; // Fixe la cible du saut après "else"
        }
        else
        {
            PCODE[jumpIf].SUITE = PC + 1; // Fixe le saut de fin du "if"
        }
    }
    break;

    case WHILE_TOKEN:
    {
        Test_Symbole(WHILE_TOKEN); // Consomme le token "while"
        int condAddr = PC + 1; // Adresse du début de la condition
        Cond();              // Analyse la condition de la boucle
        Test_Symbole(DO_TOKEN);   // Attend le token "do"
        int jumpOut = PC + 1; // Adresse pour sortir de la boucle
        GENERER2(BZE, 0);     // Génère le branchement de sortie si la condition est fausse
        Inst();              // Analyse l'instruction à répéter
        GENERER2(BRN, condAddr); // Retourne à l'évaluation de la condition
        PCODE[jumpOut].SUITE = PC + 1; // Fixe la cible du saut de sortie
    }
    break;

    case REPEAT_TOKEN:
        RepeatInst(); // Analyse l'instruction "repeat...until"
        break;

    case FOR_TOKEN:
        ForInst(); // Analyse la boucle "for"
        break;

    case CASE_TOKEN:
        CaseInst(); // Analyse la structure "case"
        break;

    case BEGIN_TOKEN:
        Test_Symbole(BEGIN_TOKEN); // Consomme le token "begin"
        Insts();            // Analyse la séquence d'instructions du bloc
        Test_Symbole(END_TOKEN);   // Consomme le token "end"
        break;

    case WRITE_TOKEN:
    {
        Test_Symbole(WRITE_TOKEN); // Consomme "write"
        Test_Symbole(PRG_TOKEN);   // Consomme '('
        do
        {
            Exp();        // Analyse une expression à écrire
            GENERER1(PRN); // Génère l'instruction d'impression
            if (SYM_COUR.CODE == VIR_TOKEN)
                Test_Symbole(VIR_TOKEN); // Consomme la virgule s'il y a plusieurs arguments
            else
                break;
        } while (1);
        Test_Symbole(PRD_TOKEN); // Consomme ')'
    }
    break;

    case READ_TOKEN:
    {
        Test_Symbole(READ_TOKEN); // Consomme "read"
        Test_Symbole(PRG_TOKEN);  // Consomme '('
        do
        {
            char nm[32];
            strcpy(nm, SYM_COUR.nom); // Sauvegarde le nom de la variable
            Test_Symbole(ID_TOKEN);       // Consomme l'identifiant
            int ad = getAdresse(nm); // Récupère l'adresse de la variable
            GENERER2(LDI, ad);        // Charge l'adresse en immédiat
            GENERER1(INN);            // Génère l'instruction de lecture (input)
            if (SYM_COUR.CODE == VIR_TOKEN)
                Test_Symbole(VIR_TOKEN);  // Gère la virgule entre plusieurs variables
            else
                break;
        } while (1);
        Test_Symbole(PRD_TOKEN); // Consomme ')'
    }
    break;

    default:
        Error("Instruction inconnue"); // Affiche une erreur pour instruction inconnue
    }
}

// ---------------------------------------------------------------------
// Analyse une condition booléenne (opérations relationnelles)
// ---------------------------------------------------------------------
void Cond()
{
    Exp(); // Analyse une expression
    CODES_LEX cl = SYM_COUR.CODE; // Sauvegarde l'opérateur relationnel
    if (cl == EGAL_TOKEN || cl == DIFF_TOKEN ||
        cl == INF_TOKEN || cl == INFEG_TOKEN ||
        cl == SUP_TOKEN || cl == SUPEG_TOKEN)
    {
        Test_Symbole(cl); // Consomme l'opérateur
        Exp();      // Analyse l'expression après l'opérateur
        switch (cl)
        {
        case EGAL_TOKEN:
            GENERER1(EQL); // Génère l'instruction d'égalité
            break;
        case DIFF_TOKEN:
            GENERER1(NEQ); // Génère l'instruction de différence
            break;
        case INF_TOKEN:
            GENERER1(LSS); // Génère l'instruction de "plus petit que"
            break;
        case INFEG_TOKEN:
            GENERER1(LEQ); // Génère l'instruction de "inférieur ou égal"
            break;
        case SUP_TOKEN:
            GENERER1(GTR); // Génère l'instruction de "plus grand que"
            break;
        case SUPEG_TOKEN:
            GENERER1(GEQ); // Génère l'instruction de "supérieur ou égal"
            break;
        default:
            break;
        }
    }
    else
    {
        Error("Opérateur relationnel attendu!"); // Erreur si opérateur non trouvé
    }
}

// ---------------------------------------------------------------------
// Analyse une expression arithmétique (addition/soustraction)
// ---------------------------------------------------------------------
void Exp()
{
    Term(); // Analyse un terme
    while (SYM_COUR.CODE == PLUS_TOKEN || SYM_COUR.CODE == MOINS_TOKEN)
    {
        CODES_LEX cl = SYM_COUR.CODE;
        Test_Symbole(cl); // Consomme l'opérateur + ou -
        Term();     // Analyse le terme suivant
        if (cl == PLUS_TOKEN)
            GENERER1(ADD); // Génère l'instruction d'addition
        else
            GENERER1(SUB); // Génère l'instruction de soustraction
    }
}

// ---------------------------------------------------------------------
// Analyse un terme dans une expression (multiplication/division)
// ---------------------------------------------------------------------
void Term()
{
    Fact(); // Analyse un facteur
    while (SYM_COUR.CODE == MULTI_TOKEN || SYM_COUR.CODE == DIV_TOKEN)
    {
        CODES_LEX cl = SYM_COUR.CODE;
        Test_Symbole(cl); // Consomme * ou /
        Fact();     // Analyse le facteur suivant
        if (cl == MULTI_TOKEN)
            GENERER1(MUL); // Génère l'instruction de multiplication
        else
            GENERER1(DIVI); // Génère l'instruction de division entière
    }
}

// ---------------------------------------------------------------------
// Analyse un facteur (identifiant, nombre, expression entre parenthèses, etc.)
// ---------------------------------------------------------------------
void Fact()
{
    switch (SYM_COUR.CODE)
    {
    case ID_TOKEN:
    {
        char nm[32];
        strcpy(nm, SYM_COUR.nom); // Sauvegarde le nom de l'identifiant
        Test_Symbole(ID_TOKEN);       // Consomme l'identifiant

        if (isFunction(nm))
        {
            // Si c'est une fonction, il peut y avoir des arguments entre parenthèses
            int idxF = getProcFuncIndex(nm);
            if (SYM_COUR.CODE == PRG_TOKEN) // Si '(' est présent
            {
                Test_Symbole(PRG_TOKEN);
                parseArguments(idxF); // Analyse les arguments
                Test_Symbole(PRD_TOKEN);
            }
            else
            {
                // Appel de fonction sans argument
                GENERER2(PUSH_PARAMS_COUNT, 0);
            }
            // Génère l'instruction CALL pour appeler la fonction
            GENERER2(CALL, TAB_IDFS[idxF].Adresse);
            // Le résultat de la fonction reste sur la pile pour être utilisé dans l'expression
        }
        else if (isProcedure(nm))
        {
            // En Pascal, on ne peut pas appeler une procédure dans une expression
            Error("Cannot call a procedure in an expression context");
        }
        else
        {
            // Sinon, c'est une variable, constante, ou paramètre local
            int localIdx = findLocalParamIndex(nm);
            if (insideAFunction && localIdx == 0 && !strcmp(nm, currentFunctionName))
            {
                // Si c'est la variable résultat de la fonction (local #0)
                GENERER2(LDL, 0);
            }
            else if (localIdx >= 0)
            {
                // Si c'est un paramètre local (pass-by-ref)
                GENERER2(LDL, localIdx);
                GENERER1(LDV);
            }
            else if (isVar(nm))
            {
                // Si c'est une variable globale
                int adr = getAdresse(nm);
                GENERER2(LDA, adr);
                GENERER1(LDV);
            }
            else if (isConst(nm))
            {
                // Si c'est une constante, on charge sa valeur
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
                    Error("type de constante insupporté");
                }
            }
            else
            {
                // Si l'identifiant est inconnu
                Error("Identifiant inconnu");
            }
        }
    }
    break;

    case ENT_TOKEN:
    {
        // Nombre entier littéral
        int v = atoi(SYM_COUR.nom);
        Test_Symbole(ENT_TOKEN);
        GENERER2(LDI, v);
    }
    break;

    case FLOAT_TOKEN:
    {
        // Nombre réel littéral
        float vf = atof(SYM_COUR.nom);
        Test_Symbole(FLOAT_TOKEN);
        int int_repr;
        memcpy(&int_repr, &vf, sizeof(float));
        GENERER2(LDF, int_repr);
    }
    break;

    case PRG_TOKEN:
        // Expression entre parenthèses
        Test_Symbole(PRG_TOKEN);
        Exp();
        Test_Symbole(PRD_TOKEN);
        break;

    default:
        Error("Facteur invalide"); // Erreur si facteur invalide
        break;
    }
}

// ---------------------------------------------------------------------
// Analyse une instruction "repeat ... until"
// ---------------------------------------------------------------------
void RepeatInst()
{
    Test_Symbole(REPEAT_TOKEN); // Consomme "repeat"
    int start = PC + 1;    // Position de début du bloc répété
    Insts();             // Analyse les instructions à répéter
    Test_Symbole(UNTIL_TOKEN);  // Attend "until"
    Cond();              // Analyse la condition de sortie
    GENERER2(BZE, start); // Si la condition est fausse, retourne au début du bloc
}

// ---------------------------------------------------------------------
// Analyse une boucle "for"
// ---------------------------------------------------------------------
void ForInst()
{
    Test_Symbole(FOR_TOKEN); // Consomme "for"
    if (SYM_COUR.CODE != ID_TOKEN)
        Error("Identifiant attendu après FOR");
    char varFor[32];
    strcpy(varFor, SYM_COUR.nom); // Sauvegarde le nom de la variable de boucle
    Test_Symbole(ID_TOKEN);
    Test_Symbole(AFFECT_TOKEN); // Consomme ":="
    Exp();               // Analyse l'expression d'initialisation
    int addrVar = getAdresse(varFor); // Récupère l'adresse de la variable de boucle
    GENERER2(STO, addrVar); // Stocke la valeur initiale dans la variable

    int sens = 0;
    if (SYM_COUR.CODE == TO_TOKEN)
    {
        sens = 0;         // Boucle croissante
        Test_Symbole(TO_TOKEN);  // Consomme "to"
    }
    else if (SYM_COUR.CODE == DOWNTO_TOKEN)
    {
        sens = 1;         // Boucle décroissante
        Test_Symbole(DOWNTO_TOKEN); // Consomme "downto"
    }
    else
    {
        Error("TO ou DOWNTO attendu"); // Erreur si ni "to" ni "downto"
    }

    Exp(); // Analyse l'expression de la limite
    int slotFin = OFFSET++;  // Emplacement pour stocker la valeur de fin
    GENERER2(STO, slotFin);   // Stocke la limite dans le slot dédié
    Test_Symbole(DO_TOKEN);       // Consomme "do"

    int condAddr = PC + 1;   // Adresse de retour pour réévaluer la condition
    GENERER2(LDA, addrVar);   // Charge l'adresse de la variable de boucle
    GENERER1(LDV);          // Charge la valeur de la variable
    GENERER2(LDA, slotFin);  // Charge l'adresse du slot de fin
    GENERER1(LDV);          // Charge la limite
    if (sens == 0)
        GENERER1(LEQ);      // Génère l'instruction de "inférieur ou égal" pour boucle croissante
    else
        GENERER1(GEQ);      // Génère l'instruction de "supérieur ou égal" pour boucle décroissante

    int jumpEnd = PC + 1;  // Adresse pour sortir de la boucle
    GENERER2(BZE, 0);       // Branche si la condition n'est pas satisfaite

    Inst();              // Analyse le corps de la boucle

    GENERER2(LDA, addrVar); // Recharge l'adresse de la variable de boucle
    GENERER1(LDV);         // Recharge sa valeur
    GENERER2(LDI, 1);      // Charge la valeur 1 pour incrémenter/décrémenter
    if (sens == 0)
        GENERER1(ADD);    // Additionne 1 pour boucle croissante
    else
        GENERER1(SUB);    // Soustrait 1 pour boucle décroissante
    GENERER2(STO, addrVar); // Stocke la nouvelle valeur dans la variable
    GENERER2(BRN, condAddr); // Revient à la condition de boucle
    PCODE[jumpEnd].SUITE = PC + 1; // Fixe le saut de sortie
}

// ---------------------------------------------------------------------
// Analyse une structure "case"
// ---------------------------------------------------------------------
void CaseInst()
{
    Test_Symbole(CASE_TOKEN); // Consomme "case"
    Exp();             // Analyse l'expression à comparer
    int tmpSlot = OFFSET++; // Alloue un emplacement temporaire
    GENERER2(STO, tmpSlot);  // Stocke la valeur dans le slot temporaire
    Test_Symbole(OF_TOKEN);   // Consomme "of"

    int jumpEndLabels[50]; // Tableau pour stocker les labels de fin de chaque branche
    int nEnd = 0;          // Compteur de labels de fin

    // Traite chaque branche "valeur : instruction"
    while (SYM_COUR.CODE == ENT_TOKEN)
    {
        int labelVal = atoi(SYM_COUR.nom); // Convertit le numéro de label en entier
        Test_Symbole(ENT_TOKEN);
        Test_Symbole(COLON_TOKEN);

        GENERER2(LDA, tmpSlot); // Recharge la valeur stockée
        GENERER1(LDV);
        GENERER2(LDI, labelVal); // Charge le label courant
        GENERER1(EQL);          // Vérifie l'égalité

        int jumpIfNot = PC + 1;  // Prépare un saut si la condition est fausse
        GENERER2(BZE, 0);
        Inst();               // Analyse les instructions de la branche
        int jumpAfter = PC + 1; // Prépare un saut pour sortir de la branche
        GENERER2(BRN, 0);
        jumpEndLabels[nEnd++] = jumpAfter; // Enregistre le label de fin
        PCODE[jumpIfNot].SUITE = PC + 1; // Fixe la cible du saut si égalité non satisfaite

        if (SYM_COUR.CODE == PV_TOKEN)
            Test_Symbole(PV_TOKEN); // Consomme le point-virgule si présent
        else
            break;
    }
    if (SYM_COUR.CODE == ELSE_TOKEN)
    {
        Test_Symbole(ELSE_TOKEN); // Consomme "else"
        Inst();            // Analyse l'instruction pour la branche "else"
        int jumpAfterElse = PC + 1;
        GENERER2(BRN, 0);
        jumpEndLabels[nEnd++] = jumpAfterElse;
        if (SYM_COUR.CODE == PV_TOKEN)
            Test_Symbole(PV_TOKEN); // Consomme le point-virgule
    }
    Test_Symbole(END_TOKEN); // Consomme "end"
    int endOfCase = PC + 1; // Adresse de fin de la structure "case"
    // Fixe tous les sauts enregistrés à la fin de la structure
    for (int i = 0; i < nEnd; i++)
    {
        PCODE[jumpEndLabels[i]].SUITE = endOfCase;
    }
}

// ---------------------------------------------------------------------
// Analyse la partie des procédures et fonctions
// ---------------------------------------------------------------------
void ProcFuncPart()
{
    // Tant qu'on trouve "procedure" ou "function", on les traite
    while (SYM_COUR.CODE == PROCEDURE_TOKEN || SYM_COUR.CODE == FUNCTION_TOKEN)
    {
        if (SYM_COUR.CODE == PROCEDURE_TOKEN)
            ProcDecl(); // Déclaration d'une procédure
        else
            FuncDecl(); // Déclaration d'une fonction
    }
}

// ---------------------------------------------------------------------
// Déclaration d'une procédure : procedure <id> (paramlist); Bloc; RET <n>
// ---------------------------------------------------------------------
void ProcDecl()
{
    initLocalParams(); // Initialise les paramètres locaux

    Test_Symbole(PROCEDURE_TOKEN); // Consomme "procedure"
    char procName[32];
    strcpy(procName, SYM_COUR.nom); // Enregistre le nom de la procédure
    Test_Symbole(ID_TOKEN);

    if (IDexists(procName))
        Error("Procedure name already used"); // Erreur si le nom existe déjà
    int idx = NBR_IDFS; // Index pour enregistrer la procédure dans la table des identifiants

    strcpy(TAB_IDFS[idx].Nom, procName); // Enregistre le nom dans la table
    TAB_IDFS[idx].TIDF = TPROC;          // Marque comme procédure
    TAB_IDFS[idx].type = TYPE_UNDEF;     // Type non défini (procédure n'a pas de type de retour)
    TAB_IDFS[idx].Adresse = -1;          // Adresse initiale inconnue
    TAB_IDFS[idx].Value = 0;             // Nombre de paramètres initialisé à 0
    NBR_IDFS++;                          // Incrémente le nombre d'identifiants

    parseParamList(idx); // Analyse la liste des paramètres
    Test_Symbole(PV_TOKEN);   // Consomme le point-virgule

    // Enregistre l'adresse de début du code de la procédure
    int startPC = PC + 1;
    TAB_IDFS[idx].Adresse = startPC;

    // Charge chaque paramètre depuis le cadre d'appel et l'assigne au slot local correspondant
    for (int p = 0; p < TAB_IDFS[idx].Value; p++)
    {
        GENERER2(LDL, p); // Charge le paramètre depuis le cadre d'appel
        GENERER2(STL, p); // Stocke le paramètre dans le slot local
    }

    Bloc();         // Analyse le bloc de la procédure
    Test_Symbole(PV_TOKEN); // Consomme le point-virgule final

    // Génère l'instruction de retour avec le nombre de paramètres
    GENERER2(RET, TAB_IDFS[idx].Value);
}

// ---------------------------------------------------------------------
// Déclaration d'une fonction : function <id> (paramlist) : <type>; Bloc; (push result) RET <n>
// ---------------------------------------------------------------------
void FuncDecl()
{
    initLocalParams(); // Initialise les paramètres locaux

    Test_Symbole(FUNCTION_TOKEN); // Consomme "function"
    char fnName[32];
    strcpy(fnName, SYM_COUR.nom); // Enregistre le nom de la fonction
    Test_Symbole(ID_TOKEN);

    int idx = NBR_IDFS;      // Index pour la table des identifiants
    strcpy(TAB_IDFS[idx].Nom, fnName); // Enregistre le nom
    TAB_IDFS[idx].TIDF = TFUNC;        // Marque comme fonction
    TAB_IDFS[idx].Adresse = -1;        // Adresse initiale inconnue
    TAB_IDFS[idx].Value = 0;           // Nombre de paramètres initialisé à 0
    TAB_IDFS[idx].type = TYPE_INT;     // Par défaut, le type de retour est entier
    NBR_IDFS++;                        // Incrémente le nombre d'identifiants

    parseParamList(idx);  // Analyse la liste de paramètres et met à jour TAB_IDFS[idx].Value
    Test_Symbole(COLON_TOKEN); // Consomme ":"
    DataType retType = parseBaseType(); // Analyse le type de retour
    TAB_IDFS[idx].type = retType;       // Enregistre le type de retour
    Test_Symbole(PV_TOKEN);    // Consomme le point-virgule

    insideAFunction = 1; // Indique qu'on est dans une fonction
    strcpy(currentFunctionName, fnName); // Enregistre le nom de la fonction courante
    
    int startPC = PC + 1; // Adresse de début du code de la fonction
    TAB_IDFS[idx].Adresse = startPC;

    // Charge chaque paramètre depuis le cadre d'appel vers le slot local correspondant
    for (int p = 0; p < TAB_IDFS[idx].Value; p++) {
        GENERER2(LDL, p);
        GENERER2(STL, p);
    }

    Bloc();             // Analyse le bloc de la fonction
    Test_Symbole(PV_TOKEN);  // Consomme le point-virgule final

    // Pousse la valeur résultat de la fonction (stockée en local #0) sur la pile
    GENERER2(LDL, 0);
    
    // Génère l'instruction de retour avec le nombre de paramètres
    GENERER2(RET, TAB_IDFS[idx].Value);

    insideAFunction = 0;    // Quitte le contexte de fonction
    currentFunctionName[0] = '\0'; // Réinitialise le nom de la fonction courante
}

// ---------------------------------------------------------------------
// Analyse un appel ou une affectation à partir d'un identifiant
// ---------------------------------------------------------------------
void CallOrAssign()
{
    char name[32];
    strcpy(name, SYM_COUR.nom); // Sauvegarde le nom de l'identifiant
    Test_Symbole(ID_TOKEN);         // Consomme l'identifiant

    // Si on est dans une fonction et que l'identifiant correspond au nom de la fonction,
    // il s'agit d'une affectation au résultat de la fonction (local #0)
    if (insideAFunction && strcmp(name, currentFunctionName) == 0)
    {
        Test_Symbole(AFFECT_TOKEN); // Consomme ":="
        Exp();                 // Analyse l'expression à assigner
        GENERER2(STL, 0);       // Stocke le résultat dans le slot local #0
        return;
    }

    // Sinon, vérifie si l'identifiant correspond à une procédure ou fonction connue
    int isProc = isProcedure(name);
    int isFunc = isFunction(name);

    if (isProc || isFunc)
    {
        // Appel de procédure ou fonction
        int idxPF = getProcFuncIndex(name);

        // Analyse les arguments optionnels : soit "(...)" soit zéro argument
        if (SYM_COUR.CODE == PRG_TOKEN)
        {
            Test_Symbole(PRG_TOKEN);
            parseArguments(idxPF); // Analyse les arguments fournis
            Test_Symbole(PRD_TOKEN);
        }
        else
        {
            // Cas d'appel avec zéro argument
            GENERER2(PUSH_PARAMS_COUNT, 0);
        }
        // Génère l'instruction CALL pour effectuer l'appel
        GENERER2(CALL, TAB_IDFS[idxPF].Adresse);

        // Si l'appel de fonction est dans un contexte d'instruction, on doit jeter le résultat
        if (isFunc)
            GENERER2(STO, -9999); // Pop le résultat de la pile (valeur jetée)
    }
    else
    {
        // Sinon, c'est une affectation de variable : "ID := exp"
        Test_Symbole(AFFECT_TOKEN); // Consomme ":="
        Exp();                 // Analyse l'expression de droite
        // Détermine si l'identifiant est un paramètre local ou une variable globale
        int localIdx = findLocalParamIndex(name);
        if (localIdx >= 0)
        {
            GENERER2(LDL, localIdx); // Charge l'adresse du paramètre local
            GENERER1(STO_IND);       // Stocke indirectement la valeur dans la mémoire locale
        }
        else
        {
            int adr = getAdresse(name); // Récupère l'adresse globale de la variable
            GENERER2(STO, adr);          // Stocke la valeur dans l'adresse globale
        }
    }
}

// ---------------------------------------------------------------------
// Analyse la liste des paramètres dans une déclaration de proc/fonction
// ---------------------------------------------------------------------
static void parseParamList(int indexProcFunc)
{
    int total = 0; // Nombre total de paramètres
    if (SYM_COUR.CODE == PRG_TOKEN)
    {
        Test_Symbole(PRG_TOKEN); // Consomme '('
        while (SYM_COUR.CODE == ID_TOKEN)
        {
            char groupIDS[10][32]; // Groupe d'identifiants pour des paramètres partagés
            int gCount = 0;        // Compteur pour le groupe
            // Analyse une liste d'identifiants séparés par des virgules
            while (SYM_COUR.CODE == ID_TOKEN)
            {
                strcpy(groupIDS[gCount], SYM_COUR.nom); // Enregistre l'identifiant
                gCount++;
                Test_Symbole(ID_TOKEN); // Consomme l'identifiant
                if (SYM_COUR.CODE == VIR_TOKEN)
                    Test_Symbole(VIR_TOKEN); // Consomme la virgule
                else
                    break;
            }
            Test_Symbole(COLON_TOKEN);   // Consomme ':'
            DataType ptype = parseBaseType(); // Analyse le type de base du paramètre

            // Enregistre chaque identifiant du groupe comme paramètre local
            for (int i = 0; i < gCount; i++)
            {
                addLocalParam(groupIDS[i], ptype);
                total++;
            }

            if (SYM_COUR.CODE == PV_TOKEN)
                Test_Symbole(PV_TOKEN); // Consomme le point-virgule séparant les groupes
            else
                break;
        }
        Test_Symbole(PRD_TOKEN); // Consomme ')'
    }
    TAB_IDFS[indexProcFunc].Value = total; // Stocke le nombre total de paramètres dans la table d'identifiants
}

// ---------------------------------------------------------------------
// Analyse la liste des arguments dans un appel de proc/fonction
// ---------------------------------------------------------------------
static void parseArguments(int indexProcFunc)
{
    int nbParams = TAB_IDFS[indexProcFunc].Value; // Nombre de paramètres attendus
    int count = 0; // Compteur d'arguments fournis

    if (SYM_COUR.CODE != PRD_TOKEN)
    {
        while (1)
        {
            if (SYM_COUR.CODE == ID_TOKEN)
            {
                int addr = getAdresse(SYM_COUR.nom); // Récupère l'adresse d'un paramètre
                GENERER2(LDA, addr); // Charge l'adresse du paramètre
                Test_Symbole(ID_TOKEN); // Consomme l'identifiant
            }
            else
            {
                Exp(); // Analyse l'expression de l'argument
            }
            count++; // Incrémente le nombre d'arguments fournis

            if (SYM_COUR.CODE == VIR_TOKEN)
                Test_Symbole(VIR_TOKEN); // Consomme la virgule séparant les arguments
            else
                break; // Sort de la boucle s'il n'y a plus d'arguments
        }
    }

    // Vérifie que le nombre d'arguments fournis est correct
    if (count != nbParams)
    {
        char buf[128];
        sprintf(buf, "Incorrect number of parameters. Expected %d, got %d",
                nbParams, count);
        Error(buf);
    }

    // Pousse le nombre d'arguments sur la pile pour l'appel
    GENERER2(PUSH_PARAMS_COUNT, count);
}
