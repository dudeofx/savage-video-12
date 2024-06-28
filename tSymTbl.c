#include <stdlib.h>
#include <stdio.h>

#include "tString.h"
#include "tSymTbl.h"

//-------------------------------------------------------------------------------
tSymTbl *tSymTbl_Create(void *owner) {
   tSymTbl *tmp = malloc(sizeof(tSymTbl));
   tmp->root = NULL;
   tmp->parent = NULL;
}
//-------------------------------------------------------------------------------
tSymTblEntry *tSymTbl_AddEntry(tSymTbl *tbl, int type, int family, tString *ident) {
   tSymTblEntry *tmp;
   
   if (ident == NULL) return NULL; 
   return tSymTbl_AddEntry_Using_CharPtr(tbl, type, family, ident->data, ident->len);
}
//-------------------------------------------------------------------------------
tSymTblEntry *tSymTbl_AddEntry_Using_CharPtr(tSymTbl *tbl, int type, int family, char *ident_ptr, int ident_len) {
   tSymTblEntry *tmp;
   
   if (ident_ptr == NULL) return NULL; 
   if (ident_len <= 0) return NULL;
   
   tString *ident = tString_Create(ident_ptr, ident_len);

   tSymTblEntry *item = malloc(sizeof(tSymTblEntry));
   item->type = type;
   item->flags = 0;
   item->family = family;
   item->ident = ident;
   item->value = 0;
   item->data = NULL;
   item->left = NULL;
   item->right = NULL;

   if (tbl->root == NULL) { 
      tbl->root = item;
      return item;
   } else {
      tmp = tbl->root;
      while (1) {
         int tellme = tString_Compare(ident, tmp->ident);
         if (tellme < 0) {
            if (tmp->left == NULL) {
               tmp->left = item;
               return item;
            }
            tmp = tmp->left;
         } else {
            if (tmp->right == NULL) {
               tmp->right = item;
               return item;
            }
            tmp = tmp->right;
         }
      }
   }
   
   return NULL;
}
//-------------------------------------------------------------------------------
void tSymTbl_TraverseDestroy(tSymTblEntry *entry) {
   if (entry == NULL) return;
   tSymTbl_TraverseDestroy(entry->left);
   tSymTbl_TraverseDestroy(entry->right);
   free(entry->ident);
   if (entry->data != NULL) free(entry->data);
   free(entry);
}
//-------------------------------------------------------------------------------
void tSymTbl_Destroy(tSymTbl *tbl) {
   assert(tbl != NULL);
   tSymTbl_TraverseDestroy(tbl->root);
}
//-------------------------------------------------------------------------------
tSymTblEntry *tSymTbl_GetEntry(tSymTbl *tbl, tString *ident) {
   if (ident == NULL) return NULL;
   return tSymTbl_GetEntry_Using_CharPtr(tbl, ident->data, ident->len);
}
//-------------------------------------------------------------------------------
tSymTblEntry *tSymTbl_GetEntry_Using_CharPtr(tSymTbl *tbl, char *ident_ptr, int ident_len) {
   int verdict = 0;
   
   if (ident_ptr == NULL) return NULL;
   if (ident_len <= 0) return NULL;
   if (tbl == NULL) return NULL;

   tSymTblEntry *entry = tbl->root;

   do {
      if (entry == NULL) return NULL;
      verdict = tString_Compare_With_CharPtr(entry->ident, ident_ptr, ident_len);
      if (verdict > 0) entry = entry->left;
      if (verdict < 0) entry = entry->right;
   } while (verdict != 0);
   return entry;  
}
//-------------------------------------------------------------------------------

