// tStrings are required to be null terminated as well as maintain their length variable

#ifndef __tString_h__
#define __tString_h__

#include <string.h>
#include <assert.h>

typedef struct {
   int len;
   char data[4];
} tString;

tString *tString_Create(void *st, int len);
tString *tString_Create_FromCString(char *st);
tString *tString_Create_RawBuff(int len);
tString *tString_Copy(tString *st);

int      tString_Compare(tString *st1, tString *st2);
int      tString_Compare_With_CharPtr(tString *st, char *ptr, int len);


#endif

