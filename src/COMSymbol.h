//------------------------------------------------------------------------------
//--  Copyright (c) 2005 by Dag Erlandsson, All Rights Reserved.              --
//--                                                                          --
//--  This program is free software; you can redistribute it and/or           --
//--  modify it under the terms of the GNU General Public License             --
//--  as published by the Free Software Foundation; either version            --
//--  2 of the License, or (at your option) any later version.                --
//--  See http://www.gnu.org/licenses/gpl.txt                                 --
//------------------------------------------------------------------------------

#ifndef __SYMBOL_H_
#define __SYMBOL_H_ 1

#include "resource.h"
#include "symbol.h"

//------------------------------------------------------------------------------

class ATL_NO_VTABLE CSymbol : 
   public CComObjectRootEx<CComSingleThreadModel>,
// public CComCoClass<CSymbol, &CLSID_CoSymbol>,
   public ISupportErrorInfo,
   public IDispatchImpl<ISymbol, &IID_ISymbol, &LIBID_Megatune>
{
private:
   symbol *_symbol;
public:
   CSymbol() { }
   void AttachSymbol(symbol* sym)
   {
      _symbol = sym;
   }

//DECLARE_REGISTRY_RESOURCEID(IDR_SYMBOL)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CSymbol)
   COM_INTERFACE_ENTRY(ISymbol)
   COM_INTERFACE_ENTRY(IDispatch)
   COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(CSymbol)

// ISymbol
public:
   STDMETHOD(get_Min)         (/*[out, retval]*/ double *pVal);
   STDMETHOD(get_Max)         (/*[out, retval]*/ double *pVal);
   STDMETHOD(get_Page)        (/*[out, retval]*/ int    *pVal);
   STDMETHOD(get_nValues)     (/*[out, retval]*/ int    *pVal);
   STDMETHOD(get_SizeOf)      (/*[out, retval]*/ int    *pVal);
   STDMETHOD(get_Size)        (/*[out, retval]*/ int    *pVal);
   STDMETHOD(get_IsConst)     (/*[out, retval]*/ BOOL   *pVal);
   STDMETHOD(get_IsBits)      (/*[out, retval]*/ BOOL   *pVal);

   STDMETHOD(Index)           (/*[in]*/ int ix, /*[in]*/ int iy, /*[out, retval]*/ int* pVal);
   STDMETHOD(get_NoRange)     (/*[out, retval]*/  BOOL  *pVal);
   STDMETHOD(get_Cols)        (/*[out, retval]*/ int    *pVal);
   STDMETHOD(get_Rows)        (/*[out, retval]*/ int    *pVal);

   STDMETHOD(get_ValueString) (/*[in]*/ int Index, /*[out, retval]*/ BSTR *pVal);
   STDMETHOD(get_ValueRaw)    (/*[in]*/ int Index, /*[out, retval]*/ long *pVal);
   STDMETHOD(put_ValueRaw)    (/*[in]*/ int Index, /*[in]*/ double newVal);
   STDMETHOD(get_Value)       (/*[in]*/ int Index, /*[out, retval]*/ double *pVal);
   STDMETHOD(put_Value)       (/*[in]*/ int Index, /*[in]*/ double newVal);
   STDMETHOD(get_Label)       (/*[out, retval]*/ BSTR   *pVal);
   STDMETHOD(get_Translate)   (/*[out, retval]*/ double *pVal);
   STDMETHOD(get_Scale)       (/*[out, retval]*/ double *pVal);
   STDMETHOD(get_Digits)      (/*[out, retval]*/ short  *pVal);
   STDMETHOD(get_Hi)          (/*[out, retval]*/ double *pVal);
   STDMETHOD(get_Lo)          (/*[out, retval]*/ double *pVal);
   STDMETHOD(get_Units)       (/*[out, retval]*/ BSTR   *pVal);
   STDMETHOD(get_Name)        (/*[out, retval]*/ BSTR   *pVal);

//ISupportErrorInfo
   STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
   {
      return (InlineIsEqualGUID(IID_ISymbol,riid)) ? S_OK : S_FALSE;
   }
   
};

#endif //__SYMBOL_H_
