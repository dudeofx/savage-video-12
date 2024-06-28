#ifndef __parsers_h__
#define __parsers_h__

// see rule book or PrintError() function
#define RULE_1001 -1001
#define RULE_1002 -1002
#define RULE_1003 -1003
#define RULE_1004 -1004


#define PARSE_ENTRY_CHKPNT(ctrl)                         \
int exit_code = 0;                                       


#define EXIT_PARSE(code)                                 \
{ exit_code = code; goto _clean_exit_; }


#define EXIT_PARSE_ON_DISCORD(check, code)               \
{ if (check < 0) FATALITY(check); }                      \
{ if (check == 0) EXIT_PARSE(code); }


#define EXIT_PARSE_ON_MATCH(check, code)                 \
{ if (check < 0) FATALITY(check); }                      \
{ if (check > 0) EXIT_PARSE(code); }


#define PARSE_EXIT_CHKPNT(ctrl, inx)                     \
_clean_exit_:                                            \
if (ctrl->reach < inx) ctrl->reach = inx;


#define FATALITY(code)                                   \
{ exit_code = code;  goto _fatal_exit_; }


#define PARSE_FATALITY_EXIT(ctrl)                        \
_fatal_exit_:                                            


int Parse_AssignmentStatement(tParseCtrl *ctrl, int inx, tTokenNode **pending);
int Parse_CodeStatement(tParseCtrl *ctrl, int inx, tTokenNode **pending);
int Parse_ConsoleCommands(tParseCtrl *ctrl, int inx, tTokenNode **pending);
int Parse_ConsoleExit(tParseCtrl *ctrl, int inx, tTokenNode **pending);
int Parse_ConsoleHelp(tParseCtrl *ctrl, int inx, tTokenNode **pending);
int Parse_ConsoleListVars(tParseCtrl *ctrl, int inx, tTokenNode **pending);
int Parse_ConsoleListMem(tParseCtrl *ctrl, int inx, tTokenNode **pending);

int Parse_EscSeq(tParseCtrl *ctrl, int inx, int *value);
int Parse_Expression(tParseCtrl *ctrl, int inx, tTokenNode **pending);
int Parse_Identifier(tParseCtrl *ctrl, int inx, tTokenNode **pending);
int Parse_LineInput(tParseCtrl *ctrl, int inx);

int Parse_Literal(tParseCtrl *ctrl, int inx, tTokenNode **pending);
int Parse_LiteralBoolean(tParseCtrl *ctrl, int inx, tTokenNode **pending);
int Parse_LiteralChar(tParseCtrl *ctrl, int inx, int *value);
int Parse_LiteralDecimal(tParseCtrl *ctrl, int inx, int *value);
int Parse_LiteralIntegral(tParseCtrl *ctrl, int inx, tTokenNode **pending);
int Parse_LiteralHex(tParseCtrl *ctrl, int inx, int *value);
int Parse_LiteralNull(tParseCtrl *ctrl, int inx, tTokenNode **pending);
int Parse_LiteralString(tParseCtrl *ctrl, int inx, tTokenNode **pending);

int Parse_MatchPattern(tParseCtrl *ctrl, int inx, char *match, int match_lim);
int Parse_Primary(tParseCtrl *ctrl, int inx, tTokenNode **pending);
int Parse_PrimaryArray(tParseCtrl *ctrl, int inx, tTokenNode *parent);
int Parse_PrimaryBasic(tParseCtrl *ctrl, int inx, tTokenNode *parent);
int Parse_SavageType(tParseCtrl *ctrl, int inx, int *type, int *family);
int Parse_VariableDeclaration(tParseCtrl *ctrl, int inx, tTokenNode **pending);
int Parse_WhiteSpaces(tParseCtrl *ctrl, int inx);

#endif

