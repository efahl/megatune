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
#include "symbol.h"
#include "msDatabase.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern msDatabase   mdb;

//------------------------------------------------------------------------------

symErr::symErr(const char *m, const char *t, int p) : msg(m), pos(p)
{
   txt = _strdup(t); // Memory leak, but who cares.
}

//------------------------------------------------------------------------------

const double symbol::noRange = 123E45;

#define round(v) ((v) < 0.0 ? ((v)-_round) : ((v)+_round))
#define toUser(v,s,t) double((v+t)*s)
#define toIntr(v,s,t) (DWORD((round(v)/s)-t))

//------------------------------------------------------------------------------

#define notDigit(c) (strchr("0123456789", c) == NULL)
#define isSign(c)   (strchr("+-",c) != NULL)
#define isChar(c,x) (toupper(c) == x)
#define notChar(c,x) (toupper(c) != x)

#define skipSp {while (*s == ' ') s++;}
#define next   {s++; skipSp}

//------------------------------------------------------------------------------
// The shape parameter has the form '[oN]' or '[oNxN]' where 'N' are integer
// dimensions of the array and 'o' is an optional order specify taking the
// form "X:" or "Y:".  So, for an X-ordered 6x12 array you could say either
// "[X:6x12]" or just "[6x12]" (the default is x-ordered).

void symbol::parseArraySpec(const CString &ss)
{
   _xOrder   = true;
   _order    = 0;
   _shape[0] = 1;
   _shape[1] = 1;

   const char *s = ss; skipSp;
   const char *o = s;

   if (notChar(*s, '[')) throw symErr("Missing opening '['", o, s-o);
   next; // Past opening [.

   if (isChar(*s, 'Y') || isChar(*s, 'X')) {
      _xOrder = isChar(*s, 'X');
      next; // Skip the X or Y.
      if (notChar(*s, ':')) throw symErr("Missing ':' after order specifier", o, s-o);
      next; // Skip the colon.
   }

   if (notDigit(*s)) throw symErr("Missing number in first dimension", o, s-o);
   _shape[0] = strtol(s, const_cast<char **>(&s), 10);
   _order    = 1;
   skipSp; // Past any trailing blanks.

   if (isChar(*s, ']')) {
      next; // Past the closing ].
   }
   else {
      if (notChar(*s, 'X')) throw symErr("Missing 'x' or ']' after first dimension", o, s-o);
      next; // Skip the x.
      if (notDigit(*s)) throw symErr("Missing number in second dimension", o, s-o);
      _shape[1] = strtol(s, const_cast<char **>(&s), 10);
      _order    = 2;
      if (notChar(*s, ']')) throw symErr("Missing closing bracket ']' after second dimension", o, s-o);
      next; // Past the closing ].
   }
   if (*s) throw symErr("Useless junk in array specification", o, s-o);
}

//------------------------------------------------------------------------------
// The "bitSpec" parameter takes the form '[N:N+O]' where 'N' are integer
// bit offset values and O is a bit offset for computation of user values.

void symbol::parseBitSpec(const CString &ss)
{
   _bitLo  = -1;
   _bitHi  = -1;
   _bitOfs =  0;

   const char *s = ss; skipSp;
   const char *o = s;

   if (_signed) throw symErr("Bit field mappings may not be on signed values", o, 0);

   if (notChar(*s, '[')) throw symErr("Missing opening bracket '['", o, s-o);
   next; // Past opening [.

   if (notDigit(*s)) throw symErr("Missing number in low-bit position", o, s-o);
   _bitLo = strtol(s, const_cast<char **>(&s), 10);
   if (_bitLo < 0 || _bitLo >= _sizeOf*8) throw symErr("Lower bit specification out of range", o, s-o-1);
   skipSp; // Past any trailing blanks.

   if (notChar(*s, ':')) throw symErr("Missing ':' in bit field specification", o, s-o);
   next; // Skip the colon.

   if (notDigit(*s)) throw symErr("Missing number in high-bit position", o, s-o);
   _bitHi = strtol(s, const_cast<char **>(&s), 10);
   if (_bitHi < 0 || _bitHi >= _sizeOf*8) throw symErr("Upper bit specification out of range", o, s-o);
   if (_bitLo > _bitHi) throw symErr("Bits indices specificied backwards", o, s-o);
   skipSp; // Past any trailing blanks.

   if (isSign(*s)) { // We've got a bit offset to parse.
      _bitOfs = strtol(s, const_cast<char **>(&s), 10);
   }

   if (notChar(*s, ']')) throw symErr("Missing closing bracket ']' after high-bit value", o, s-o);
   next; // Past the closing ].

   if (*s) throw symErr("Useless junk in bit field specification", o, s-o);
}

//------------------------------------------------------------------------------
// The "type" parameter has the form ['S' | 'U']['08' | '16' | '32'].
// Maybe "F32" and "F64" in the future.

void symbol::parseTypeSpec(const CString &ss)
{
   _signed = false;
   _sizeOf = 1;

   const char *s = ss; skipSp;
   const char *o = s;

   if (isChar(*s, 'S') || isChar(*s, 'U'))
      _signed = isChar(*s, 'S');
   else
      throw symErr("Missing 'S' or 'U' at start of type specification", o, s-o);
   next; // Past sign indicator

   if (notDigit(*s)) throw symErr("Missing number in type specification", o, s-o);
   switch (strtol(s, const_cast<char **>(&s), 10)) {
      case  8: _sizeOf = 1; break;
      case 16: _sizeOf = 2; break;
      case 32: _sizeOf = 4; break;
      default: throw symErr("Data size incorrect in type specification, must be 8, 16 or 32", o, s-o);
   }
   skipSp;

   if (*s) throw symErr("Useless junk in type specification", o, s-o);
}

//------------------------------------------------------------------------------

#undef skipSp
#undef next

//------------------------------------------------------------------------------

#define U08min static_cast<unsigned char >(         0)
#define U16min static_cast<unsigned short>(         0)
#define U32min static_cast<unsigned long >(         0)
#define U08max static_cast<unsigned char >(      0xff)
#define U16max static_cast<unsigned short>(    0xffff)
#define U32max static_cast<unsigned long >(0xffffffff)

#define S08min static_cast<  signed char >(      0x80)
#define S16min static_cast<  signed short>(    0x8000)
#define S32min static_cast<  signed long >(0x80000000)
#define S08max static_cast<  signed char >(      0x7f)
#define S16max static_cast<  signed short>(    0x7fff)
#define S32max static_cast<  signed long >(0x7fffffff)

void symbol::setRawLimits()
{
   if (_signed) {
      switch (_sizeOf) {
         case 1:
            _loRaw = _lo == noRange ? S08min : toIntr(_lo, fabs(_scale), _trans);
            _hiRaw = _hi == noRange ? S08max : toIntr(_hi, fabs(_scale), _trans);
            break;
         case 2:
            _loRaw = _lo == noRange ? S16min : toIntr(_lo, fabs(_scale), _trans);
            _hiRaw = _hi == noRange ? S16max : toIntr(_hi, fabs(_scale), _trans);
            break;
         case 4:
            _loRaw = _lo == noRange ? S32min : toIntr(_lo, fabs(_scale), _trans);
            _hiRaw = _hi == noRange ? S32max : toIntr(_hi, fabs(_scale), _trans);
            break;
      }
   }
   else {
      switch (_sizeOf) {
         case 1:
            _loRaw = _lo == noRange ? U08min : toIntr(_lo, _scale, _trans);
            _hiRaw = _hi == noRange ? U08max : toIntr(_hi, _scale, _trans);
            break;
         case 2:
            _loRaw = _lo == noRange ? U16min : toIntr(_lo, _scale, _trans);
            _hiRaw = _hi == noRange ? U16max : toIntr(_hi, _scale, _trans);
            break;
         case 4:
            _loRaw = _lo == noRange ? U32min : toIntr(_lo, _scale, _trans);
            _hiRaw = _hi == noRange ? U32max : toIntr(_hi, _scale, _trans);
            break;
      }
   }
}

//------------------------------------------------------------------------------

void symbol::setBasic(const CString &name, const CString &type, int page, int offset, const CString &units, bool isConst)
{
   _sequence = -1;
   _name     = name;
   _units    = units;
   _pageNo   = page;
   _offset   = offset;
   _const    = isConst;
   parseTypeSpec(type);
}

//------------------------------------------------------------------------------

void symbol::setScalar(const CString &name, const CString &type, int page, int offset, const CString &units, double scale, double trans, double lo, double hi, int digits, bool isConst)
{
   setBasic(name, type, page, offset, units, isConst);
   _scale  = scale;
   _trans  = trans;
   _lo     = lo;
   _hi     = hi;
   if (digits > 32) digits = 32; // Try to make sure we don't make too-long strings.
   _digits = digits;
   _round  = fabs(_scale/2.0);
   setRawLimits();
}

void symbol::setCScalar(const CString &name, const CString &type, int page, int offset, const CString &units, double scale, double trans, double lo, double hi, int digits)
{
   setScalar(name, type, page, offset, units, scale, trans, lo, hi, digits, true);
}

void symbol::setOScalar(const CString &name, const CString &type, int page, int offset, const CString &units, double scale, double trans)
{
   int    d = 0;
   double s = scale;
   while (s > 0.0 && s < 1.0) {
      d++;
      s *= 10.0;
   }
   setScalar(name, type, page, offset, units, scale, trans, noRange, noRange, d, false);
}

//------------------------------------------------------------------------------

void symbol::setArray(const CString &name, const CString &type, int page, int offset, const CString &units, const CString &shape, double scale, double trans, double lo, double hi, int digits, bool isConst)
{
   setScalar(name, type, page, offset, units, scale, trans, lo, hi, digits, isConst);
   parseArraySpec(shape);
}

void symbol::setCArray(const CString &name, const CString &type, int page, int offset, const CString &units, const CString &shape, double scale, double trans, double lo, double hi, int digits)
{
   setArray(name, type, page, offset, units, shape, scale, trans, lo, hi, digits, true);
}


//------------------------------------------------------------------------------

void symbol::setBits(const CString &name, const CString &type, int page, int offset, const CString &bitSpec, bool isConst)
{
   setBasic(name, type, page, offset, CString(""), isConst);
   parseBitSpec(bitSpec);
   _lo = 0;
   _hi = nValues()-1;
}

void symbol::setCBits(const CString &name, const CString &type, int page, int offset, const CString &bitSpec)
{
   setBits(name, type, page, offset, bitSpec, true);
}

void symbol::setOBits(const CString &name, const CString &type, int page, int offset, const CString &bitSpec)
{
   setBits(name, type, page, offset, bitSpec, false);
}

//------------------------------------------------------------------------------

void symbol::setExpr(const CString &name, const CString &expr, const CString &units, const CString &file, int line)
{
   static int n = 0;
   _sequence = n++;
   _name     = name;
   _expr     = expr;
   _const    = false;
   _units    = units;
   _file     = file;
   _line     = line;
}

//------------------------------------------------------------------------------

void symbol::varIndex(int index)
{
   assert(!_const);
   _index = index;
}

//------------------------------------------------------------------------------

int symbol::nValues() const
{
   return isBits() ? (1 << (_bitHi-_bitLo+1)) : (_shape[0]*_shape[1]); 
}

int symbol::size() const
{
   return _sizeOf * _shape[0] * _shape[1];
}

void symbol::shape(int &rows, int &cols) const
{
   if (xOrder() && _order > 1) {
      cols = _shape[0];
      rows = _shape[1];
   }
   else {
      rows = _shape[0];
      cols = _shape[1];
   }
}

bool symbol::xOrder() const
{
   return _xOrder;
}

double symbol::minUser() const
{
   double vmin = valueUser(0);
   for (int i = 1; i < nValues(); i++) {
      double v = valueUser(i);
      if (v < vmin) vmin = v;
   }
   return vmin;
}

double symbol::maxUser() const
{
   double vmax = valueUser(0);
   for (int i = 1; i < nValues(); i++) {
      double v = valueUser(i);
      if (v > vmax) vmax = v;
   }
   return vmax;
}

//------------------------------------------------------------------------------

signed long symbol::valueRaw(int index) const
{
   signed long v = 0;
   switch (_sizeOf) {
      case 1: v = mdb.cDesc.getB(_pageNo, _offset+_sizeOf*index, _const?0:1); break;
      case 2: v = mdb.cDesc.getW(_pageNo, _offset+_sizeOf*index, _const?0:1); break;
      case 4: v = mdb.cDesc.getD(_pageNo, _offset+_sizeOf*index, _const?0:1); break;
   }

   if (_signed) {
      // Sign extend signed negatives if _sizeOf < 4.
      switch (_sizeOf) {
         case 1: if (v & 0x00000080) v |= 0xffffff00; break;
         case 2: if (v & 0x00008000) v |= 0xffff0000; break;
      }
   }

   if (isBits()) {
      long mask = ((1 << (_bitHi-_bitLo+1)) - 1) << _bitLo;
      v  = (v & mask) >> _bitLo;
   }
   return v;
}

//------------------------------------------------------------------------------

double symbol::valueUser(int index) const
{
   return isExpr() ? mdb.cDesc._userVar[_index+index] : toUser(valueRaw(index), _scale, _trans)+_bitOfs;
}

const char *symbol::valueString(int index) const
{
   static char s[64];
   sprintf(s, "%.*f", _digits, valueUser(index));
   return s;
}

const char *symbol::valueUsing(int index, symbol *from, bool scaleOnly) const
{
   static char s[64];
   double trans = scaleOnly ? 0.0 : from->_trans;
   double x     = isExpr() ? mdb.cDesc._userVar[_index+index] : toUser(valueRaw(index), from->_scale, trans)+from->_bitOfs;
   sprintf(s, "%.*f", from->_digits, x);
   return s;
}

const char *symbol::loString() const
{
   static char s[64];
   sprintf(s, "%.*f", _digits, _lo);
   return s;
}

const char *symbol::hiString() const
{
   static char s[64];
   sprintf(s, "%.*f", _digits, _hi);
   return s;
}

//------------------------------------------------------------------------------

void symbol::storeRaw(long value, int index) const
{
   if (isBits()) {
      signed long v = 0;
      switch (_sizeOf) {
         case 1: v = mdb.cDesc.getB(_pageNo, _offset+_sizeOf*index); break;
         case 2: v = mdb.cDesc.getW(_pageNo, _offset+_sizeOf*index); break;
         case 4: v = mdb.cDesc.getD(_pageNo, _offset+_sizeOf*index); break;
      }

      int mask = ((1 << (_bitHi-_bitLo+1)) - 1) << _bitLo;
      value    = (v & ~mask) | (value << _bitLo);
   }

   // Signedness doesn't matter, as we are extracting a subset of bytes.

   switch (_sizeOf) {
      case 1: mdb.cDesc.putB(value, _pageNo, _offset+_sizeOf*index); break;
      case 2: mdb.cDesc.putW(value, _pageNo, _offset+_sizeOf*index); break;
      case 4: mdb.cDesc.putD(value, _pageNo, _offset+_sizeOf*index); break;
   }
}

//------------------------------------------------------------------------------

bool symbol::incRaw(int amount, int index) const
{
// if (_scale < 0.0) amount = -amount; // User value has opposite sign of internal.
   if (_signed) {
      signed long v = valueRaw(index);
      signed long l; memcpy(&l, &_loRaw, sizeof(signed long));
      signed long h; memcpy(&h, &_hiRaw, sizeof(signed long));
      switch (_sizeOf) {
         case 1: if (amount<0 && v+amount >= l  || amount>0 && v+amount <= h) v += amount; break;
         case 2: if (amount<0 && v+amount >= l  || amount>0 && v+amount <= h) v += amount; break;
         case 4: if (amount<0 && v+amount >= l  || amount>0 && v+amount <= h) v += amount; break;
      }
      storeRaw(v, index);
   }
   else {
      unsigned long v = valueRaw(index);
      unsigned long l; memcpy(&l, &_loRaw, sizeof(unsigned long));
      unsigned long h; memcpy(&h, &_hiRaw, sizeof(unsigned long));
      switch (_sizeOf) {
         case 1: if (amount<0 && v >= -amount && v+amount >= l || amount>0 && v+amount <= h) v += amount; break;
         case 2: if (amount<0 && v >= -amount && v+amount >= l || amount>0 && v+amount <= h) v += amount; break;
         case 4: if (amount<0 && v >= -amount && v+amount >= l || amount>0 && v+amount <= h) v += amount; break;
      }
      storeRaw(v, index);
   }
   return true; // Fix this so it returns true only if the data was changed...
}

//------------------------------------------------------------------------------

bool symbol::incValue(double amount, int index) const
{
   // Only scale amount, as it is relative, not absolute.
   return incRaw(DWORD(round(amount)/_scale), index);
}

//------------------------------------------------------------------------------

void symbol::storeValue(double dtt, int index) const
{
   if (isBits()) dtt -= _bitOfs;
   storeRaw(toIntr(dtt, _scale, _trans), index);
}

//------------------------------------------------------------------------------

bool symbol::storeValue(CString valueString) const
{
   if (!isBits()               ) return false;
   if (valueString == "INVALID") return false;

   int nVals  = nValues();
   for (int ii = 0; ii < nVals; ii++) {
      if (valueString == userStrings[ii]) {
         storeRaw(ii);
         return true;
      }
   }
   return false;
}

//------------------------------------------------------------------------------

void symbol::storeUsing(double dtt, int index, symbol *from, bool scaleOnly) const
{
   if (isBits()) dtt -= _bitOfs;
   double trans = scaleOnly ? 0.0 : from->_trans;
   storeRaw(DWORD(((dtt < 0.0 ? (dtt-from->_round) : (dtt+from->_round))/from->_scale)-trans), index);
}

//------------------------------------------------------------------------------
bool symbol::tooLow (double dtt) const { return _lo != noRange && dtt < _lo; }
bool symbol::tooHigh(double dtt) const { return _hi != noRange && dtt > _hi; }

//------------------------------------------------------------------------------

const CString symbol::label(int index) const
{
   char idxLbl[100];

#define XX (index/(_xOrder?_shape[0]:_shape[1])+1)
#define YY (index%(_xOrder?_shape[0]:_shape[1])+1)

   switch (_order) {
      case 0: sprintf(idxLbl, "%s",        _name         ); break;
      case 1: sprintf(idxLbl, "%s[%d]",    _name, index+1); break;
      case 2: sprintf(idxLbl, "%s[%d,%d]", _name, XX, YY ); break;
   }
   return idxLbl;
}

//------------------------------------------------------------------------------

int symbol::index(int ix, int iy) const
{
// return _xOrder ? (ix + _shape[1] * iy) : (iy + _shape[0] * ix);
   if (_order < 2)
      return iy;
   else
      return _xOrder ? (ix + _shape[0] * iy) : (iy + _shape[1] * ix);
}

int symbol::offset(int index) const
{
   if (index < 0) index = 0;
   return _offset + index*_sizeOf;
}

//------------------------------------------------------------------------------

bool symbol::dump(FILE *f)
{
   if (_order != 0) dPrintL(f, "");
   dPrintN(f, "%-20s ", _name);
   int im = mdb.cDesc.pageOffset(_pageNo, isVar())+_offset;
   if (isBits()) {
      unsigned iV = valueRaw();
      CString  uv = "???";
      if (iV >= 0 && iV < userStrings.size()) uv = userStrings[iV];
      dPrintN(f, "v=\"%s\"  r=%#06x ", uv, iV);
      dPrintL(f, "o=%5d z=%5d h=[%d:%d] IM=%04x", _offset, _sizeOf, _bitLo, _bitHi, im);
   }
   else {
      if (_order == 0) {
         dPrintN(f, "v=%10s r=%#06x ", valueString(), valueRaw());
      }
      dPrintN(f, "o=%5d z=%5d s=%f t=%f u=%s IM=%04x ", _offset, _sizeOf, _scale, _trans, _units, im);
      switch (_order) {
         case 0: dPrintL(f, ""); break;

         case 1: {
            dPrintL(f, "h=[%2d]",  _shape[0]);
            int i;
            for (i = 0; i < _shape[0]; i++) {
               dPrintL(f, "   %3d - %10s %#06x", i, valueString(i), valueRaw(i));
            }
            for (i =0; i < 72; i++) dPrintN(f, "-"); dPrintL(f, "");
            break;
         }

         case 2: {
            int rows, cols;
            shape(rows, cols);
            dPrintN(f, "h=[c%2d,r%2d]", cols, rows);
            dPrintL(f, "");
            dPrintN(f, "User    ");
            int r,c;
            for (c = 0; c < cols; c++) {
               dPrintN(f, " [%8d]", c);
            }
            dPrintL(f, "");
            for (r = 0; r < rows; r++) {
               dPrintN(f, "[%5d]", r);
               for (int c = 0; c < cols; c++) {
                  int idx = index(c, r);
                  dPrintN(f, " %10s", valueString(idx));
               }
               dPrintL(f, "");
            }
            dPrintL(f, "");
            dPrintN(f, "Raw     ");
            for (c = 0; c < cols; c++) {
               dPrintN(f, " [%8d]", c);
            }
            dPrintL(f, "");
            for (r = 0; r < rows; r++) {
               dPrintN(f, "[%9d]", r);
               for (int c = 0; c < cols; c++) {
                  int idx = index(c, r);
                  dPrintN(f, " %#06x[%04x]", valueRaw(idx), idx);
               }
               dPrintL(f, "");
            }
            for (int i =0; i < 7+11*cols; i++) dPrintN(f, "-"); dPrintL(f, "");
            break;
         }
      }
   }
   return true;
}

//------------------------------------------------------------------------------

static void formatAppend(CString &self, const char *format, ...)
{
   CString tail;
   va_list args;
   va_start(args, format);
      tail.FormatV(format, args);
      self += tail;
   va_end(args);
}

CString symbol::valueFormatted()
{
   CString fmt;
   if (isBits()) {
      formatAppend(fmt, "\"%s\"", userStrings[valueRaw()]);
   }
   else {
      bool isArray = !isBits() && nValues() > 1;
      CString separator = "";
      if (isArray) {
         separator = " ";
         formatAppend(fmt, "\n");
      }

      int rows, cols;
      shape(rows, cols);
      for (int r = 0; r < rows; r++) {
         for (int c = 0; c < cols; c++) {
            formatAppend(fmt, separator+valueString(index(c, r)));
         }
         if (isArray) formatAppend(fmt, "\n");
      }
   }
   return fmt;
}

//------------------------------------------------------------------------------
