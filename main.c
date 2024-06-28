#include "main.h"

char *__str_type[TYPE_INVALID+1] = {"void", "boolean", "byte", "char", "short", "int", "long", "string", "<invalid>" };
char input_buffer[MAX_INPUT] = "\0";
int  exit_flag = 0;

//-------------------------------------------------------------------------------
//===[utilities that deal with input]============================================
//-------------------------------------------------------------------------------
void FlushInput() {
      for (int c = 0; (c != '\n') && (c != EOF); c = getchar()); 
}
//-------------------------------------------------------------------------------
int GetInput(tParseCtrl *ctrl) {
    int ch;
    int count = 0;
    
    assert(ctrl != NULL);
       
    for(;;) {
        ch = fgetc(stdin);
        if (ch == EOF) return count;
        if (ch == '\n') return count;
        ctrl->buff[ctrl->len++] = ch;
        if (ctrl->len >= MAX_INPUT) {
           FlushInput();
           printf("\nERROR: Input flushed because its too big. The %i buffer was not enought for it.\n       Consider using the \"!run\" command to execute large programs.\n", MAX_INPUT);
           continue;
        }


    }
    
   return count;
}
//-------------------------------------------------------------------------------
void ResetCtrl(tParseCtrl *ctrl) {
   *(ctrl->buff) = '\0';
   ctrl->len = 0;
   ctrl->reach = 0;
   DestroyTokenNode(ctrl->root);
   ctrl->root = NULL;
}
//-------------------------------------------------------------------------------
//===[utilities that deal with node management]==================================
//-------------------------------------------------------------------------------
void AttachNode(tParseCtrl *ctrl, tTokenNode *parent, tTokenNode *node) {

   if (node == NULL) return;
   
   if (parent == NULL) {
      assert(ctrl->root == NULL);
      ctrl->root = node;
      return;
   }
   
   tTokenNode *child = parent->eldest;
   if (child == NULL) {
      parent->eldest = node;
      return;
   }
   while (child->sibling != NULL) child = child->sibling;
   child->sibling = node;   

}
//-------------------------------------------------------------------------------
tTokenNode *CreateNode(tParseCtrl *ctrl, int token, int family) {
   tTokenNode *node = (tTokenNode *) malloc(sizeof(tTokenNode));
   node->ctrl = ctrl;
   node->token = token;
   node->family = family;
   node->handler_func = Handler_DummyNodeHandler;
   node->opcode = OPR_NOP;
   node->p1 = 0;
   node->p2 = 0;
   node->p3 = NULL;
   node->r_flags = 0;
   node->r1 = 0;
   node->r2 = NULL;
   node->parent = NULL;
   node->sibling = NULL;
   node->eldest = NULL;
   
   return node;
}
//-------------------------------------------------------------------------------
void DestroyTokenNode(tTokenNode *node) {
   if (node == NULL) return;
   
   tTokenNode *child = node->eldest;
   while (child != NULL) {
      tTokenNode *mark = child;
      child = child->sibling;
      DestroyTokenNode(mark);
   } 
   if (node->p3 != NULL) free(node->p3);
   free(node);
}
//-------------------------------------------------------------------------------
void ExecParseTree(tTokenNode *root) {
   if (root != NULL) {
      root->handler_func(root);
      switch (root->family) {
         case FAM_Boolean:
            printf("(boolean) \n");
            if ((root->r_flags & tSymTbl_VAR_INITIALIZED) == 0) { 
               printf(" ??? \n\n");
            } else {
               switch (root->r1) {
                  case 0: printf("false\n\n"); break;
                  case 1: printf("true\n\n"); break;
               }
            }
            break;
         case FAM_String:
            printf("(string)\n");
            if ((root->r_flags & tSymTbl_VAR_INITIALIZED) == 0) { 
               printf(" ??? \n\n");
            } else {
               if (root->r2 == NULL) {
                  printf("(null)\n\n");
               } else {
                  printf("%s\n\n", root->r2->data);
               }
            }
            break;
         case FAM_Integral:
            if ((root->r_flags & tSymTbl_VAR_INITIALIZED) == 0) { 
               printf("(integral) \n ??? \n\n");
            } else {
               printf("(integral) \n%i\n\n", root->r1);
            }
            break;
         case FAM_Null:
            printf("(null)\n\n");
            break;
      }
   }
}
//-------------------------------------------------------------------------------
//===[Misc]======================================================================
//-------------------------------------------------------------------------------
// creates a new string "dst" with all escape sequences removed
tString *DecodeEscSeq(char *src, int src_len) {
   int ofs;
   int parse;
   int codept;
   int byte_count;
   int neta;
   tString *dst;

   tParseCtrl ctrl = {
      .buff = src,
      .len = src_len,
      .lim = src_len,
      .reach = 0,
      .vars = NULL,
      .root = NULL,
   };


   // count bytes that new string will take
   ofs = 0;
   neta = 0;
   while (ofs < src_len) {
      byte_count = UTF8_Decode(src+ofs, src_len-ofs, &codept);   
      if (byte_count <= 0) return NULL;  // should never happen on a valid "src" string
      if (codept == '&') {
         byte_count = Parse_EscSeq(&ctrl, ofs, &codept);
         neta += UTF8_Encode(codept, NULL);
      } else {
         neta += byte_count;
      }
      ofs += byte_count;
   }
   
   dst = tString_Create_RawBuff(neta);
   
   // render new string;
   ofs = 0;
   neta = 0;
   while (ofs < src_len) {
      byte_count = UTF8_Decode(src+ofs, src_len-ofs, &codept);   
      if (byte_count <= 0) return NULL;  // should never happen on a valid "src" string
      if (codept == '\\') {
         byte_count = Parse_EscSeq(&ctrl, ofs, &codept);
         neta += UTF8_Encode(codept, dst->data+neta);
      } else {
         memcpy(dst->data+neta, src+ofs, byte_count);
         neta += byte_count;
      }
      ofs += byte_count;
   }
   
   return dst;
}
//-------------------------------------------------------------------------------
char *StrType(int type) {
  if (type < 0) return __str_type[TYPE_INVALID];
  if (type >= TYPE_INVALID) return __str_type[TYPE_INVALID];
  return __str_type[type];
}
//-------------------------------------------------------------------------------
void PrintError(tParseCtrl *ctrl, int code) {
      
   printf("Error %i: col %i: ", code, ctrl->reach);

   switch (code) {
      case 0:
         printf("General Syntax!\n");
         return;
      case RULE_1001:
         printf("Unknown Identifier!\n");
         return;
      case RULE_1002:
         printf("Duplicated Identifier!\n");
         return;
      case RULE_1003:
         printf("Uninitialized Variable!\n");
         return;
      case RULE_1004:
         printf("Type mismatch!\n");
         return;
   }

   if (code < ctrl->len) { 
      printf("General Syntax!\n");
      return;
   }

   printf("Undocumented Fatality!\n");
   return;
}
//===============================================================================
void main() {
   int code;
   tParseCtrl ctrl = {
      .buff = input_buffer,
      .len = 0,
      .lim = MAX_INPUT-1,
      .reach = 0,
      .vars = NULL,
      .root = NULL,
   };


   ctrl.vars = tSymTbl_Create(&ctrl);
   printf("-------------------------------------------------------------------------------\n");
   printf("Welcome to Victor's experimental interpreter. 🙂\n");
   printf("Enter \"!exit\" to get back to the shell.\n");
   printf("Enter \"!help\" to get a list of helpfull commands.\n");
   printf("-------------------------------------------------------------------------------\n");
   printf("\n");
   do {
      ResetCtrl(&ctrl);
      printf(">: ");
      GetInput(&ctrl);

      code = Parse_LineInput(&ctrl, 0);
      if ((code <= 0) || (code < ctrl.len)) { 
         PrintError(&ctrl, code); 
         continue; 
      }

      ExecParseTree(ctrl.root);
   } while (!exit_flag);
   printf("-------------------------------------------------------------------------------\n");
   printf("Thank you for trying it out.\n\n");
}
//-------------------------------------------------------------------------------

