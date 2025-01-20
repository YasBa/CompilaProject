#include "analyse_lexical.h"

// Token courant qui contient le type et la chaîne associée
TSym_Cour SYM_COUR;

// Token précédent, utilisé pour garder en mémoire le dernier token consommé
TSym_Cour symPre;        

// Fichier source à analyser (souvent ouvert avant l'analyse)
FILE*     fsource = NULL;

// Caractère courant lu dans le fichier source
int       car_cour = 0;

// Numéro de la ligne en cours (pour l'affichage d'erreurs par exemple)
int       line_num = 1;

// Affiche une erreur avec le numéro de ligne et le token qui pose problème, puis quitte le programme
void Error(const char* msg)
{
    fprintf(stderr, "Error line %d: %s (last token: '%s')\n",
            line_num, msg, SYM_COUR.nom);
    exit(EXIT_FAILURE);
}

// Lit le caractère suivant dans le fichier source et met à jour le numéro de ligne si nécessaire
void LireCar()
{
    car_cour = fgetc(fsource);
    if (car_cour == '\n') {
        line_num++;  // On passe à la ligne suivante
    }
}

// Convertit une chaîne en minuscules, modifiant la chaîne directement
static void toLowerString(char *s)
{
    for(int i = 0; s[i]; i++) {
        s[i] = (char)tolower((unsigned char)s[i]);  // Convertit chaque caractère en minuscule
    }
}

// Lit un nombre depuis le flux (peut être un entier ou un réel)
// Stocke le résultat dans symCour.nom et définit le type de token (NUM_TOKEN ou REAL_TOKEN)
static void lireNombre()
{
    char buf[64];   // Buffer pour construire la chaîne représentant le nombre
    int  i = 0;     // Position actuelle dans le buffer
    int  isReal = 0;  // Indique si le nombre contient un point (donc réel)

    // Tant que le caractère courant est un chiffre ou un point et qu'on ne dépasse pas la taille du buffer
    while (i < 63 && (isdigit(car_cour) || car_cour == '.')) {
        if (car_cour == '.') {
            if (isReal) {
                // Si un point a déjà été trouvé, on arrête la lecture
                break;
            }
            isReal = 1;  // Marque que c'est un réel
        }
        buf[i++] = (char)car_cour;  // Stocke le caractère dans le buffer
        LireCar();  // Lit le caractère suivant
    }
    buf[i] = '\0';  // Termine la chaîne

    // Copie la chaîne dans le token courant
    strncpy(SYM_COUR.nom, buf, sizeof(SYM_COUR.nom)-1);
    SYM_COUR.nom[sizeof(SYM_COUR.nom)-1] = '\0';

    // Définit le type du token en fonction de la présence d'un point
    SYM_COUR.CODE = (isReal ? REAL_TOKEN : NUM_TOKEN);
}

// Lit un mot (identifiant ou mot-clé) depuis le flux d'entrée
static void lireMot()
{
    int i = 0;  // Index pour construire le mot
    // Tant que le caractère est alphanumérique ou un '_' et que le buffer n'est pas plein
    while ((isalnum(car_cour) || car_cour=='_') && i < 63) {
        SYM_COUR.nom[i++] = (char)car_cour;  // Ajoute le caractère au mot
        LireCar();  // Lit le caractère suivant
    }
    SYM_COUR.nom[i] = '\0';  // Termine le mot par un caractère nul
    toLowerString(SYM_COUR.nom);  // Met le mot en minuscules pour faciliter la comparaison

    // Vérifie si le mot correspond à un mot-clé connu et définit le type du token
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
    else if (!strcmp(SYM_COUR.nom, "real"))      SYM_COUR.CODE= REAL_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "boolean"))   SYM_COUR.CODE= BOOL_TOKEN;
    else if (!strcmp(SYM_COUR.nom, "string"))    SYM_COUR.CODE= STRING_TOKEN;
    else                                       SYM_COUR.CODE= ID_TOKEN;  // Sinon, c'est un identifiant
}

// Passe au prochain symbole (token) dans le flux d'entrée
void SymSuiv()
{
    // Ignore tous les espaces, retours à la ligne, etc.
    while (isspace(car_cour)) {
        LireCar();  // Lit le prochain caractère jusqu'à trouver un non-espace
    }

    // Si on atteint la fin du fichier, définit un token spécial de fin
    if (car_cour == EOF || car_cour == -1) {
        SYM_COUR.CODE = DIEZE_TOKEN;
        strcpy(SYM_COUR.nom, "#EOF");
        return;
    }

    // Si le caractère est une lettre, le mot (identifiant ou mot-clé) est lu
    if (isalpha(car_cour)) {
        lireMot();
    }
    // Si le caractère est un chiffre, lit un nombre
    else if (isdigit(car_cour)) {
        lireNombre();
    }
    else {
        // Sinon, c'est un symbole ou un opérateur
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
            LireCar();  // Passe au caractère suivant pour vérifier le type
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
            // Si aucun cas ne correspond, c'est un caractère inconnu
            SYM_COUR.CODE = ERREUR_TOKEN;
            sprintf(SYM_COUR.nom, "%c", car_cour);
            Error("Unknown character");
            break;
        }
    }
}

// Vérifie que le token courant correspond au token attendu, puis passe au suivant
void Test_Symbole (CODES_LEX t)
{
    if (SYM_COUR.CODE == t) {
        symPre = SYM_COUR;  // Sauvegarde le token courant en tant que token précédent
        SymSuiv();         // Passe au token suivant
    }
    else {
        char buf[128];
        sprintf(buf, "Unexpected token. Expected %d, found %d", t, SYM_COUR.CODE);
        Error(buf);  // Affiche une erreur si le token ne correspond pas
    }
}
