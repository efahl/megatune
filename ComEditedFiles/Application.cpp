#include "stdafx.h"
#include "Megatune.h"
#include "Application.h"

#include "msDatabase.h"

extern msDatabase mdb;

#include "Megatune_i.h"
#include "COMSymbol.h"

STDMETHODIMP CApplication::Symbol(BSTR Name, ISymbol **ppSymbol)
{
   int nLen = WideCharToMultiByte(CP_ACP,0,Name,SysStringLen(Name),NULL,0,NULL,NULL);
   char* name = new char[nLen+1];
   WideCharToMultiByte(CP_ACP,0,Name,SysStringLen(Name),name,nLen,NULL,NULL);
   name[nLen] = '\0' ;

   symbol *sym = mdb.cDesc.lookup(name);
   delete[] name;

   if (sym == NULL)
      return Error(_T("Symbol not found"));

   CSymbol* pComSymbol = new CComObject<CSymbol>;
   pComSymbol->AttachSymbol(sym);

   return pComSymbol->QueryInterface( IID_ISymbol, (void **)(ppSymbol) );
}


STDMETHODIMP CApplication::BurnConst(int PageNo)
{
   int savePage = mdb.setPageNo(PageNo, true);
   mdb.burnConst(); // Burn it into flash.
   mdb.setPageNo(savePage);

   return S_OK;
}

STDMETHODIMP CApplication::GetConst(int PageNo)
{
   int savePage = mdb.setPageNo(PageNo, true);
   mdb.getConst(PageNo);
   mdb.setPageNo(savePage);

   return S_OK;
}

STDMETHODIMP CApplication::get_SymbolsCount(int *pVal)
{
   *pVal = mdb.cDesc.symbolCount();

   return S_OK;
}

STDMETHODIMP CApplication::get_Symbols(int Index, ISymbol **ppSymbol)
{
   symbol *sym = mdb.cDesc.symbols(Index);

   if (sym == NULL)
      return Error(_T("Symbol index out of bounds"));

   CSymbol* pComSymbol = new CComObject<CSymbol>;
   pComSymbol->AttachSymbol(sym);

   return pComSymbol->QueryInterface( IID_ISymbol, (void **)(ppSymbol) );
}
