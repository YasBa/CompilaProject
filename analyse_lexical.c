#include "analyse_lexical.h"

TSym_Cour SYM_COUR;
TSym_Cour symPre;        
FILE*     fsource = NULL;
int       car_cour = 0;
int       line_num = 1;

void Error(const char* msg)
{
    fprintf(stderr, "Erreur ligne %d: %s (dernier token: '%s')\n",line_num, msg, SYM_COUR.nom);
    exit(EXIT_FAILURE);
}

void LireCar()
{
    car_cour = fgetc(fsource);
    if (car_cour == '\n') {
        line_num++;
    }
}

static void toLowerString(char *s)
{
    for(int i = 0; s[i]; i++) {
        s[i] = (char)tolower(s[i]);
    }
}

static void lireNombre()
{
    char buf[64];
    int  indice = 0;
    int  isFloat = 0;

    while (indice < 63 && (isdigit(car_cour) || car_cour == '.')) {
        if (car_cour == '.') {
            if (isFloat) {
                break;
            }
            isFloat = 1;
        }
        buf[indice++] = (char)car_cour;
        LireCar();
    }
    buf[indice] = '\0';

    strncpy(SYM_COUR.nom, buf, sizeof(SYM_COUR.nom)-1);
    SYM_COUR.nom[sizeof(SYM_COUR.nom)-1] = '\0';
    SYM_COUR.CODE = (isFloat ? FLOAT_TOKEN : ENT_TOKEN);
}

static void lireMot()
{
    int indice = 0;
    while ((isalnum(car_cour) || car_cour=='_') && indice < 63) {
        SYM_COUR.nom[indice++] = (char)car_cour;
        LireCar();
    }
    SYM_COUR.nom[indice] = '\0';
    toLowerString(SYM_COUR.nom);

    if      (!strcmp(SYM_COUR.nom, "program"))   SYM_COUR.CODE = PROGRAM_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "var"))       SYM_COUR.CODE= VAR_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "const"))     SYM_COUR.CODE= CONST_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "type"))      SYM_COUR.CODE= TYPE_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "if"))        SYM_COUR.CODE= IF_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "then"))      SYM_COUR.CODE= THEN_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "else"))      SYM_COUR.CODE= ELSE_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "while"))     SYM_COUR.CODE = WHILE_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "do"))        SYM_COUR.CODE = DO_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "repeat"))    SYM_COUR.CODE= REPEAT_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "until"))     SYM_COUR.CODE= UNTIL_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "for"))       SYM_COUR.CODE= FOR_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "to"))        SYM_COUR.CODE= TO_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "downto"))    SYM_COUR.CODE= DOWNTO_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "case"))      SYM_COUR.CODE= CASE_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "of"))        SYM_COUR.CODE= OF_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "begin"))     SYM_COUR.CODE= BEGIN_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "end"))       SYM_COUR.CODE= END_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "write"))     SYM_COUR.CODE= WRITE_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "read"))      SYM_COUR.CODE= READ_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "procedure")) SYM_COUR.CODE= PROCEDURE_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "function"))  SYM_COUR.CODE= FUNCTION_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "integer"))   SYM_COUR.CODE= INT_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "real"))      SYM_COUR.CODE= FLOAT_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "boolean"))   SYM_COUR.CODE= BOOL_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "string"))    SYM_COUR.CODE= STRING_TOKEN;
    else                                         SYM_COUR.CODE= ID_TOKEN;
}

void SymSuiv()
{
    while (isspace(car_cour)) {
        LireCar();
    }

    if (car_cour == EOF || car_cour == -1) {
        SYM_COUR.CODE = DIEZE_TOKEN;
        strcpy(SYM_COUR.nom, "#EOF");
        return;
    }

    if (isalpha(car_cour)) {
        lireMot();
    }
    else if (isdigit(car_cour)) {
        lireNombre();
    }
    else {
        switch(car_cour) {
        case '+':
            SYM_COUR.CODE = PLUS_TOKEN;
            SYM_COUR.nom[0] = '+';
            SYM_COUR.nom[1] = '\0';
            LireCar();
            break;
        case '-':
            SYM_COUR.CODE= MOINS_TOKEN;
            SYM_COUR.nom[0] = '-';
            SYM_COUR.nom[1] = '\0';
            LireCar();
            break;
        case '*':
            SYM_COUR.CODE = MULTI_TOKEN;
            SYM_COUR.nom[0] = '*';
            SYM_COUR.nom[1] = '\0';
            LireCar();
            break;
        case '/':
            SYM_COUR.CODE = DIV_TOKEN;
            SYM_COUR.nom[0] = '/';
            SYM_COUR.nom[1] = '\0';
            LireCar();
            break;
        case ';':
            SYM_COUR.CODE = PV_TOKEN;
            SYM_COUR.nom[0] = ';';
            SYM_COUR.nom[1] = '\0';
            LireCar();
            break;
        case '.':
            SYM_COUR.CODE = PT_TOKEN;
            SYM_COUR.nom[0] = '.';
            SYM_COUR.nom[1] = '\0';
            LireCar();
            break;
        case '(':
            SYM_COUR.CODE = PRG_TOKEN;
            SYM_COUR.nom[0] = '(';
            SYM_COUR.nom[1] = '\0';
            LireCar();
            break;
        case ')':
            SYM_COUR.CODE = PRD_TOKEN;
            SYM_COUR.nom[0] = ')';
            SYM_COUR.nom[1] = '\0';
            LireCar();
            break;
        case '=':
            SYM_COUR.CODE = EGAL_TOKEN;
            SYM_COUR.nom[0] = '=';
            SYM_COUR.nom[1] = '\0';
            LireCar();
            break;
        case ',':
            SYM_COUR.CODE = VIR_TOKEN;
            SYM_COUR.nom[0] = ',';
            SYM_COUR.nom[1] = '\0';
            LireCar();
            break;
        case '<':
            LireCar();
            if (car_cour == '=') {
                SYM_COUR.CODE = INFEG_TOKEN;
                strcpy(SYM_COUR.nom, "<=");
                LireCar();
            }
            else if (car_cour == '>') {
                SYM_COUR.CODE = DIFF_TOKEN;
                strcpy(SYM_COUR.nom, "<>");
                LireCar();
            }
            else {
                SYM_COUR.CODE = INF_TOKEN;
                strcpy(SYM_COUR.nom, "<");
            }
            break;
        case '>':
            LireCar();
            if (car_cour == '=') {
                SYM_COUR.CODE = SUPEG_TOKEN;
                strcpy(SYM_COUR.nom, ">=");
                LireCar();
            }
            else {
                SYM_COUR.CODE = SUP_TOKEN;
                strcpy(SYM_COUR.nom, ">");
            }
            break;
        case ':':
            LireCar();
            if (car_cour == '=') {
                SYM_COUR.CODE = AFFECT_TOKEN;
                strcpy(SYM_COUR.nom, ":=");
                LireCar();
            }
            else {
                SYM_COUR.CODE = COLON_TOKEN;
                strcpy(SYM_COUR.nom, ":");
            }
            break;
        default:
            SYM_COUR.CODE = ERREUR_TOKEN;
            sprintf(SYM_COUR.nom, "%c", car_cour);
            Error("caractère inconnu");
            break;
        }
    }
}

void Test_Symbole (CODES_LEX cl)
{
    if (SYM_COUR.CODE == cl) {
        symPre = SYM_COUR;
        SymSuiv();
    }
    else {
        char buf[128];
<<<<<<< HEAD
        sprintf(buf, "Unexpected token. Expected %d, found %d", cl, SYM_COUR.CODE);
        Error(buf);
=======
        sprintf(buf, "Jeton inattendu. Attendu %d, trouvé %d", cl, SYM_COUR.CODE);
        Error(buf);  // Affiche une erreur si le token ne correspond pas
>>>>>>> d2ee875ffcd679402f58d31a485fff20d72e7d01
    }
}
