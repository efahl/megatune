// Application.h : Declaration of the CApplication

#ifndef __APPLICATION_H_
#define __APPLICATION_H_

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CApplication
class ATL_NO_VTABLE CApplication : 
   public CComObjectRootEx<CComSingleThreadModel>,
   public CComCoClass<CApplication, &CLSID_Application>,
   public IDispatchImpl<IApplication, &IID_IApplication, &LIBID_Megatune>
{
public:
   CApplication()
   {
   }

DECLARE_REGISTRY_RESOURCEID(IDR_APPLICATION)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CApplication)
   COM_INTERFACE_ENTRY(IApplication)
   COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IApplication
public:
   STDMETHOD(get_Symbols)(/*[in]*/ int Index, /*[out, retval]*/ ISymbol* *pVal);
   STDMETHOD(get_SymbolsCount)(/*[out, retval]*/ int *pVal);
   STDMETHOD(GetConst)(/*[in]*/ int PageNo);
   STDMETHOD(BurnConst)(/*[in]*/ int PageNo);
   STDMETHOD(Symbol)(/*[in]*/ BSTR Name, /*[out, retval]*/ ISymbol **ppSymbol);
};

#endif //__APPLICATION_H_
