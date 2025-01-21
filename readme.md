# la Grammaire du Langage



## Éléments Lexicaux

### Tokens
Le langage utilise les tokens suivants :
- Mots-clés : `program`, `const`, `type`, `var`, `procedure`, `function`, `if`, `then`, `else`, `while`, `do`, `repeat`, `until`, `for`, `to`, `downto`, `case`, `of`, `begin`, `end`, `write`, `read`
- Types de données : `integer`, `real`, `boolean`, `string`
- Opérateurs : `+`, `-`, `*`, `/`, `:=`, `<>`, `<`, `>`, `<=`, `>=`
- Délimiteurs : `;`, `=`, `,`, `:`, `(`, `)`, `.`

### Non-Terminaux
La grammaire inclut des non-terminaux pour la structure du programme, les déclarations, les instructions et les expressions :

- Structure du programme : `PROGRAM`, `BLOCK`
- Déclarations : `CONSTS`, `CONST_DECL`, `TYPES`, `TYPE_DECL`, `VARS`, `VAR_DECL`
- Procédures et Fonctions : `PROCFUNCPART`, `PROCDECL`, `FUNCDECL`, `PARAMLIST`, `PARAMSECTION`
- Instructions : `INSTRUCTIONS`, `INSTRUCTION`, `AFFECT`, `IF`, `WHILE`, `REPEAT`, `FOR`, `CASE`, `WRITE`, `READ`, `CALL`
- Expressions : `COND`, `EXP`, `RELOP`, `TERM`, `ADDOP`, `FACT`, `CASE_ELEMENT`
- Listes : `EXPR_LIST`, `IDENT_LIST`, `ARG_LIST`
- Types de base : `BASE_TYPE`, `ID`, `NUM`, `REAL`

## Règles de Production

### Structure globale
La structure de tout programme suit les règles suivantes :
```
PROGRAM -> "program" ID ";" BLOCK "."
BLOCK -> CONSTS TYPES VARS PROCFUNCPART INSTS
```

### Déclarations de Constantes
```
CONSTS -> "const" CONST_DECL { ";" CONST_DECL } ";" | ε
CONST_DECL -> ID "=" CONST_VALUE
CONST_VALUE -> NUM | REAL
```

### Déclarations de Types
```
TYPES -> "type" TYPE_DECL { ";" TYPE_DECL } ";" | ε
TYPE_DECL -> ID "=" BASE_TYPE
```

### Déclarations de Variables
```
VARS -> "var" VAR_LIST ";" | ε
VAR_LIST -> ID { "," ID } ":" BASE_TYPE
```

### Procédures et Fonctions
```
PROCFUNCPART -> PROCDECL | FUNCDECL | PROCDECL PROCFUNCPART | FUNCDECL PROCFUNCPART | ε
PROCDECL -> "procedure" ID [ "(" PARAMLIST ")" ] ";" BLOCK ";"
FUNCDECL -> "function" ID [ "(" PARAMLIST ")" ] ":" BASE_TYPE ";" BLOCK ";"
PARAMLIST -> "(" PARAMSECTION { ";" PARAMSECTION } ")" | ε
PARAMSECTION -> ID { "," ID } ":" BASE_TYPE
```

### Instructions
```
INSTRUCTIONS -> "begin" INST { ";" INST } "end"
INSTRUCTION -> AFFECT
      | IF
      | WHILE 
      | REPEAT
      | FOR 
      | CASE 
      | WRITE
      | READ 
      | CALL
```

### Instructions de Base
```
AFFECT-> ID ":=" EXPR
IF -> "if" COND "then" INST [ "else" INST ]
WHILE -> "while" COND "do" INST
REPEAT -> "repeat" INST "until" COND
FOR -> "for" ID ":=" EXPR ("to" | "downto") EXPR "do" INST
CASE -> "case" EXPR "of" CASE_ELEMENT { ";" CASE_ELEMENT } [ "else" INST ] "end"
CASE_ELEMENT -> NUM ":" INST
WRITE -> "write" "(" EXPR_LIST ")"
READ-> "read" "(" IDENT_LIST ")"
CALL -> ID "(" [ ARG_LIST ] ")"
```

### Expressions et Conditions
```
COND -> EXPR RELOP EXPR
RELOP -> "=" | "<>" | "<" | ">" | "<=" | ">="
EXPR -> TERM { ADDOP TERM }
ADDOP -> "+" | "-"
TERM -> FACT { MULOP FACT }
MULOP -> "*" | "/"
FACT -> ID [ "(" EXPR { "," EXPR } ")" ] 
      | NUM 
      | REAL 
      | "(" EXPR ")"
```

### Liste et Types de Base
```
EXPR_LIST -> EXPR { "," EXPR }
IDENT_LIST -> ID { "," ID }
ARG_LIST -> EXPR { "," EXPR }
ARRAY_LIST -> ID "=" "[" REAL { "," REAL } "]" 
BASE_TYPE -> "integer" | "real" | "boolean" | "string" | "array"
```

### Définitions Lexicales
```
ID -> lettre { lettre | chiffre | "_" }
NUM -> chiffre { chiffre }
REAL -> NUM "." NUM

lettre -> "a" | "b" | ... | "z" | "A" | "B" | ... | "Z"
chiffre -> "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
```

# Grammaire et Mnémoniques du P-CODE

## **Grammaire**

| **Non-Terminal** | **Production**                                                                                 |
|------------------|-----------------------------------------------------------------------------------------------|
| **PCODE**        | `PCODE ::= INT NUM {INST_PCODE} HLT`                                                         |
| **INST_PCODE**   | `INST_PCODE ::= ADD | SUB | EQL | ... | [LDA | BZE | BRN | LDI] NUM`                          |
| **NUM**          | `NUM ::= CHIFFRE {CHIFFRE}`                                                                   |
| **CHIFFRE**      | `CHIFFRE ::= 1 | 2 | 3 | ... | 9`                                                             |

### **Résumé de la Grammaire**
- Un programme P-CODE suit la structure :
  - Initialisation avec `INT NUM`
  - Une séquence d’instructions `{INST_PCODE}`
  - Arrêt avec `HLT`
- Les instructions peuvent inclure des opérations mathématiques (`ADD`, `SUB`), des comparaisons (`EQL`), et des contrôles (`BZE`, `BRN`).
- Les nombres sont composés d’un ou plusieurs chiffres entre 1 et 9.

---

## **Terminaux et Non-Terminaux**

| **Catégorie**    | **Symboles**                                                                                  |
|------------------|-----------------------------------------------------------------------------------------------|
| **Terminaux**    | `INT`, `HLT`, `ADD`, `SUB`, `EQL`, `LDA`, `BZE`, `BRN`, `LDI`, `NUM`, `CHIFFRE`, `1` à `9`     |
| **Non-terminaux**| `PCODE`, `INST_PCODE`, `NUM`, `CHIFFRE`                                                       |

---

## **Mnémoniques**

| **Instruction** | **Description**                                                                                 |
|------------------|-----------------------------------------------------------------------------------------------|
| **ADD**         | Additionne le sous-sommet de pile et le sommet, laisse le résultat au sommet (idem pour `SUB`, `MUL`, `DIV`). |
| **EQL**         | Laisse 1 au sommet de pile si sous-sommet = sommet, 0 sinon (idem pour `NEQ`, `GTR`, `LSS`, `GEQ`, `LEQ`). |
| **PRN**         | Imprime le sommet, dépile.                                                                     |
| **INN**         | Lit un entier, le stocke à l'adresse trouvée au sommet de pile, dépile.                        |
| **INT c**       | Incrémente de la constante `c` le pointeur de pile (la constante `c` peut être négative).       |
| **LDI v**       | Empile la valeur `v`.                                                                          |
| **LDA a**       | Empile l'adresse `a`.                                                                          |
| **LDV**         | Remplace le sommet par la valeur trouvée à l'adresse indiquée par le sommet (déréférence).      |
| **STO**         | Stocke la valeur au sommet à l'adresse indiquée par le sous-sommet, dépile 2 fois.             |
| **BRN i**       | Branchement inconditionnel à l'instruction `i`.                                                |
| **BZE i**       | Branchement à l'instruction `i` si le sommet = 0, dépile.                                      |
| **HLT**         | Halte.                                                                                         |

---

## **Résumé**

Cette grammaire et ces mnémoniques définissent un langage P-CODE avec :
- Une structure fixe incluant initialisation (`INT`), instructions, et fin (`HLT`).
- Un ensemble d'instructions permettant des calculs, des comparaisons, et des contrôles.
- Une distinction claire entre **terminaux** (éléments lexicaux du langage) et **non-terminaux** (éléments syntaxiques définissant la structure).

---
# **Try it urself**

```
//compile
gcc -o main.exe .\main.c .\analyseur_lexical.c .\analyseur_syntaxique.c .\analyseur_semantique.c .\interpreteur.c .\generation_code.c

//execute
.\main.exe .\TESTS\smit_test .\PCODE.po
```