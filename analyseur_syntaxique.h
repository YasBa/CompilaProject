#ifndef SYNTAXIQUE_H
#define SYNTAXIQUE_H

void Program();
void Bloc();
void Insts();
void Inst();
void Cond();
void Exp();
void Term();
void Fact();
void RepeatInst();
void ForInst();
void CaseInst();
void ProcFuncPart();
void ProcDecl();
void FuncDecl();
void CallOrAssign();

static int insideAFunction = 0;
static char currentFunctionName[64];

#endif
