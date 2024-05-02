/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Wed Feb 15 19:20:10 2006
 */
/* Compiler settings for .\Megatune.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID IID_ISymbol = {0x97E8E3EE,0x90BC,0x4A82,{0xAD,0x77,0x36,0x62,0x2E,0x11,0xB2,0x84}};


const IID IID_IApplication = {0x55CB6183,0xA34A,0x4FB6,{0xB8,0x53,0x87,0x87,0x34,0xDD,0xDD,0x07}};


const IID LIBID_Megatune = {0x1564034E,0x05AC,0x485D,{0x9B,0x9B,0x96,0xA0,0x4A,0x71,0x2A,0x71}};


const CLSID CLSID_Application = {0xFBBD4DEF,0xD556,0x48CF,{0x8B,0x96,0x68,0xD4,0xCD,0x93,0xD6,0x7A}};


#ifdef __cplusplus
}
#endif

