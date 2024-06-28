#ifndef __tSymTbl_h__
#define __tSymTbl_h__

#include "tString.h"

#define tSymTbl_VAR_INITIALIZED     1

typedef struct _tSymTblEntry tSymTblEntry;
struct _tSymTblEntry {
   int           type;
   int           flags;
   int           family;
   tString      *ident;
   int           value;
   tString      *data;
   tSymTblEntry *left;
   tSymTblEntry *right;
};

typedef struct _tSymTbl tSymTbl;
struct _tSymTbl {
   tSymTblEntry *root;
   tSymTbl *parent;
};

tSymTbl      *tSymTbl_Create(void *owner);
void          tSymTbl_Destroy(tSymTbl *tbl);

tSymTblEntry *tSymTbl_AddEntry(tSymTbl *tbl, int type, int family, tString *ident);
tSymTblEntry *tSymTbl_AddEntry_Using_CharPtr(tSymTbl *tbl, int type, int family, char *ident_ptr, int ident_len);

tSymTblEntry *tSymTbl_GetEntry(tSymTbl *tbl, tString *ident);
tSymTblEntry *tSymTbl_GetEntry_Using_CharPtr(tSymTbl *tbl, char *ident_ptr, int ident_len);

#endif


