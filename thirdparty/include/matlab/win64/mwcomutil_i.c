

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Thu Jan 19 13:49:45 2017
 */
/* Compiler settings for win64\mwcomutil.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=IA64 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

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

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IMWUtil,0xC47EA90E,0x56D1,0x11d5,0xB1,0x59,0x00,0xD0,0xB7,0xBA,0x75,0x44);


MIDL_DEFINE_GUID(IID, LIBID_MWComUtil,0x34E7425D,0x645F,0x49B2,0x85,0xF0,0x64,0x4F,0x96,0x58,0x8F,0x27);


MIDL_DEFINE_GUID(CLSID, CLSID_MWField,0xF619B4C6,0xAACC,0x405A,0x97,0x77,0x70,0x55,0x00,0x01,0x3C,0xD5);


MIDL_DEFINE_GUID(CLSID, CLSID_MWStruct,0x3070E315,0x8CEF,0x4F4D,0xA6,0x73,0x05,0xF1,0x11,0x57,0x66,0xAD);


MIDL_DEFINE_GUID(CLSID, CLSID_MWComplex,0x905AF508,0x54AB,0x4C85,0x8C,0x2A,0x93,0x1E,0xB2,0x36,0xD5,0xEA);


MIDL_DEFINE_GUID(CLSID, CLSID_MWSparse,0x62177C02,0x8BB7,0x460B,0x84,0x78,0x23,0x27,0x79,0x65,0x23,0xA3);


MIDL_DEFINE_GUID(CLSID, CLSID_MWArg,0xAC09C06D,0x66FD,0x411E,0xB7,0x4A,0xAF,0x8D,0x60,0xF5,0x26,0xF2);


MIDL_DEFINE_GUID(CLSID, CLSID_MWArrayFormatFlags,0x8D7D1B9F,0x2AD9,0x457F,0x99,0x72,0x78,0x16,0xA1,0x80,0x20,0x13);


MIDL_DEFINE_GUID(CLSID, CLSID_MWDataConversionFlags,0x40EA9077,0x04D5,0x46E3,0x83,0x97,0xCA,0xDD,0xB7,0x5E,0xA8,0x70);


MIDL_DEFINE_GUID(CLSID, CLSID_MWUtil,0x0E00419C,0x2D5E,0x4EF7,0xBB,0x23,0x4D,0xA1,0xE6,0x2E,0x12,0x86);


MIDL_DEFINE_GUID(CLSID, CLSID_MWFlags,0x6EE18B65,0xCC40,0x4479,0x8A,0xBD,0x19,0xB3,0x57,0x21,0x95,0xF4);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



