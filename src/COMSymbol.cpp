#include "stdafx.h"
#include "Megatune.h"
#include "COMSymbol.h"

#include <comdef.h>

#include "msDatabase.h"
extern msDatabase mdb;

STDMETHODIMP CSymbol::get_Name      (BSTR   *pVal) { *pVal = A2BSTR(_symbol->name());  return S_OK; }
STDMETHODIMP CSymbol::get_Units     (BSTR   *pVal) { *pVal = A2BSTR(_symbol->units()); return S_OK; }
STDMETHODIMP CSymbol::get_Lo        (double *pVal) { *pVal = _symbol->lo();            return S_OK; }
STDMETHODIMP CSymbol::get_Hi        (double *pVal) { *pVal = _symbol->hi();            return S_OK; }
STDMETHODIMP CSymbol::get_Digits    (short  *pVal) { *pVal = _symbol->digits();        return S_OK; }
STDMETHODIMP CSymbol::get_Scale     (double *pVal) { *pVal = _symbol->scale();         return S_OK; }
STDMETHODIMP CSymbol::get_Translate (double *pVal) { *pVal = _symbol->trans();         return S_OK; }
STDMETHODIMP CSymbol::get_Label     (BSTR   *pVal) { *pVal = A2BSTR(_symbol->label()); return S_OK; }

STDMETHODIMP CSymbol::get_Value(int Index, double *pVal) { *pVal = _symbol->valueUser(Index); return S_OK; }

STDMETHODIMP CSymbol::put_Value(int Index, double newVal)
{
   _symbol->storeValue(newVal, Index);
   mdb.cDesc.send(_symbol, Index);
   return S_OK;
}

STDMETHODIMP CSymbol::get_ValueRaw(int Index, long *pVal)
{
   *pVal = _symbol->valueRaw(Index);
   return S_OK;
}

STDMETHODIMP CSymbol::put_ValueRaw(int Index, double newVal)
{
   _symbol->storeRaw(newVal, Index);
   mdb.cDesc.send(_symbol, Index);
   return S_OK;
}

STDMETHODIMP CSymbol::get_ValueString(int Index, BSTR *pVal)
{
   const char* pValStr = NULL;

   if (_symbol->isBits())
   {
      signed long value = _symbol->valueRaw();
      if ( value < _symbol->userStrings.size() && _symbol->userStrings[value] != "INVALID")
            pValStr = _symbol->userStrings[value];
      else
      {
         static char s[64];
         sprintf(s, "%.0f", _symbol->valueRaw());
         pValStr = s;
      }
   }

   if (pValStr == NULL)
      pValStr = _symbol->valueString(Index);

   *pVal = A2BSTR(pValStr);

   return S_OK;
}


STDMETHODIMP CSymbol::get_Rows(int *pVal)
{
   int Rows, Cols;
   _symbol->shape(Rows, Cols);
   *pVal = Rows;

   return S_OK;
}

STDMETHODIMP CSymbol::get_Cols(int *pVal)
{
   int Rows, Cols;
   _symbol->shape(Rows, Cols);
   *pVal = Cols;

   return S_OK;
}


STDMETHODIMP CSymbol::get_NoRange(BOOL *pVal)
{
   *pVal = (_symbol->hi() == _symbol->noRange) || (_symbol->lo() == _symbol->noRange);
   return S_OK;
}

STDMETHODIMP CSymbol::Index(int ix, int iy, int *pVal)
{
   *pVal = _symbol->index(ix, iy);
   return S_OK;
}

STDMETHODIMP CSymbol::get_IsBits(BOOL *pVal)
{
   *pVal = _symbol->isBits();
   return S_OK;
}

STDMETHODIMP CSymbol::get_IsConst(BOOL *pVal)
{
   *pVal = _symbol->isConst();
   return S_OK;
}

STDMETHODIMP CSymbol::get_Size(int *pVal)
{
   *pVal = _symbol->size();
   return S_OK;
}

STDMETHODIMP CSymbol::get_SizeOf(int *pVal)
{
   *pVal = _symbol->sizeOf();
   return S_OK;
}

STDMETHODIMP CSymbol::get_nValues(int *pVal)
{
   *pVal = _symbol->nValues();
   return S_OK;
}

STDMETHODIMP CSymbol::get_Page(int *pVal)
{
   *pVal = _symbol->page();
   return S_OK;
}

#define toUser(v,s,t) (double((v+t)*s))

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

STDMETHODIMP CSymbol::get_Max(double *pVal)
{
   if (_symbol->hi() == symbol::noRange)
   {
      if (_symbol->isSigned())
      {
         switch (_symbol->sizeOf())
         {
            case 1: *pVal = toUser(S08max, _symbol->scale(), _symbol->trans()); break;
            case 2: *pVal = toUser(S16max, _symbol->scale(), _symbol->trans()); break;
            case 4: *pVal = toUser(S32max, _symbol->scale(), _symbol->trans()); break;
         }
      }
      else
      {
         switch (_symbol->sizeOf())
         {
            case 1: *pVal = toUser(U08max, _symbol->scale(), _symbol->trans()); break;
            case 2: *pVal = toUser(U16max, _symbol->scale(), _symbol->trans()); break;
            case 4: *pVal = toUser(U32max, _symbol->scale(), _symbol->trans()); break;
         }
      }
   }
   else
      *pVal = _symbol->hi();

   return S_OK;
}

STDMETHODIMP CSymbol::get_Min(double *pVal)
{
   if (_symbol->lo() == symbol::noRange)
   {
      if (_symbol->isSigned())
      {
         switch (_symbol->sizeOf())
         {
            case 1: *pVal = toUser(S08min, _symbol->scale(), _symbol->trans()); break;
            case 2: *pVal = toUser(S16min, _symbol->scale(), _symbol->trans()); break;
            case 4: *pVal = toUser(S32min, _symbol->scale(), _symbol->trans()); break;
         }
      }
      else
      {
         switch (_symbol->sizeOf())
         {
            case 1: *pVal = toUser(U08min, _symbol->scale(), _symbol->trans()); break;
            case 2: *pVal = toUser(U16min, _symbol->scale(), _symbol->trans()); break;
            case 4: *pVal = toUser(U32min, _symbol->scale(), _symbol->trans()); break;
         }
      }
   }
   else
      *pVal = _symbol->lo();

   return S_OK;
}
