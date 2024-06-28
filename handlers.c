#include "main.h"

//-------------------------------------------------------------------------------
int Handler_ConsoleExit(tTokenNode *node) {
   exit_flag = 1;
   return 0;
}
//-------------------------------------------------------------------------------
int Handler_ConsoleHelp(tTokenNode *node) {
   printf("-------------------------------------------------------------------------------\n");
   printf("!exit                  - exit the interpreter\n");
   printf("!reset                 - clears the interpreter enviroment\n");
//   printf("!ls_mem                - list raw memory allocations\n");
   printf("!ls_vars               - list global variables\n");
//   printf("!ls_usrlib             - list user library\n");
//   printf("!ls_syslib             - list system library\n");
//   printf("!import <filename>     - imports library from disk\n");
//   printf("!run <filename>        - load and execute code from disk\n");
   printf("-------------------------------------------------------------------------------\n");
   printf("\n");

   return 0;
}
//-------------------------------------------------------------------------------
int Handler_ConsoleListVars(tTokenNode *node) {
   //............................................................................
   void traverse(tSymTblEntry *vars) {
      if (vars == NULL) return;
      traverse(vars->left);
      printf("   %s %.*s\n", StrType(vars->type), vars->ident->len, vars->ident->data);
      traverse(vars->right);
   }
   //............................................................................
   if (node->ctrl->vars == NULL) {
      printf("no globals defined\n");
   } else {
      printf("\n");
      traverse(node->ctrl->vars->root);
      printf("\n");
   }

   return 0;
}
//-------------------------------------------------------------------------------
int Handler_ConsoleListMem(tTokenNode *node) {
   printf("Memory Dump\n");
//   tMemHeap_Report();
   return 0;
}
//-------------------------------------------------------------------------------
int Handler_DummyNodeHandler(tTokenNode *node) { return 0;}
//-------------------------------------------------------------------------------
int Handler_Operator(tTokenNode *node) {

   switch (node->opcode) {
      case OPR_ASSIGN: {
         tTokenNode *expression = node->eldest;
         assert(expression != NULL);
        
         tSymTblEntry *var = tSymTbl_GetEntry(node->ctrl->vars, node->p3);
         assert(var != NULL);

         int  err = expression->handler_func(expression);
         if (err < 0) return -1; // runtime error (not there yet)
                 
         var->value = expression->r1;
         var->data = expression->r2;
         
         node->family = var->family;
         node->r1 = var->value;
         node->r2 = var->data;
         node->r_flags = var->flags;

         return 0;
      } 
      case OPR_NOT:
         node->eldest->handler_func(node->eldest);
         node->r1 = !(node->eldest->r1);
         break;
   }

   return 0;
}
//-------------------------------------------------------------------------------
int Handler_Variable(tTokenNode *node) {
   tSymTblEntry *var = tSymTbl_GetEntry(node->ctrl->vars, node->p3);
   assert(var != NULL);

   node->family = var->family;
   node->r1 = var->value;
   node->r_flags = var->flags;
   if (var->data != NULL) {
      node->r2 = var->data;
   }

   return 0;
}
//-------------------------------------------------------------------------------
int Handler_VariableInitialize(tTokenNode *node) {
   int code;
  
   tSymTblEntry *entry = tSymTbl_AddEntry(node->ctrl->vars, node->p1, node->family, node->p3);

   if (node->eldest != NULL) {

      entry->value = node->eldest->r1;
      node->r1 = entry->value;

      if (node->eldest->r2 != NULL) {
         entry->data = node->eldest->r2;
         node->eldest->r2 = NULL;
         node->r2 = entry->data;
      }
      node->r_flags = entry->flags;
   }

   return 0;
};
//-------------------------------------------------------------------------------

