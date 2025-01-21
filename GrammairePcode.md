# Grammaire et Mnémoniques du P-CODE



## **Symboles Terminaux et Non-Terminaux**

| **Catégorie**    | **Symboles**                                                                                  |
|------------------|-----------------------------------------------------------------------------------------------|
| **Terminaux**    | `INT`, `HLT`, `ADD`, `SUB`, `EQL`, `LDA`, `BZE`, `BRN`, `LDI`, `NUM`, `CHIFFRE`, `1` à `9`     |
| **Non-terminaux**| `PCODE`, `INST_PCODE`, `NUM`, `CHIFFRE`                                                       |


---

## **Grammaire**

```
PCODE -> INT NUM {INST_PCODE} HLT
INST_PCODE -> ADD | SUB | EQL | ... | [LDA | BZE | BRN | LDI] NUM
NUM -> CHIFFRE {CHIFFRE}
CHIFFRE -> 1 | 2 | 3 | ... | 9
```


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

