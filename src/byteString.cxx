//------------------------------------------------------------------------------
//--  Copyright (c) 2004,2005,2006 by Eric Fahlgren                           --
//--  All Rights Reserved.                                                    --
//--                                                                          --
//--  This program is free software; you can redistribute it and/or           --
//--  modify it under the terms of the GNU General Public License             --
//--  as published by the Free Software Foundation; either version            --
//--  2 of the License, or (at your option) any later version.                --
//--  See http://www.gnu.org/licenses/gpl.txt                                 --
//------------------------------------------------------------------------------

#include "stdafx.h"
//#include <windows.h>
//#include <stdio.h>
#include "byteString.h"
#include "megatune.h"

//==============================================================================

byteString::byteString(const char *s, int l)
 : _byt(NULL),
   _siz( 0),
   _len(-1)
{
   if (s != NULL && l == -1) l = strlen(s);
   copy(reinterpret_cast<const BYTE *>(s), l);
}

byteString::byteString(const BYTE *s, int l) : _byt (NULL), _siz(0), _len(-1) { copy(s, l); }
byteString::byteString(const byteString &d)  : _byt (NULL), _siz(0), _len(-1) { copy(d); } 
byteString::byteString(BYTE  b)              : _byt (NULL), _siz(0), _len(-1) { copy(reinterpret_cast<BYTE *>(&b), sizeof(b)); }
byteString::byteString(WORD  w)              : _byt (NULL), _siz(0), _len(-1) { copy(reinterpret_cast<BYTE *>(&w), sizeof(w)); }
byteString::byteString(DWORD d)              : _byt (NULL), _siz(0), _len(-1) { copy(reinterpret_cast<BYTE *>(&d), sizeof(d)); }

//------------------------------------------------------------------------------

void byteString::resize(int newLen)
{
   if (newLen >= _siz) {
      if (_len == -1) _len = 0;
      BYTE *newBytes = new BYTE[newLen+1];
      if (_byt) {
         memcpy(newBytes, _byt, _len);
         delete [] _byt;
      }
      memset(newBytes+_len, '\0', newLen-_len+1);
      _byt = newBytes;
      _siz = newLen+1;
   }
   _len = newLen;
}

//------------------------------------------------------------------------------

byteString &byteString::release()
{
   if (_byt) delete[]_byt;
   _byt = NULL;
   _siz = 0;
   _len = -1;
   return *this;
}

//------------------------------------------------------------------------------

void byteString::copy(const BYTE *s, int len)
{
   if (s == NULL && len == -1) return;
   _siz = len+1;
   _byt = new BYTE[_siz];
   _len = len;
   if (s)
      memcpy(_byt, s, len);
   else
      memset(_byt, '\0', len);
   _byt[_len] = '\0';
}

byteString &byteString::copy(const byteString &s)
{
   copy(s._byt, s._len);
   return *this;
}

//------------------------------------------------------------------------------

byteString &byteString::operator = (const byteString &rhs)
{
   if (_byt) delete [] _byt; // This may need better initialization of "this."
   return copy(rhs);
}

//------------------------------------------------------------------------------

int byteString::cmp(const byteString &rhs) const
{
   if (_byt == NULL || _len <= 0) {
      return rhs._byt == NULL ? 0 : rhs._len > 0;
   }
   if (rhs._byt == NULL || rhs._len <= 0) return -1;
   for (int i = 0; i < _len; i++) {
      if (i >= rhs._len) return -1;
      if (_byt[i] != rhs._byt[i]) return rhs._byt[i]-_byt[i];
   }
   return 0;
}

//------------------------------------------------------------------------------

byteString &byteString::append(const BYTE *s, int len)
{
   if (s && len > 0) {
      if (_len < 0) _len = 0;
      BYTE *byt = new BYTE[_len+len+1];
      if (_byt) {
         memcpy(byt, _byt, _len);
         delete [] _byt;
      }
      memcpy(byt+_len, s, len);
      _len += len;
      _byt  = byt;
   }

   return *this;
}

byteString &byteString::append(const byteString &s)
{
   return append(s._byt, s._len);
}

byteString &byteString::append(BYTE c)
{
   return append(&c, 1);
}

//------------------------------------------------------------------------------

byteString &byteString::replace(const char *from, const char *to)
{
   size_t fromLen = strlen(from);
   size_t toLen   = strlen(to);

   for (size_t i = 0; i < (_len-fromLen); i++) {
      if (memcmp(_byt+i, from, fromLen) == 0) {
         if (toLen != fromLen) {
            if (toLen > fromLen) resize(_len+toLen-fromLen); // Make it bigger
            memmove(_byt+i+toLen, _byt+i+fromLen, _len-i-fromLen);
            _len += toLen-fromLen;
         }
         memcpy(_byt+i, to, toLen);
         i += toLen-1;
      }
   }

   _byt[_len] = 0;

   return *this;
}

//------------------------------------------------------------------------------
// Convert standard C character constants escape sequences into characters.
//    \' \" \? \\ \a \b \f \n \r \t \v
//    \nnn where nnn is three octal digits.
//    \xnn where nn is two hexadecimal digits.

byteString &byteString::xlate()
{
   static char *_octal = "01234567";
   static char *_hexal = "0123456789abcdefABCDEF";

   BYTE bb;
   for (int i = 0; i < _len; i++) {
      if (_byt[i] == '\\') {
         switch (_byt[i+1]) {
            case '\'': bb = BYTE('\''); goto move;
            case '"' : bb = BYTE('"');  goto move;
            case '?' : bb = BYTE('\?'); goto move;
            case '\\': bb = BYTE('\\'); goto move;
            case 'a' : bb = BYTE('\a'); goto move;
            case 'b' : bb = BYTE('\b'); goto move;
            case 'f' : bb = BYTE('\f'); goto move;
            case 'n' : bb = BYTE('\n'); goto move;
            case 'r' : bb = BYTE('\r'); goto move;
            case 't' : bb = BYTE('\t'); goto move;
            case 'v' : bb = BYTE('\v'); goto move;
            break;
            move:
               _byt[i] = bb;
               memmove(_byt+i+1, _byt+i+2, _len-(i+1));
               _len--;
               break;

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case 'x': { // Convert \001 and \xf1 escapees.
               int   base  = 8; // Assume it's octal.
               int   iVal  = 1;
               char *valid = _octal;
               int   iMax  = 3; // Only three max allowed.

               if (_byt[i+iVal] == 'x' || _byt[i+iVal] == 'X') {
                  base     = 16;
                  iVal     = 2; // Skip the 'x'
                  valid    = _hexal;
                  iMax     = 2;
               }

               char num[10]; // Should be good enough.
               int iNum = 0;
               while (strchr(valid, _byt[i+iVal]) != 0 && iNum < iMax) {
                  num[iNum] = _byt[i+iVal];
                  iNum++;
                  iVal++;
               }

               if (iNum < iMax) {
                  CString error;
                  error.Format( "Invalid %s character in byte string escape sequence '%c'.",
                     base == 8 ? "octal" : "hexidecimal",
                     char(_byt[i+iVal]));
                  throw error;
               }

               num[iNum] = '\0';
               int l = strtol(num, NULL, base);

               if (l > 255) {
                  CString error;
                  error.Format( "Invalid %s byte string escape sequence value exceeds 255, found %d.",
                     base == 8 ? "octal" : "hexidecimal", l);
                  throw error;
               }

               _byt[i] = BYTE(l);
               memmove(_byt+i+1, _byt+i+iVal, _len-(i+iVal));
               _len -= iVal-1;
            }
            break;

            default:
               break;
         }
      }
   }
   _byt[_len] = 0;

   return *this;
}

//==============================================================================

#if TESTING
int main()
{
   byteString bs("\\001"); bs.dump(); bs.xlate(); bs.dump();

   bs[3] = 65;   bs.dump();
   bs[2] = 66;   bs.dump();
   bs = "HellO"; bs.dump();

   bs = "X\\1X";           bs.dump(); bs.xlate(); bs.dump(); 
   bs = "X\\x41X";         bs.dump(); bs.xlate(); bs.dump(); 
   bs = "\\x41\\x42\\xff"; bs.dump(); bs.xlate(); bs.dump();

   byteString b1("This");
   byteString b2("That");
   b1.append(b2); b1.dump();

   b1 = "A<br>B<br>C\\tD";
   b1.dump();
   b1.append("\\tE");
   b1.dump();
   b1.xlate();
   b1.dump();
   b1.replace("<br>", "\r\n");
   b1.dump();
}
#endif
