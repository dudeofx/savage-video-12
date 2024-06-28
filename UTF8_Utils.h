#ifndef __tUTF8_Utils_h__
#define __tUTF8_Utils_h__

int UTF8_Decode(char *buff, int buff_lim, int *code_point);
int UTF8_Encode(int code_point, char *dst);

#endif

