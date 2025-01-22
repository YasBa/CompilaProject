#include "analyseur_lexical.h"

TSym_Cour SYM_COUR;
TSym_Cour symPre;        
FILE*     fsource = NULL;
int       car_cour = 0;
int       line_num = 1;

void Error(const char* msg)
{
    fprintf(stderr, "Erreur : %s\nLigne : %d\n",msg , line_num);
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
    else if (!strcmp(SYM_COUR.nom, "array"))     SYM_COUR.CODE= ARRAY_TOKEN;
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
        sprintf(buf, "Token inattendu. Attendu %d, trouvé %d", cl, SYM_COUR.CODE);
        Error(buf);
    }
}


const char *getErrorMessage(CODES_ERR error_code)
{
    switch (error_code)
    {
    case ID_ERR:
        return "Identifier error";
    case PROGRAM_ERR:
        return "Program error";
    case CONST_ERR:
        return "Constant error";
    case VAR_ERR:
        return "Variable error";
    case BEGIN_ERR:
        return "Begin error";
    case END_ERR:
        return "End error";
    case IF_ERR:
        return "If error";
    case THEN_ERR:
        return "Then error";
    case WHILE_ERR:
        return "While error";
    case DO_ERR:
        return "Do error";
    case READ_ERR:
        return "Read error";
    case WRITE_ERR:
        return "Write error";
    case PV_ERR:
        return "Semicolon error";
    case PT_ERR:
        return "Period error";
    case PLUS_ERR:
        return "Plus error";
    case MOINS_ERR:
        return "Minus error";
    case MULT_ERR:
        return "Multiplication error";
    case DIV_ERR:
        return "Division error";
    case VIR_ERR:
        return "Comma error";
    case AFF_ERR:
        return "Assignment error";
    case INF_ERR:
        return "Less than error";
    case INFEG_ERR:
        return "Less than or equal to error";
    case SUP_ERR:
        return "Greater than error";
    case SUPEG_ERR:
        return "Greater than or equal to error";
    case DIFF_ERR:
        return "Not equal to error";
    case PO_ERR:
        return "Opening parenthesis error";
    case PF_ERR:
        return "Closing parenthesis error";
    case FIN_ERR:
        return "End of file error";
    case INTEGER_DATA_ERR:
        return "Integer format error";
    case FLOAT_DATA_ERR:
        return "Float format error";
    case BOOLEAN_DATA_ERR:
        return "Boolean format error";
    case STRING_DATA_ERR:
        return "String format error";
    case ARRAY_DATA_ERR:
        return "Array format error";
    case ERREUR_ERR:
        return "General error";
    case EOF_ERR:
        return "End of file error";
    case EG_ERR:
        return "Equality error";
    case CONST_VAR_BEGIN_ERR:
        return "Constant/Variable/Begin declaration error";
    case VAR_BEGIN_ERR:
        return "Variable/Begin declaration error";
    case INTEGER_ERR:
        return "Integer error";
    case FLOAT_ERR:
        return "Float error";
    case BOOLEAN_ERR:
        return "Boolean error";
    case STRING_ERR:
        return "String error";
    case ARRAY_ERR:
        return "Array error";
    case REPEAT_ERR:
        return "Repeat error";
    case UNTIL_ERR:
        return "Until error";
    case FOR_ERR:
        return "For error";
    case ELSE_ERR:
        return "Else error";
    case CASE_ERR:
        return "Case error";
    case OF_ERR:
        return "Of error";
    case INTO_ERR:
        return "Into error";
    case DOWNTO_ERR:
        return "Downto error";
    case DDOT_ERR:
        return "Double dot error";
    default:
        return "Unknown error";
    }
}