#ifndef __main_h__
#define __main_h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "tString.h"
#include "tSymTbl.h"

#define MAX_INPUT   0xFFFF

// Data Families
enum {
   FAM_Unknown,
   FAM_Error,
   FAM_Null,
   FAM_Void,
   FAM_Console,
   FAM_Boolean,
   FAM_Integral,
   FAM_String,
   FAM_Structure,
   FAM_CNT
};

// Data Types
enum {
   TYPE_void,
   TYPE_boolean,
   TYPE_byte,
   TYPE_char,
   TYPE_short,
   TYPE_int,
   TYPE_long,
   TYPE_string,
   TYPE_INVALID
};

// Token Types
enum {
   TKN_INVALID,
   TKN_EXIT,
   TKN_HELP,
   TKN_LSVARS,
   TKN_RUN,
   TKN_IMPORT,
   TKN_Operator,
   TKN_Literal,
   TKN_Variable,
   TKN_NULL,
   TKN_Initialize,
   TKN_CNT
};

// operators
enum {
   OPR_NOP,
   OPR_ASSIGN,
   OPR_ADD,
   OPR_SUB,
   OPR_MUL,
   OPR_DIV,
   OPR_MOD,
   OPR_NOT,
   OPR_NEG,
   OPR_CNT  
};

typedef struct _tParseCtrl tParseCtrl;
typedef struct _tTokenNode tTokenNode;

// a Token node is a node in a syntax tree
struct _tTokenNode {
   tParseCtrl *ctrl;
   int         token;
   int         opcode;
   int         family;
   int         p1;
   int         p2;
   tString    *p3;
   int         r_flags;
   int         r1;
   tString    *r2;
  
   int (*handler_func)(tTokenNode *node);
   tTokenNode *parent;
   tTokenNode *sibling;
   tTokenNode *eldest;
};

// maintains the state of parsing
struct _tParseCtrl {
   char         *buff;      // text buffer
   int           len;       // length of text
   int           lim;       // length of buffer
   int           reach;     // in case of error, this is the index where the parsers failed
   tSymTbl      *vars;
   tTokenNode   *root;      // pointer to the syntax tree being constructed
};

extern int exit_flag;

#include "UTF8_Utils.h"
#include "parsers.h"
#include "handlers.h"

//===[utilities that deal with input]============================================
void FlushInput();
int GetInput(tParseCtrl *ctrl);
void ResetCtrl(tParseCtrl *ctrl);
//===[utilities that deal with node management]==================================
void AttachNode(tParseCtrl *ctrl, tTokenNode *parent, tTokenNode *node);
tTokenNode *CreateNode(tParseCtrl *ctrl, int token, int family);
void DestroyTokenNode(tTokenNode *node);
void ExecParseTree(tTokenNode *root);
//===[Misc]======================================================================
tString *DecodeEscSeq(char *src, int src_len);
char *StrType(int type);
//-------------------------------------------------------------------------------


#endif

