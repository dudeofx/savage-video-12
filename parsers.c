#include "main.h"

//-------------------------------------------------------------------------------
int Parse_AssignmentStatement(tParseCtrl *ctrl, int inx, tTokenNode **pending) {
   int   parse;
   int   mark = inx;
   char *ident_ptr = NULL;
   int   ident_len = 0;
   tTokenNode *expression = NULL;
   
PARSE_ENTRY_CHKPNT(ctrl);

   parse = Parse_Identifier(ctrl, inx, NULL);
   EXIT_PARSE_ON_DISCORD(parse, 0);
   ident_ptr = ctrl->buff += inx;
   ident_len = parse;
   inx += parse;

   parse = Parse_WhiteSpaces(ctrl, inx);
   inx += parse;

   parse = Parse_MatchPattern(ctrl, inx, ":=", 2);
   EXIT_PARSE_ON_DISCORD(parse, 0);
   inx += parse;

   parse = Parse_WhiteSpaces(ctrl, inx);
   inx += parse;

   parse = Parse_Expression(ctrl, inx, &expression);
   EXIT_PARSE_ON_DISCORD(parse, 0);
   inx += parse;

   exit_code = (inx - mark);
  
PARSE_EXIT_CHKPNT(ctrl, inx);

   if ((exit_code > 0) && (pending != NULL)) {
  
      tSymTblEntry *entry = tSymTbl_GetEntry_Using_CharPtr(ctrl->vars, ident_ptr, ident_len);
      if (entry == NULL) FATALITY(RULE_1001); // unknown identifier
      
      if (expression != NULL) {
         if (expression->family != entry->family) FATALITY(RULE_1004); // duplicate identifier
      }
   
      *pending = CreateNode(ctrl, TKN_Operator, FAM_Unknown);
      (*pending)->opcode = OPR_ASSIGN;
      (*pending)->p3 = tString_Create(ident_ptr, ident_len);
      (*pending)->handler_func = Handler_Operator;

      AttachNode(ctrl, *pending, expression);

      entry->flags |= tSymTbl_VAR_INITIALIZED;
   }

   return exit_code;  
   
PARSE_FATALITY_EXIT(ctrl);
  
   if (expression != NULL) {
      DestroyTokenNode(expression);
   }

   return exit_code;  
}
//-------------------------------------------------------------------------------
int Parse_CodeStatement(tParseCtrl *ctrl, int inx, tTokenNode **pending) {
   int parse;

PARSE_ENTRY_CHKPNT(ctrl);

   parse = Parse_VariableDeclaration(ctrl, inx, pending);
   EXIT_PARSE_ON_MATCH(parse, parse);
   
   parse = Parse_AssignmentStatement(ctrl, inx, pending);
   EXIT_PARSE_ON_MATCH(parse, parse);

   parse = Parse_Expression(ctrl, inx, pending);
   EXIT_PARSE_ON_MATCH(parse, parse);
   
PARSE_EXIT_CHKPNT(ctrl, inx);
   return exit_code;

PARSE_FATALITY_EXIT(ctrl);
   
   if (pending != NULL) {
      if (*pending != NULL) {
         DestroyTokenNode(*pending);
         *pending = NULL;
      }
   }

   return exit_code;  
}
//-------------------------------------------------------------------------------
int Parse_ConsoleCommands(tParseCtrl *ctrl, int inx, tTokenNode **pending) {
   int parse;

PARSE_ENTRY_CHKPNT(ctrl);

   parse = Parse_ConsoleExit(ctrl, inx, pending);
   EXIT_PARSE_ON_MATCH(parse, parse);
      
   parse = Parse_ConsoleHelp(ctrl, inx, pending);
   EXIT_PARSE_ON_MATCH(parse, parse);
   
   parse = Parse_ConsoleListVars(ctrl, inx, pending);
   EXIT_PARSE_ON_MATCH(parse, parse);
   
   parse = Parse_ConsoleListMem(ctrl, inx, pending);
   EXIT_PARSE_ON_MATCH(parse, parse);
   
PARSE_EXIT_CHKPNT(ctrl, inx);
   return exit_code;

PARSE_FATALITY_EXIT(ctrl);
   
   if (pending != NULL) {
      if (*pending != NULL) {
         DestroyTokenNode(*pending);
         *pending = NULL;
      }
   }

   return exit_code;  
}
//-----------------------------------------------------------------------------------------
int Parse_ConsoleExit(tParseCtrl *ctrl, int inx, tTokenNode **pending) {
   int parse;

PARSE_ENTRY_CHKPNT(ctrl);
     
   parse = Parse_MatchPattern(ctrl, inx, "!exit", 5);
   EXIT_PARSE_ON_DISCORD(parse, 0);
     
   exit_code = parse;
   
PARSE_EXIT_CHKPNT(ctrl, inx);

   if ((exit_code > 0) && (pending != NULL)) {
       *pending = CreateNode(ctrl, TKN_EXIT, FAM_Console);
       (*pending)->handler_func = Handler_ConsoleExit;
   }
   return exit_code;

PARSE_FATALITY_EXIT(ctrl);
   return exit_code;  
}
//-------------------------------------------------------------------------------
int Parse_ConsoleHelp(tParseCtrl *ctrl, int inx, tTokenNode **pending) {
   int parse;

PARSE_ENTRY_CHKPNT(ctrl);

   parse = Parse_MatchPattern(ctrl, inx, "!help", 5);
   EXIT_PARSE_ON_DISCORD(parse, 0);
  
   exit_code = parse;
   
PARSE_EXIT_CHKPNT(ctrl, inx);

   if ((exit_code > 0) && (pending != NULL)) {
       *pending = CreateNode(ctrl, TKN_HELP, FAM_Console);
       (*pending)->handler_func = Handler_ConsoleHelp;
   }
   return exit_code;

PARSE_FATALITY_EXIT(ctrl);
   return exit_code;  
}
//-------------------------------------------------------------------------------
int Parse_ConsoleListVars(tParseCtrl *ctrl, int inx, tTokenNode **pending) {
   int parse;

PARSE_ENTRY_CHKPNT(ctrl);

   parse = Parse_MatchPattern(ctrl, inx, "!ls_vars", 8);
   EXIT_PARSE_ON_DISCORD(parse, 0);
   
   exit_code = parse;
   
PARSE_EXIT_CHKPNT(ctrl, inx);

   if ((exit_code > 0) && (pending != NULL)) {
       *pending = CreateNode(ctrl, TKN_LSVARS, FAM_Console);
       (*pending)->handler_func = Handler_ConsoleListVars;
   }
   return exit_code;

PARSE_FATALITY_EXIT(ctrl);
   return exit_code;  
}
//-------------------------------------------------------------------------------
int Parse_ConsoleListMem(tParseCtrl *ctrl, int inx, tTokenNode **pending) {
   int parse;

PARSE_ENTRY_CHKPNT(ctrl);

   parse = Parse_MatchPattern(ctrl, inx, "!ls_mem", 7);
   EXIT_PARSE_ON_DISCORD(parse, 0);

   exit_code = parse;
   
PARSE_EXIT_CHKPNT(ctrl, inx);

   if ((exit_code > 0) && (pending != NULL)) {
//       *pending = CreateNode(ctrl, TKN_LSMEM, FAM_Console);
//       (*pending)->handler_func = Handler_ConsoleListMem;
   }
   return exit_code;

PARSE_FATALITY_EXIT(ctrl);
   return exit_code;  
}
//-------------------------------------------------------------------------------
int Parse_EscSeq(tParseCtrl *ctrl, int inx, int *value) {
   char *key[] = {"\\t", "\\'", "\\\"", "\\r", "\\\\", "\\n", "\\f", "\\b"};
   int val[] = {'\t', '\'',  '\"', '\r', '\\', '\n', '\f', '\b'};
   char *buff = ctrl->buff + inx;
   int lim = ctrl->len - inx;
   int parse;
   int digit;
   int num = 0;

PARSE_ENTRY_CHKPNT(ctrl);
   
   for (int i = 0; i < 8; i++) {
      parse = Parse_MatchPattern(ctrl, inx, key[i], 2);
      if (parse > 0) {
         if (value != NULL) *value = val[i];
         EXIT_PARSE(2);
      }
   }
   
   parse = Parse_MatchPattern(ctrl, inx, "\\x", 2);
   if (parse > 0) {
      for (int i = 0; i < 6; i++) {
         digit = -1;
         if (parse >= lim) EXIT_PARSE(0); // buffer overflow
         if ((buff[parse] >= '0') && (buff[parse] <= '9')) digit = buff[parse] - '0';
         if ((buff[parse] >= 'a') && (buff[parse] <= 'f')) digit = buff[parse] - 'a' + 10;
         if ((buff[parse] >= 'A') && (buff[parse] <= 'F')) digit = buff[parse] - 'A' + 10;
         if (digit < 0) break;
         num = num * 16 + digit;
         parse++;
      }
      if (parse <= 2) EXIT_PARSE(0);           // pattern must have at least one hex digit
      if (buff[parse] != ';') EXIT_PARSE(0);  // this pattern must terminate with a semicolon
      parse++;
      if (value != NULL) *value = num;  
      EXIT_PARSE(parse);
   }

PARSE_EXIT_CHKPNT(ctrl, inx);
   return exit_code;

PARSE_FATALITY_EXIT(ctrl);
   return exit_code;  
}
//-------------------------------------------------------------------------------
int Parse_Expression(tParseCtrl *ctrl, int inx, tTokenNode **pending) {
   return Parse_Primary(ctrl, inx, pending);
}
//-------------------------------------------------------------------------------
int Parse_Identifier(tParseCtrl *ctrl, int inx, tTokenNode **pending) {
   int byte_count;
   int codept;
   char *buff = ctrl->buff+inx;
   int lim = ctrl->len - inx;
   int i = 0;
   
PARSE_ENTRY_CHKPNT(ctrl);

   byte_count = UTF8_Decode(buff, lim, &codept);
   if (byte_count <= 0) EXIT_PARSE(0);
   if ((codept >= '0') && (codept <= '9')) EXIT_PARSE(0);   //<-- first character cant be a digit 
   while (byte_count > 0) {
      int flag = 0;
      if ((codept >= '0') && (codept <= '9')) flag++;
      if ((codept >= 'a') && (codept <= 'z')) flag++;
      if ((codept >= 'A') && (codept <= 'Z')) flag++;
      if (codept >= '_') flag++;
      if (flag <= 0) EXIT_PARSE(i);

      i++;
      byte_count = UTF8_Decode(buff+i, lim-i, &codept);
   }
   
   exit_code = i;

PARSE_EXIT_CHKPNT(ctrl, inx);

   if ((exit_code > 0) && (pending != NULL)) {
      *pending = CreateNode(ctrl, TKN_Variable, FAM_Unknown);
      (*pending)->p3 = tString_Create(ctrl->buff+inx, i);        
      (*pending)->handler_func = Handler_Variable;
   }

   return exit_code;

PARSE_FATALITY_EXIT(ctrl);
   return exit_code;  
}
//-------------------------------------------------------------------------------
int Parse_LineInput(tParseCtrl *ctrl, int inx) {
   int parse;
   tTokenNode *node = NULL;

PARSE_ENTRY_CHKPNT(ctrl);
    
   //trim off leading whitespaces
   inx += parse = Parse_WhiteSpaces(ctrl, inx);

   parse = Parse_ConsoleCommands(ctrl, inx, &node);
   EXIT_PARSE_ON_MATCH(parse, parse);
   
   parse = Parse_CodeStatement(ctrl, inx, &node);
   EXIT_PARSE_ON_MATCH(parse, parse);

PARSE_EXIT_CHKPNT(ctrl, inx);

   if ((exit_code > 0) && (node != NULL)) {
      AttachNode(ctrl, NULL, node);
   }
   return exit_code;
   
PARSE_FATALITY_EXIT(ctrl);
   
   if (node != NULL) { 
      DestroyTokenNode(node);  
   }
   
   return exit_code;
}
//-------------------------------------------------------------------------------
int Parse_Literal(tParseCtrl *ctrl, int inx, tTokenNode **pending) {
   int parse;
   
PARSE_ENTRY_CHKPNT(ctrl);

   parse = Parse_LiteralIntegral(ctrl, inx, pending);
   EXIT_PARSE_ON_MATCH(parse, parse);
   
//   parse = Parse_LiteralFloat(ctrl, inx, pending);
//   EXIT_PARSE_ON_MATCH(parse, parse);
   
   parse = Parse_LiteralBoolean(ctrl, inx, pending);
   EXIT_PARSE_ON_MATCH(parse, parse);
   
   parse = Parse_LiteralString(ctrl, inx, pending);
   EXIT_PARSE_ON_MATCH(parse, parse);

   parse = Parse_LiteralNull(ctrl, inx, pending);
   EXIT_PARSE_ON_MATCH(parse, parse);
  
PARSE_EXIT_CHKPNT(ctrl, inx);
   return exit_code;

PARSE_FATALITY_EXIT(ctrl);
   
   if (pending != NULL) {
      if (*pending != NULL) {
         DestroyTokenNode(*pending);
         *pending = NULL;
      }
   }
   
   return exit_code;
}
//-------------------------------------------------------------------------------
int Parse_LiteralBoolean(tParseCtrl *ctrl, int inx, tTokenNode **pending) {
   int parse;
   int r_value = -1;

PARSE_ENTRY_CHKPNT(ctrl);

   parse = Parse_MatchPattern(ctrl, inx, "true", 4);
   if (parse > 0) { r_value = 1; EXIT_PARSE(parse); }

   parse = Parse_MatchPattern(ctrl, inx, "false", 5);
   if (parse > 0) { r_value = 0; EXIT_PARSE(parse); }

PARSE_EXIT_CHKPNT(ctrl, inx);

   if ((exit_code > 0) && (pending != NULL)) {
      *pending = CreateNode(ctrl, TKN_Literal, FAM_Boolean);
      (*pending)->r1 = r_value;
   }

   return exit_code;

PARSE_FATALITY_EXIT(ctrl);
   return exit_code;
}
//-------------------------------------------------------------------------------
int Parse_LiteralChar(tParseCtrl *ctrl, int inx, int *value) {
   char *buff = ctrl->buff + inx;
   int lim = ctrl->lim - inx;
   int i = 0;
   int codept;
   int parse;
   
PARSE_ENTRY_CHKPNT(ctrl);

   if (i >= lim) EXIT_PARSE(0);
   if (buff[i] != '\'') EXIT_PARSE(0);
   i++;
   
   parse = UTF8_Decode(buff+i, lim-i, &codept);
   if (parse <= 0) EXIT_PARSE(0);
   if (codept == '\\') {
      inx++;
      parse = Parse_EscSeq(ctrl, inx, &codept);
      if (parse <= 0) EXIT_PARSE(0);
   }
   i += parse;

   if (i >= lim) EXIT_PARSE(0);
   if (buff[i] != '\'') EXIT_PARSE(0);
   i++;
   
   exit_code = i;
   
PARSE_EXIT_CHKPNT(ctrl, inx);
   
   if (value != NULL) *value = codept;

   return exit_code;

PARSE_FATALITY_EXIT(ctrl);
   return exit_code;
}
//-------------------------------------------------------------------------------
int Parse_LiteralDecimal(tParseCtrl *ctrl, int inx, int *value) {
   char *buff = ctrl->buff + inx;
   int lim = ctrl->len - inx;
   int count = 0;
   int digit = 0;
   int num = 0;

PARSE_ENTRY_CHKPNT(ctrl);

   while (count < lim) {
      if ((buff[count] < '0') || (buff[count] > '9')) break;
      digit = buff[count] - '0';
      num = num * 10 + digit;
      count++;
   }

   exit_code = count;

PARSE_EXIT_CHKPNT(ctrl, inx);
   
   if (value != NULL) *value = num;
   
   return exit_code;

PARSE_FATALITY_EXIT(ctrl);
   return exit_code;
}
//-------------------------------------------------------------------------------
int Parse_LiteralIntegral(tParseCtrl *ctrl, int inx, tTokenNode **pending) {
   int parse;
   int value;
   
PARSE_ENTRY_CHKPNT(ctrl);
   
   //*** check for hex must be before decimal because of the leading zero
   parse = Parse_LiteralHex(ctrl, inx, &value);
   EXIT_PARSE_ON_MATCH(parse, parse);

   parse = Parse_LiteralDecimal(ctrl, inx, &value);
   EXIT_PARSE_ON_MATCH(parse, parse);
   
   parse = Parse_LiteralChar(ctrl, inx, &value);
   EXIT_PARSE_ON_MATCH(parse, parse);

PARSE_EXIT_CHKPNT(ctrl, inx);

   if ((exit_code > 0) && (pending != NULL)) {
      *pending = CreateNode(ctrl, TKN_Literal, FAM_Integral);
      (*pending)->r1 = value;
   }
   
   return exit_code;

PARSE_FATALITY_EXIT(ctrl);
   return exit_code;
}
//-------------------------------------------------------------------------------
int Parse_LiteralHex(tParseCtrl *ctrl, int inx, int *value) {
   char *buff = ctrl->buff + inx;
   int lim = ctrl->len - inx;
   int count;
   int digit;
   int num = 0;

PARSE_ENTRY_CHKPNT(ctrl);

   count = Parse_MatchPattern(ctrl, inx, "0x", 2);
   EXIT_PARSE_ON_DISCORD(count, 0);

   while (count < lim) {
      digit = -1;
      if ((buff[count] >= '0') && (buff[count] <= '9')) digit = buff[count] - '0';
      if ((buff[count] >= 'a') && (buff[count] <= 'f')) digit = buff[count] - 'a' + 10;
      if ((buff[count] >= 'A') && (buff[count] <= 'F')) digit = buff[count] - 'A' + 10;
      if (digit < 0) break;
      num = num * 16 + digit;
      count++;
   }
   
   if (count > 2) exit_code = count;

PARSE_EXIT_CHKPNT(ctrl, inx);
   
   if (value != NULL) *value = num;
   
   return exit_code;

PARSE_FATALITY_EXIT(ctrl);
   return exit_code;
}
//-------------------------------------------------------------------------------
int Parse_LiteralNull(tParseCtrl *ctrl, int inx, tTokenNode **pending) {
PARSE_ENTRY_CHKPNT(ctrl);

   exit_code = Parse_MatchPattern(ctrl, inx, "null", 4);

PARSE_EXIT_CHKPNT(ctrl, inx);

   if ((exit_code > 0) && (pending != NULL)) {
      *pending = CreateNode(ctrl, TKN_NULL, FAM_Null);
      (*pending)->r1 = 0;
   }

   return exit_code;

PARSE_FATALITY_EXIT(ctrl);
   return exit_code;
}
//-------------------------------------------------------------------------------
int Parse_LiteralString(tParseCtrl *ctrl, int inx, tTokenNode **pending) {
   int parse;
   int codept;
   int neta = 0;
   int mark = inx;

PARSE_ENTRY_CHKPNT(ctrl);

   parse = Parse_MatchPattern(ctrl, inx, "\"", 1);
   EXIT_PARSE_ON_DISCORD(parse, 0);
   inx += parse;
   neta += parse;
   
   do {
      parse = UTF8_Decode(ctrl->buff + inx, ctrl->lim - inx, &codept);
      if (parse > 0) {
         if (codept == '"') EXIT_PARSE(neta+parse);
         if (codept == '\\') {
            parse = Parse_EscSeq(ctrl, inx, &codept);
            EXIT_PARSE_ON_DISCORD(parse, 0);
         }
         inx += parse;
         neta += parse;
      }
   } while (parse > 0);
 
PARSE_EXIT_CHKPNT(ctrl, inx);

   if ((exit_code > 0) && (pending != NULL)) {
      *pending = CreateNode(ctrl, TKN_Literal, FAM_String);
      (*pending)->r2 = DecodeEscSeq(ctrl->buff+mark+1, exit_code-2);
   }

   return exit_code;

PARSE_FATALITY_EXIT(ctrl);
   return exit_code;
}
//-------------------------------------------------------------------------------
int Parse_MatchPattern(tParseCtrl *ctrl, int inx, char *match, int match_lim) {
   assert(ctrl != NULL);
   assert(match != NULL);

   if (ctrl->len - inx < match_lim) return 0;    // buffer is too small to hold pattern

   char *buff = ctrl->buff + inx;

   for (int i = 0; i < match_lim; i++) {
      if (*buff != *match) return 0;   // not a match
      buff++;
      match++;
   }

   return match_lim;
}
//-------------------------------------------------------------------------------
int Prase_PostfixExpression(tParseCtrl *ctrl, tTokenNode *parent) {
// <postfix expression> ::= <primary> | <identifier> | <postfix expression> "++" | <postfix expression> "--"
   return 0;
}
//-------------------------------------------------------------------------------
int Parse_Primary(tParseCtrl *ctrl, int inx, tTokenNode **pending) {
   int parse;

PARSE_ENTRY_CHKPNT(ctrl);

   parse = Parse_Literal(ctrl, inx, pending);
   EXIT_PARSE_ON_MATCH(parse, parse);   

   parse = Parse_Identifier(ctrl, inx, pending);
   EXIT_PARSE_ON_MATCH(parse, parse);   

PARSE_EXIT_CHKPNT(ctrl, inx);  

   if ((exit_code > 0) && (pending != NULL)) {
      if ((*pending) != NULL) {
         if ((*pending)->token == TKN_Variable) {
            tSymTblEntry *entry = tSymTbl_GetEntry((*pending)->ctrl->vars, (*pending)->p3);
            if (entry == NULL) FATALITY(RULE_1001);  // unknown identifer
            if ((entry->flags && tSymTbl_VAR_INITIALIZED) == 0) FATALITY(RULE_1003); // uninitialized variable

            (*pending)->family = entry->family;
         }
      }
   }
   return exit_code;

PARSE_FATALITY_EXIT(ctrl);
   
   if (pending != NULL) {
      if (*pending != NULL) {
         DestroyTokenNode(*pending);
         *pending = NULL;
      }
   }

   return exit_code;  
}
//-------------------------------------------------------------------------------
int Parse_PrimaryArray(tParseCtrl *ctrl, int inx, tTokenNode *parent) {
   return 0;
}
//-------------------------------------------------------------------------------
int Parse_PrimaryBasic(tParseCtrl *ctrl, int inx, tTokenNode *parent) {
//<simple primary> ::= <literal> | this | "(" <expression> ")" | <class instant creation> | <field access> | <method invocation> | <array access>
   int parse;
/*
   parse = Parse_Literal(ctrl, parent);
   if (parse > 0) return CleanExit(parse);
*/
/*
   neta = 0;
   parse = Parse_MatchPattern(ctrl, "(", 1);
   if (parse > 0) {
      ctrl->inx += parse;
      neta += parse;
      
      parse = Parse_Expression(ctrl);
      if (parse > 0) {
         ctrl->inx == parse;
         neta += parse;
         
         parse = Parse_MatchPattern(ctrl, ")", 1);
         if (parse > 0) {
            return CleanExit (parse + neta);
         }
      }
   }
   ctrl->inx = mark;
*/

/*
   function call
*/
/*
  neta = 0;
  parse = Parse_MatchPattern(ctrl, "!", 1);
  if (parse > 0) {
     ctrl->inx += parse;
     neta += parse;

     parse = Parse_WhiteSpaces(ctrl);   // tolerate whitespaces in between unary operators
     ctrl->inx += parse;
     neta += parse;

     if (ctrl->mode == _opt_CONSTRUCT) {
        not_operator = CreateNode(TKN_Operator, FAM_Boolean);
        not_operator->handler_func = Handler_Operator;
        not_operator->opcode = OPR_NOT;
     }

     parse = Parse_Factor(ctrl, not_operator);
     if (parse > 0) {
        return CleanExit(parse + neta);
     }
  }
*/
  
//  return CleanExit(0);
  return 0; 
}
//-------------------------------------------------------------------------------
int Parse_SavageType(tParseCtrl *ctrl, int inx, int *type, int *family) {
   char *type_names[] = {"void",    "boolean",    "byte",       "char",       "short",      "int",        "long",       "string"};
   int   type_index[] = {TYPE_void, TYPE_boolean, TYPE_byte,    TYPE_char,    TYPE_short,   TYPE_int,     TYPE_long,    TYPE_string};
   int family_index[] = {FAM_Void,  FAM_Boolean,  FAM_Integral, FAM_Integral, FAM_Integral, FAM_Integral, FAM_Integral, FAM_String};
   int parse;
   
   for (int i = 0; i < TYPE_INVALID; i++) {
      parse = Parse_MatchPattern(ctrl, inx, type_names[i], strlen(type_names[i]));
      if (parse > 0) {
         if (type != NULL) *type = type_index[i];
         if (family != NULL) *family = family_index[i];
         return parse;
      }
   }
}
//-------------------------------------------------------------------------------
int Parse_UnaryExpression(tParseCtrl *ctrl, tTokenNode *parent) {
// <unary expression> := "++" <unary expression> | "--" <unary expression> | "-" <unary expression> | <postfix expression> | "~" <unary expression> | "!" <unary expression> | <cast expression>
}
//-------------------------------------------------------------------------------
int Parse_VariableDeclaration(tParseCtrl *ctrl, int inx, tTokenNode **pending) {
   int type_id;
   int family_id;
   int parse;
   int neta = 0;
   char *ident_ptr = NULL;
   char ident_len = 0;
   tTokenNode *expression = NULL;

PARSE_ENTRY_CHKPNT(ctrl);

   parse = Parse_SavageType(ctrl, inx, &type_id, &family_id);
   EXIT_PARSE_ON_DISCORD(parse, 0);
   inx += parse;
   neta += parse;

   parse = Parse_WhiteSpaces(ctrl, inx);
   inx += parse;
   neta += parse;
   
   parse = Parse_Identifier(ctrl, inx, NULL);
   EXIT_PARSE_ON_DISCORD(parse, 0);
   ident_ptr = ctrl->buff + inx;
   ident_len = parse;
   inx += parse;
   neta += parse;

   parse = Parse_WhiteSpaces(ctrl, inx);
   neta += parse;
   inx += parse;
  
   parse = Parse_MatchPattern(ctrl, inx, ":", 1);
   EXIT_PARSE_ON_DISCORD(parse, neta);
   neta += parse;
   inx += parse;

   parse = Parse_WhiteSpaces(ctrl, inx);
   neta += parse;
   inx += parse;
   
   parse = Parse_Expression(ctrl, inx, &expression);
   EXIT_PARSE_ON_DISCORD(parse, 0);
   neta += parse;

   exit_code = neta;
   
PARSE_EXIT_CHKPNT(ctrl, inx);

   if ((exit_code > 0) && (pending != NULL)) {
   
      tSymTblEntry *entry = tSymTbl_GetEntry_Using_CharPtr(ctrl->vars, ident_ptr, ident_len);
      if (entry != NULL) FATALITY(RULE_1002);  // duplicate identifier
      
      if (expression != NULL) {
         if (expression->family != family_id) FATALITY(RULE_1004);  // type mismatch
      }

      // register identifer
      entry = tSymTbl_AddEntry_Using_CharPtr(ctrl->vars, type_id, family_id, ident_ptr, ident_len);
      
      if (expression != NULL) {
         *pending = CreateNode(ctrl, TKN_Initialize, family_id);
         (*pending)->p1 = type_id;
         (*pending)->p3 = tString_Create(ident_ptr, ident_len);
         (*pending)->handler_func = Handler_VariableInitialize;
         AttachNode(ctrl, *pending, expression);        
         entry->flags |= tSymTbl_VAR_INITIALIZED;
      }
   }
   return exit_code;
   
PARSE_FATALITY_EXIT(ctrl);
   if (expression != NULL) {
      DestroyTokenNode(expression);
   }
   return exit_code;
}
//-------------------------------------------------------------------------------
int Parse_WhiteSpaces(tParseCtrl *ctrl, int inx) {
   int count = 0;
   int lim = ctrl->len - inx;
   char *buff = ctrl->buff + inx;
   
   while (count < lim) {
      if (buff[count] > ' ') break;
      count++;
   }
   
   return count;
}
//-------------------------------------------------------------------------------

