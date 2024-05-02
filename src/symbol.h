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

//{{AFX_INCLUDES()
//}}AFX_INCLUDES
#ifndef SYMTAB_H
#define SYMTAB_H 1

#if _MSC_VER >= 1000
#  pragma once
#endif

#pragma warning(disable:4786)
#include <vector>

//------------------------------------------------------------------------------

struct symErr {
   const char *msg;
   const char *txt;
   int pos;
   symErr(const char *m, const char *t, int p);
};

//------------------------------------------------------------------------------

class symbol {
public:
   int         _sequence; // Order in which they were defined.
private:

   CString     _name;
   CString     _units;
   bool        _const; // True means in constants, false in output channels/variables.

   int         _pageNo; // Zero-based index into tables.
   int         _offset; // Offset of first byte in this table.
   int         _sizeOf; // Number of bytes in one value, 1, 2 or 4 are valid.
   bool        _signed; // Is data signed or unsigned?

   //--  User-units formatting information.  -----------------------------------
   double      _scale;   // Scale of raw value.
   double      _trans;   // Translation of raw value.
   int         _digits;  // Display digits to right of decimal.
   double      _round;   // Fudge factor based upon digits count.
   double      _lo, _hi; // Limit values.
   long        _loRaw, _hiRaw; // Limits in internal format.

   //--  Data for array types.  ------------------------------------------------
   bool        _xOrder;   // Is table in x- or y-order?
   int         _order;    // Tensor order, 0=scalar.
   int         _shape[2]; // Size in up to two dimensions.

   //--  Data for bit fields.  -------------------------------------------------
   int         _bitLo;  // Starting bit in a byte-base bitfield, zero-based.
   int         _bitHi;  // Ending bit in bitfield.
   int         _bitOfs; // Offset for user value when extracted from bits.
                        // For instance, MS-I stores nCylinders in 0:3 as 1..16,
                        // so valueUser returns 0+1==1 for the first value.

   //--  Data for output channels.  --------------------------------------------
   int         _index;  // Index into userVars data block, different than
                        // _offset, which indexes into _ochRaw...
   CString     _expr;
   CString     _file;
   int         _line;

   void parseArraySpec(const CString &ss);
   void parseBitSpec  (const CString &ss);
   void parseTypeSpec (const CString &ss);
   void setBasic      (const CString &name, const CString &type, int page, int offset, const CString &units, bool isConst);
   void setScalar     (const CString &name, const CString &type, int page, int offset, const CString &units,                           double scale, double trans, double lo, double hi, int digits, bool isConst);
   void setArray      (const CString &name, const CString &type, int page, int offset, const CString &units, const CString &shape, double scale, double trans, double lo, double hi, int digits, bool isConst);
   void setBits       (const CString &name, const CString &type, int page, int offset,                           const CString &bitSpec,                                                             bool isConst);

   void setRawLimits();

public:
   static const double noRange;

   std::vector<CString> userStrings; // Use for whatever you like, typically bit field values.

   symbol()
    : _name  ("") 
    , _units ("")
    , _const (true)
    , _pageNo( 0)
    , _offset(-1)
    , _sizeOf( 1)
    , _signed(false)
    , _scale (1.0)
    , _trans (0.0)
    , _digits(0)
    , _round (0.5)
    , _lo    (symbol::noRange)
    , _hi    (symbol::noRange)
    , _xOrder(true)
    , _order (0)
    , _bitLo (-1)
    , _bitHi (-1)
    , _bitOfs( 0)
    , _index (-1)
    , _expr  ("")
   { _shape[0] = _shape[1] = 1; setRawLimits(); }

   void setCScalar(const CString &name, const CString &type, int page, int offset, const CString &units,                           double scale, double trans, double lo, double hi, int digits);
   void setCArray (const CString &name, const CString &type, int page, int offset, const CString &units, const CString &shape, double scale, double trans, double lo, double hi, int digits);
   void setCBits  (const CString &name, const CString &type, int page, int offset,                           const CString &bitSpec);

   void setOScalar(const CString &name, const CString &type, int page, int offset, const CString &units,                           double scale, double trans);
   void setOBits  (const CString &name, const CString &type, int page, int offset,                           const CString &bitSpec);

   void setExpr   (const CString &name, const CString &expr, const CString &units, const CString &file, int line);

   bool        isBits  () const { return _bitLo != -1 && _bitHi != -1; }
   bool        isConst () const { return _const; }
   bool        isExpr  () const { return ! _expr.IsEmpty(); }
   bool        isVar   () const { return !_const && _expr.IsEmpty(); }
   bool        isSigned() const { return _signed; }

   signed long valueRaw   (            int index=0) const;
   double      valueUser  (            int index=0) const;
   const char *valueString(            int index=0) const;
   const char *valueUsing (            int index, symbol *from, bool scaleOnly=false) const;
   const char *loString   () const;
   const char *hiString   () const;


   void        storeRaw   (long   value,  int index=0) const;
   void        storeValue (double dtt,    int index=0) const;
   bool        storeValue (CString valueString) const;
   void        storeUsing (double dtt, int index, symbol *from, bool scaleOnly=false) const;
   bool        incRaw     (int    amount, int index=0) const;
   bool        incValue   (double amount, int index=0) const;

   const CString     &name  () const { return _name;   }
   const CString     &units () const { return _units;  }
   double             lo    () const { return _lo;     }
   double             hi    () const { return _hi;     }
   int                digits() const { return _digits; }
   double             scale () const { return _scale;  }
   double             trans () const { return _trans;  }
   int                bitOfs() const { return _bitOfs; }

   int               nValues() const; // Count of values, irrespective of sizeof.
   int                size  () const; // Byte count for whole symbol.
   int                sizeOf() const { return _sizeOf; }
   int                page  () const { return _pageNo; }
   int                index (int ix, int iy) const;
   int                offset(int index=0) const; // Byte offset of the nth value.
   void               shape (int &, int &) const;
   bool               xOrder() const;

   double             minUser() const;
   double             maxUser() const;

   void               varIndex(int index);
   int                varIndex() const { return _index; } // Index into och userVars.

   const CString     &exprText() const { return _expr; }
   const CString     &exprFile() const { return _file; }
   int                exprLine() const { return _line; }

   const CString      label(int index=0) const;

   bool tooLow (double dtt) const;
   bool tooHigh(double dtt) const;

   bool dump(FILE *f);

   CString valueFormatted();
};

//------------------------------------------------------------------------------
#endif
