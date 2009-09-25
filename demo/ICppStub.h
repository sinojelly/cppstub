#ifndef _CPP_STUB_INTERFACE_H_
#define _CPP_STUB_INTERFACE_H_

#include <windows.h>

extern HANDLE g_hApiHook;

extern void ApiHook_UnSetAll(HANDLE hApiHook);
extern INT ApiHook_UnSetByAddr(HANDLE hApiHook,DWORD dwSrcFuncAddr);
extern INT ApiHook_SetByAddr(HANDLE hApiHook,DWORD dwSrcFuncAddr, DWORD dwNewFuncAddr);

#define SET_STUB(src, dst)    ApiHook_SetByAddr(g_hApiHook, (DWORD)src, (DWORD)dst)
#define CLR_STUB(src)         ApiHook_UnSetByAddr(g_hApiHook, (DWORD)src)

#define CLR_ALL_STUB()        ApiHook_UnSetAll(g_hApiHook)

#endif