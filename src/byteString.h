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

#ifndef BYTESTRING_H
#define BYTESTRING_H 1
//------------------------------------------------------------------------------

#define NULLBS (reinterpret_cast<BYTE *>(0))

class byteString {
   BYTE *_byt; // This is not a string, it contains nulls.
   int   _siz;
   int   _len;

   void copy(const BYTE *s, int l);

public:
   byteString(const BYTE *s=NULL, int l=-1);
   byteString(const char *s,      int l=-1);
   byteString(const byteString &d);
   byteString(BYTE  b);
   byteString(WORD  w);
   byteString(DWORD d);

  ~byteString() { release(); }

   byteString &operator =  (const byteString &rhs);
   bool        operator == (const byteString &rhs) const { return cmp(rhs) == 0; }
   bool        operator != (const byteString &rhs) const { return cmp(rhs) != 0; }
   bool        operator <  (const byteString &rhs) const { return cmp(rhs) <  0; }

   DWORD       len() const { return _len; }
   const BYTE *ptr() const { return _byt; }
   const char *str() const { return reinterpret_cast<const char *>(_byt); }
   BYTE &operator [] (int i) {
      if (i <     0) i = 0;
      if (i >= _len) resize(i+1);
      return _byt[i];
   }

   void        resize(int   newLen);
   byteString &copy  (const byteString &s);
   int         cmp   (const byteString &rhs) const;
   
   byteString &append(const BYTE *s, int len);
   byteString &append(const byteString &s);
   byteString &append(BYTE c);
   byteString &replace(const char *from, const char *to);

   bool       empty   () const { return _len <= 0; }
   byteString &release(); // Delete the contents.
   byteString &xlate  (); // Turn all \nnn into single bytes.

   void dump() {
      printf("%#06x - (%3d:%3d) ", this, _len, _siz);
      for (int i = 0; i < _len; i++) {
         printf("%#04x ", _byt[i]);
      }
      printf("\n");
   }
};

//------------------------------------------------------------------------------
#endif
