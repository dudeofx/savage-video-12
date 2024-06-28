#include <stdlib.h>

#include "tString.h"

//-------------------------------------------------------------------------------
tString *tString_Create(void *st, int len) {
   assert(len >= 0);
   assert(st != NULL);

   tString *tmp = tString_Create_RawBuff(len);
   memcpy(tmp->data, st, len);
    
   return tmp;
}
//-------------------------------------------------------------------------------
// provided soley to make code look cleaner
tString *tString_Create_FromCString(char *st) {
   assert(st != NULL);
   return tString_Create(st, strlen(st));
}
//-------------------------------------------------------------------------------
// allocates memory for a tString but data is not initialized
tString *tString_Create_RawBuff(int len) {
   tString *tmp;
    
   assert(len >= 0);

   int neta = sizeof(int) + len+1;
   if (neta < sizeof(tString)) neta = sizeof(tString);
    
   tmp = malloc(neta);
   tmp->len = len;
   tmp->data[len] = '\0';
    
   return tmp;
}
//-------------------------------------------------------------------------------
int tString_Compare(tString *st1, tString *st2) {
   assert(st1 != NULL);
   assert(st2 != NULL);
   
   int delta = st1->len - st2->len;
   int len;
   if (delta < 0) {
      len = st1->len;
   } else {
      len = st2->len;
   }
   int verdict = memcmp(st1->data, st2->data, len);
   if (verdict == 0) return delta;
   return verdict;
}
//-------------------------------------------------------------------------------
int tString_Compare_With_CharPtr(tString *st, char *ptr, int len) {
   assert(st != NULL);
   assert(ptr != NULL);
   
   int delta = st->len - len;
   if (delta < 0) len = st->len;

   int verdict = memcmp(st->data, ptr, len);
   if (verdict == 0) return delta;
   return verdict;
}
//-------------------------------------------------------------------------------
tString *tString_Copy(tString *st) {
   if (st == NULL) return NULL;
   return tString_Create(st->data, st->len);
}
//-------------------------------------------------------------------------------

