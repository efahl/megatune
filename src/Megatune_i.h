/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Wed Feb 15 19:20:10 2006
 */
/* Compiler settings for .\Megatune.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __Megatune_i_h__
#define __Megatune_i_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __ISymbol_FWD_DEFINED__
#define __ISymbol_FWD_DEFINED__
typedef interface ISymbol ISymbol;
#endif 	/* __ISymbol_FWD_DEFINED__ */


#ifndef __IApplication_FWD_DEFINED__
#define __IApplication_FWD_DEFINED__
typedef interface IApplication IApplication;
#endif 	/* __IApplication_FWD_DEFINED__ */


#ifndef __Application_FWD_DEFINED__
#define __Application_FWD_DEFINED__

#ifdef __cplusplus
typedef class Application Application;
#else
typedef struct Application Application;
#endif /* __cplusplus */

#endif 	/* __Application_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_Megatune_0000 */
/* [local] */ 




extern RPC_IF_HANDLE __MIDL_itf_Megatune_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_Megatune_0000_v0_0_s_ifspec;

#ifndef __ISymbol_INTERFACE_DEFINED__
#define __ISymbol_INTERFACE_DEFINED__

/* interface ISymbol */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ISymbol;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("97E8E3EE-90BC-4A82-AD77-36622E11B284")
    ISymbol : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Units( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Lo( 
            /* [retval][out] */ double __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Hi( 
            /* [retval][out] */ double __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Digits( 
            /* [retval][out] */ short __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Scale( 
            /* [retval][out] */ double __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Translate( 
            /* [retval][out] */ double __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Label( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [in] */ int Index,
            /* [retval][out] */ double __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ int Index,
            /* [in] */ double newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ValueRaw( 
            /* [in] */ int Index,
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ValueRaw( 
            /* [in] */ int Index,
            /* [in] */ double newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ValueString( 
            /* [in] */ int Index,
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Rows( 
            /* [retval][out] */ int __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Cols( 
            /* [retval][out] */ int __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_NoRange( 
            /* [retval][out] */ BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Index( 
            /* [in] */ int ix,
            /* [in] */ int iy,
            /* [retval][out] */ int __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsBits( 
            /* [retval][out] */ BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Size( 
            /* [retval][out] */ int __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SizeOf( 
            /* [retval][out] */ int __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsConst( 
            /* [retval][out] */ BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_nValues( 
            /* [retval][out] */ int __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Page( 
            /* [retval][out] */ int __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Max( 
            /* [retval][out] */ double __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Min( 
            /* [retval][out] */ double __RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISymbolVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISymbol __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISymbol __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISymbol __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISymbol __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISymbol __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISymbol __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISymbol __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            ISymbol __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Units )( 
            ISymbol __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Lo )( 
            ISymbol __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Hi )( 
            ISymbol __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Digits )( 
            ISymbol __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Scale )( 
            ISymbol __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Translate )( 
            ISymbol __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Label )( 
            ISymbol __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Value )( 
            ISymbol __RPC_FAR * This,
            /* [in] */ int Index,
            /* [retval][out] */ double __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Value )( 
            ISymbol __RPC_FAR * This,
            /* [in] */ int Index,
            /* [in] */ double newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ValueRaw )( 
            ISymbol __RPC_FAR * This,
            /* [in] */ int Index,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ValueRaw )( 
            ISymbol __RPC_FAR * This,
            /* [in] */ int Index,
            /* [in] */ double newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ValueString )( 
            ISymbol __RPC_FAR * This,
            /* [in] */ int Index,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Rows )( 
            ISymbol __RPC_FAR * This,
            /* [retval][out] */ int __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Cols )( 
            ISymbol __RPC_FAR * This,
            /* [retval][out] */ int __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_NoRange )( 
            ISymbol __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Index )( 
            ISymbol __RPC_FAR * This,
            /* [in] */ int ix,
            /* [in] */ int iy,
            /* [retval][out] */ int __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsBits )( 
            ISymbol __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Size )( 
            ISymbol __RPC_FAR * This,
            /* [retval][out] */ int __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SizeOf )( 
            ISymbol __RPC_FAR * This,
            /* [retval][out] */ int __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsConst )( 
            ISymbol __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_nValues )( 
            ISymbol __RPC_FAR * This,
            /* [retval][out] */ int __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Page )( 
            ISymbol __RPC_FAR * This,
            /* [retval][out] */ int __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Max )( 
            ISymbol __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Min )( 
            ISymbol __RPC_FAR * This,
            /* [retval][out] */ double __RPC_FAR *pVal);
        
        END_INTERFACE
    } ISymbolVtbl;

    interface ISymbol
    {
        CONST_VTBL struct ISymbolVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISymbol_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISymbol_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISymbol_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISymbol_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISymbol_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISymbol_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISymbol_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISymbol_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)

#define ISymbol_get_Units(This,pVal)	\
    (This)->lpVtbl -> get_Units(This,pVal)

#define ISymbol_get_Lo(This,pVal)	\
    (This)->lpVtbl -> get_Lo(This,pVal)

#define ISymbol_get_Hi(This,pVal)	\
    (This)->lpVtbl -> get_Hi(This,pVal)

#define ISymbol_get_Digits(This,pVal)	\
    (This)->lpVtbl -> get_Digits(This,pVal)

#define ISymbol_get_Scale(This,pVal)	\
    (This)->lpVtbl -> get_Scale(This,pVal)

#define ISymbol_get_Translate(This,pVal)	\
    (This)->lpVtbl -> get_Translate(This,pVal)

#define ISymbol_get_Label(This,pVal)	\
    (This)->lpVtbl -> get_Label(This,pVal)

#define ISymbol_get_Value(This,Index,pVal)	\
    (This)->lpVtbl -> get_Value(This,Index,pVal)

#define ISymbol_put_Value(This,Index,newVal)	\
    (This)->lpVtbl -> put_Value(This,Index,newVal)

#define ISymbol_get_ValueRaw(This,Index,pVal)	\
    (This)->lpVtbl -> get_ValueRaw(This,Index,pVal)

#define ISymbol_put_ValueRaw(This,Index,newVal)	\
    (This)->lpVtbl -> put_ValueRaw(This,Index,newVal)

#define ISymbol_get_ValueString(This,Index,pVal)	\
    (This)->lpVtbl -> get_ValueString(This,Index,pVal)

#define ISymbol_get_Rows(This,pVal)	\
    (This)->lpVtbl -> get_Rows(This,pVal)

#define ISymbol_get_Cols(This,pVal)	\
    (This)->lpVtbl -> get_Cols(This,pVal)

#define ISymbol_get_NoRange(This,pVal)	\
    (This)->lpVtbl -> get_NoRange(This,pVal)

#define ISymbol_Index(This,ix,iy,pVal)	\
    (This)->lpVtbl -> Index(This,ix,iy,pVal)

#define ISymbol_get_IsBits(This,pVal)	\
    (This)->lpVtbl -> get_IsBits(This,pVal)

#define ISymbol_get_Size(This,pVal)	\
    (This)->lpVtbl -> get_Size(This,pVal)

#define ISymbol_get_SizeOf(This,pVal)	\
    (This)->lpVtbl -> get_SizeOf(This,pVal)

#define ISymbol_get_IsConst(This,pVal)	\
    (This)->lpVtbl -> get_IsConst(This,pVal)

#define ISymbol_get_nValues(This,pVal)	\
    (This)->lpVtbl -> get_nValues(This,pVal)

#define ISymbol_get_Page(This,pVal)	\
    (This)->lpVtbl -> get_Page(This,pVal)

#define ISymbol_get_Max(This,pVal)	\
    (This)->lpVtbl -> get_Max(This,pVal)

#define ISymbol_get_Min(This,pVal)	\
    (This)->lpVtbl -> get_Min(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISymbol_get_Name_Proxy( 
    ISymbol __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB ISymbol_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISymbol_get_Units_Proxy( 
    ISymbol __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB ISymbol_get_Units_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISymbol_get_Lo_Proxy( 
    ISymbol __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pVal);


void __RPC_STUB ISymbol_get_Lo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISymbol_get_Hi_Proxy( 
    ISymbol __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pVal);


void __RPC_STUB ISymbol_get_Hi_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISymbol_get_Digits_Proxy( 
    ISymbol __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *pVal);


void __RPC_STUB ISymbol_get_Digits_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISymbol_get_Scale_Proxy( 
    ISymbol __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pVal);


void __RPC_STUB ISymbol_get_Scale_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISymbol_get_Translate_Proxy( 
    ISymbol __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pVal);


void __RPC_STUB ISymbol_get_Translate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISymbol_get_Label_Proxy( 
    ISymbol __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB ISymbol_get_Label_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISymbol_get_Value_Proxy( 
    ISymbol __RPC_FAR * This,
    /* [in] */ int Index,
    /* [retval][out] */ double __RPC_FAR *pVal);


void __RPC_STUB ISymbol_get_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISymbol_put_Value_Proxy( 
    ISymbol __RPC_FAR * This,
    /* [in] */ int Index,
    /* [in] */ double newVal);


void __RPC_STUB ISymbol_put_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISymbol_get_ValueRaw_Proxy( 
    ISymbol __RPC_FAR * This,
    /* [in] */ int Index,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB ISymbol_get_ValueRaw_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISymbol_put_ValueRaw_Proxy( 
    ISymbol __RPC_FAR * This,
    /* [in] */ int Index,
    /* [in] */ double newVal);


void __RPC_STUB ISymbol_put_ValueRaw_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISymbol_get_ValueString_Proxy( 
    ISymbol __RPC_FAR * This,
    /* [in] */ int Index,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB ISymbol_get_ValueString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISymbol_get_Rows_Proxy( 
    ISymbol __RPC_FAR * This,
    /* [retval][out] */ int __RPC_FAR *pVal);


void __RPC_STUB ISymbol_get_Rows_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISymbol_get_Cols_Proxy( 
    ISymbol __RPC_FAR * This,
    /* [retval][out] */ int __RPC_FAR *pVal);


void __RPC_STUB ISymbol_get_Cols_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISymbol_get_NoRange_Proxy( 
    ISymbol __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pVal);


void __RPC_STUB ISymbol_get_NoRange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISymbol_Index_Proxy( 
    ISymbol __RPC_FAR * This,
    /* [in] */ int ix,
    /* [in] */ int iy,
    /* [retval][out] */ int __RPC_FAR *pVal);


void __RPC_STUB ISymbol_Index_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISymbol_get_IsBits_Proxy( 
    ISymbol __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pVal);


void __RPC_STUB ISymbol_get_IsBits_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISymbol_get_Size_Proxy( 
    ISymbol __RPC_FAR * This,
    /* [retval][out] */ int __RPC_FAR *pVal);


void __RPC_STUB ISymbol_get_Size_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISymbol_get_SizeOf_Proxy( 
    ISymbol __RPC_FAR * This,
    /* [retval][out] */ int __RPC_FAR *pVal);


void __RPC_STUB ISymbol_get_SizeOf_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISymbol_get_IsConst_Proxy( 
    ISymbol __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pVal);


void __RPC_STUB ISymbol_get_IsConst_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISymbol_get_nValues_Proxy( 
    ISymbol __RPC_FAR * This,
    /* [retval][out] */ int __RPC_FAR *pVal);


void __RPC_STUB ISymbol_get_nValues_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISymbol_get_Page_Proxy( 
    ISymbol __RPC_FAR * This,
    /* [retval][out] */ int __RPC_FAR *pVal);


void __RPC_STUB ISymbol_get_Page_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISymbol_get_Max_Proxy( 
    ISymbol __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pVal);


void __RPC_STUB ISymbol_get_Max_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISymbol_get_Min_Proxy( 
    ISymbol __RPC_FAR * This,
    /* [retval][out] */ double __RPC_FAR *pVal);


void __RPC_STUB ISymbol_get_Min_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISymbol_INTERFACE_DEFINED__ */


#ifndef __IApplication_INTERFACE_DEFINED__
#define __IApplication_INTERFACE_DEFINED__

/* interface IApplication */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IApplication;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("55CB6183-A34A-4FB6-B853-878734DDDD07")
    IApplication : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Symbol( 
            /* [in] */ BSTR Name,
            /* [retval][out] */ ISymbol __RPC_FAR *__RPC_FAR *ppSymbol) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE BurnConst( 
            /* [in] */ int PageNo) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetConst( 
            /* [in] */ int PageNo) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SymbolsCount( 
            /* [retval][out] */ int __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Symbols( 
            /* [in] */ int Index,
            /* [retval][out] */ ISymbol __RPC_FAR *__RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IApplicationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IApplication __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IApplication __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IApplication __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IApplication __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IApplication __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IApplication __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IApplication __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Symbol )( 
            IApplication __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [retval][out] */ ISymbol __RPC_FAR *__RPC_FAR *ppSymbol);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BurnConst )( 
            IApplication __RPC_FAR * This,
            /* [in] */ int PageNo);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetConst )( 
            IApplication __RPC_FAR * This,
            /* [in] */ int PageNo);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SymbolsCount )( 
            IApplication __RPC_FAR * This,
            /* [retval][out] */ int __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Symbols )( 
            IApplication __RPC_FAR * This,
            /* [in] */ int Index,
            /* [retval][out] */ ISymbol __RPC_FAR *__RPC_FAR *pVal);
        
        END_INTERFACE
    } IApplicationVtbl;

    interface IApplication
    {
        CONST_VTBL struct IApplicationVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IApplication_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IApplication_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IApplication_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IApplication_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IApplication_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IApplication_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IApplication_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IApplication_Symbol(This,Name,ppSymbol)	\
    (This)->lpVtbl -> Symbol(This,Name,ppSymbol)

#define IApplication_BurnConst(This,PageNo)	\
    (This)->lpVtbl -> BurnConst(This,PageNo)

#define IApplication_GetConst(This,PageNo)	\
    (This)->lpVtbl -> GetConst(This,PageNo)

#define IApplication_get_SymbolsCount(This,pVal)	\
    (This)->lpVtbl -> get_SymbolsCount(This,pVal)

#define IApplication_get_Symbols(This,Index,pVal)	\
    (This)->lpVtbl -> get_Symbols(This,Index,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IApplication_Symbol_Proxy( 
    IApplication __RPC_FAR * This,
    /* [in] */ BSTR Name,
    /* [retval][out] */ ISymbol __RPC_FAR *__RPC_FAR *ppSymbol);


void __RPC_STUB IApplication_Symbol_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IApplication_BurnConst_Proxy( 
    IApplication __RPC_FAR * This,
    /* [in] */ int PageNo);


void __RPC_STUB IApplication_BurnConst_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IApplication_GetConst_Proxy( 
    IApplication __RPC_FAR * This,
    /* [in] */ int PageNo);


void __RPC_STUB IApplication_GetConst_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IApplication_get_SymbolsCount_Proxy( 
    IApplication __RPC_FAR * This,
    /* [retval][out] */ int __RPC_FAR *pVal);


void __RPC_STUB IApplication_get_SymbolsCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IApplication_get_Symbols_Proxy( 
    IApplication __RPC_FAR * This,
    /* [in] */ int Index,
    /* [retval][out] */ ISymbol __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IApplication_get_Symbols_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IApplication_INTERFACE_DEFINED__ */



#ifndef __Megatune_LIBRARY_DEFINED__
#define __Megatune_LIBRARY_DEFINED__

/* library Megatune */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_Megatune;

EXTERN_C const CLSID CLSID_Application;

#ifdef __cplusplus

class DECLSPEC_UUID("FBBD4DEF-D556-48CF-8B96-68D4CD93D67A")
Application;
#endif
#endif /* __Megatune_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
