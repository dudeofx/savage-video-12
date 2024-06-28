#include <assert.h>
#include <stddef.h>

#include "UTF8_Utils.h"

static const unsigned char _byte_count[] = {
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x00 .. 0x0F
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x10 .. 0x1F
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x20 .. 0x2F
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x30 .. 0x3F
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x40 .. 0x4F
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x50 .. 0x5F
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x60 .. 0x6F
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x70 .. 0x7F

   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x80 .. 0x8F // error
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0x90 .. 0x9F // error
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0xA0 .. 0xAF // error
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0xB0 .. 0xBF // error
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 0xC0 .. 0xCF // 
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 0xD0 .. 0xDF // 
   3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  // 0xE0 .. 0xEF // 
   4, 4, 4, 4, 4, 4, 4, 4,                          // 0xF0 .. 0xF7 // 
   5, 5, 5, 5, 6, 6, 0, 0,                          // 0xF7 .. 0xFF // 
};

static const unsigned int _lower_limit[] = {
   0xFFFFFFFF, // invalid should never happen
   0x00000000,
   0x00000080,
   0x00000800,
   0x00010000,
   0x00200000,
   0x04000000,
};

static const unsigned char _mask[] = {
   0x00, // invalid should never happen
   0x7F,
   0x1F,
   0x0F,
   0x07,
   0x03,
   0x01,
};

//-----------------------------------------------------------------------------------------
int UTF8_Decode(char *buff, int buff_lim, int *code_point) {
   assert(code_point != NULL);
   *code_point = 0;

   if (buff_lim == 0) return 0; // error: buffer overflow

   int byte_count = 0; 
   int byte_size = 0;
   
   byte_size = byte_count = _byte_count[(unsigned char) *buff];
   
   if (byte_count == 0) return 0; // error: orphaned continuation byte 

   if (buff_lim < byte_size) return 0; // error: buffer truncated

   *code_point = (*(buff++) & _mask[byte_size]);
   byte_count--;
   if (byte_count == 0) return byte_size;
   buff_lim--;
   while (buff_lim > 0) {
      if ((0xC0 & *buff) != 0x80) return 0; // error: expected a continuation byte
      *code_point = ((*code_point << 6) | (0x3F & *buff)) ;
      byte_count--;
      if (byte_count == 0) {
         if (*code_point < _lower_limit[byte_size]) return 0; // overlong check just curtisy
         return byte_size;
      }
      buff++;
      buff_lim--;
   };
   
   return 0; // if it gets here it means data is truncated
}
//-----------------------------------------------------------------------------------------
// dst is a buffer where uft8 sequence is stored
// dst buffer must be at least 5 bytes long. No checking is done. a null byte is appended at the end of sequence.
// function returns the number of bytes of utf8 sequence excluding the null.
//
// set dst = NULL to simply get the bytes the encoded sequence would take up;
int UTF8_Encode(int code_point, char *dst) {

  if (code_point < 0) {
     if (dst != NULL) dst[0] = '\0';
     return 0;
  }
  
  if (code_point < 0x80) {
      if (dst != NULL) {
         dst[0] = code_point;
         dst[1] = '\0';
      }
      return 1;
   } 
   if (code_point < 0x0800) {
      if (dst != NULL) {
         dst[0] = ((code_point >> 6)  & 0x1F) | 0xC0;
         dst[1] = (code_point & 0x3F) | 0x80;
         dst[2] = '\0';
      }
      return 2;
   }
   if (code_point < 0x010000) {
      if (dst != NULL) {
         dst[0] = ((code_point >> 12) & 0x0F) | 0xE0;
         dst[1] = ((code_point >> 6)  & 0x3F) | 0x80;
         dst[2] = (code_point & 0x3F) | 0x80;
         dst[3] = '\0';
      }
      return 3;
   }
   if (code_point < 0x110000) {
      if (dst != NULL) {
         dst[0] = ((code_point >> 18) & 0x07) | 0xF0;
         dst[1] = ((code_point >> 12) & 0x3F) | 0x80;
         dst[2] = ((code_point >> 6)  & 0x3F) | 0x80;
         dst[3] = (code_point & 0x3F) | 0x80;
         dst[4] = '\0';
      }
      return 4;
   }

   if (dst != NULL) dst[0] = '\0';
   return 0;
}
//-----------------------------------------------------------------------------------------

