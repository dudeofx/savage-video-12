#ifndef __handlers_h__
#define __handlers_h__

int Handler_DummyNodeHandler(tTokenNode *node);
int Handler_ConsoleExit(tTokenNode *node);
int Handler_ConsoleHelp(tTokenNode *node);
int Handler_ConsoleListVars(tTokenNode *node);
int Handler_ConsoleListMem(tTokenNode *node);
int Handler_Operator(tTokenNode *node);
int Handler_Variable(tTokenNode *node);
int Handler_VariableInitialize(tTokenNode *node);

#endif

