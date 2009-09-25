
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "CppStub.h"
#include "CriticalSafe.h"

#ifdef __cplusplus
extern "C"
{
#endif

HANDLE g_hApiHook = NULL;
char g_szLogFile[MAX_PATH];

CRiticalSection g_objCritical; // 多任务安全
UINT g_uiMaxStubs = 1000; // 同时存在的stub的个数

void STUB_SetMaxStubs(UINT uiMaxStubs)
{
	CRiticalSafe wait(g_objCritical);

	if (g_hApiHook)
	{
		STUB_UnSetAll(g_hApiHook);		
		delete ((APIHOOK *)g_hApiHook)->pHookData;
		delete g_hApiHook;
		g_hApiHook = NULL;
	}

	g_uiMaxStubs = uiMaxStubs;
}

/** ApiHook 模块初始化函数

  @param INT nMaxHookFuncCounts——最大可设置的钩子数量 
  
	@return INT (by default) ——返回NULL 表示失败
	
*/

HANDLE STUB_Init(UINT uMaxFunctions)

{
	CRiticalSafe wait(g_objCritical);

	if (NULL != g_hApiHook)
	{
		return g_hApiHook;
	}

	// 日志初始化
	GetModuleFileName(NULL, g_szLogFile, MAX_PATH);  // 利用GetModuleFileName是得到的包含应用程序名的完整路径
	char *pPathEnd = strrchr(g_szLogFile, '\\');
	if (pPathEnd != NULL)
	{
		*pPathEnd = '\0';
	}
	sprintf(g_szLogFile, "%s\\CppStub.log", g_szLogFile);
	
	APIHOOK * pApiHook = (APIHOOK *)malloc(sizeof(APIHOOK));
	
	if ( pApiHook )		
	{
		
		pApiHook->uMaxFunctions = uMaxFunctions;
		
		pApiHook->pHookData =
			
			(APIHOOKDATA *)malloc(sizeof(APIHOOKDATA) *uMaxFunctions);
		
		if ( NULL != pApiHook->pHookData)
			
		{
			
			memset(pApiHook->pHookData,0,sizeof(APIHOOKDATA) *uMaxFunctions );
			g_hApiHook = (HANDLE)pApiHook;
			
			return (HANDLE)pApiHook;
			
		}
		
		free(pApiHook);
		
	}
	
	return NULL;
	
}

/** 通过地址来设置某个函数的钩子函数

@param HANDLE hApiHook——由STUB_Init()函数生成的句柄 

@param DWORD dwSrcFuncAddr——源函数地址 

@param DWORD dwNewFuncAddr——钩子函数地址 

@return INT (by default) ——返回-1表示失败;返回0表示在HOOK数组中的序号

*/

 INT STUB_SetByAddr(HANDLE hApiHook,DWORD dwSrcFuncAddr,
					  
					  DWORD dwNewFuncAddr)
					  
{
	 CRiticalSafe wait(g_objCritical);

	if (NULL == hApiHook)
	{
		hApiHook = STUB_Init(g_uiMaxStubs);		
	}

	R_ASSERT(NULL != hApiHook, -1);
	
	DWORD dwOldProtect;
	
	DWORD dwNewProtect;
	
	DWORD lpSrcFunc;
	
	DWORD lppNewFunc;
	
	UINT i;
	UINT uiIndex;
	
	INT nAlreadyFlag = 0;
	
	APIHOOK *pApiHook = (APIHOOK *)hApiHook;
	
	lpSrcFunc = dwSrcFuncAddr;
	
	/* 查找是否已设置了钩子 */	
	for ( i = 0; i < pApiHook->uMaxFunctions; i++ )		
	{		
		if ( pApiHook->pHookData[i].dwSrcFuncAddr == lpSrcFunc )			
		{			
			/* 如果已经设置了钩子,仅仅改变 */			
			nAlreadyFlag = 1;			
			break;			
		}		
	}
	
	/* 如果没有设置源函数的钩子函数,在表中找出一个可供记录的位置 */	
	if ( i == pApiHook->uMaxFunctions )		
	{		
		for ( i = 0; i < pApiHook->uMaxFunctions; i++ )			
		{			
			if (pApiHook->pHookData[i].wFlag == 0 )				
			{				
				break;				
			}
			
		} 
		
		R_FALSE_RET_S_1(i < pApiHook->uMaxFunctions, -1, "Stub resource run out!", pApiHook->uMaxFunctions);		
	}
	
	/* 将新的钩子函数地址记录到表中 */	
	pApiHook->pHookData[i].dwNewFuncAddr = dwNewFuncAddr;
	
	/* 以下这段代码将源函数头部6个字节保存到表中 */	
	lppNewFunc = (DWORD)(&(pApiHook->pHookData[i].dwNewFuncAddr) );
	
	if ( !nAlreadyFlag )		
	{		
		/* 将源函数起始处6个字节保存到 byHeaderCode.中 */		
		memcpy( pApiHook->pHookData[i].byHeaderCode,(const void *)lpSrcFunc,6);		
	}
	
	/* 以下这段代码将源函数首部6个字节改成为一条跳转到新函数地址的指令 */
	R_FALSE_RET_1(VirtualProtect( (LPVOID)lpSrcFunc, 6, PAGE_EXECUTE_READWRITE, &dwOldProtect ) != 0, -1, GetLastError());
	
	*(unsigned char *)lpSrcFunc = (unsigned char)0xff;
	
	*(((unsigned char *)lpSrcFunc)+1) = (unsigned char)0x25;
	
	//memcpy( (void *)(lpSrcFunc+2),(const void *)&lppNewFunc,4 );  //为支持对memcpy的打桩，修改这里
	for (uiIndex = 2; uiIndex < 6; uiIndex++)
	{
	 *(((unsigned char *)lpSrcFunc)+uiIndex) = *(((unsigned char *)&lppNewFunc) + uiIndex - 2);
	}

	R_FALSE_RET_1(VirtualProtect( (LPVOID)lpSrcFunc,6,dwOldProtect, &dwNewProtect) != 0, -1, GetLastError());
	
	pApiHook->pHookData[i].wFlag = 1;
	
	pApiHook->pHookData[i].dwSrcFuncAddr = lpSrcFunc; 
	
	return (INT)i;
	
}

// 此函数不作为对外接口，所以不用CRiticalSafe wait(g_objCritical);
INT STUB_UnSetByIndex(HANDLE hApiHook, UINT i)
{
	R_ASSERT(NULL != hApiHook, -1);
	
	APIHOOK *pApiHook = (APIHOOK *)hApiHook;
	
	R_ASSERT(i < pApiHook->uMaxFunctions, -1);
	
	if (pApiHook->pHookData[i].wFlag == 0) // 该项不存在
	{
		return 0;
	}
	
	DWORD dwOldProtect, dwNewProtect;
	DWORD lpSrcFunc = pApiHook->pHookData[i].dwSrcFuncAddr;
	
	/* 将lpSrcFunc指向的内存属性修改为可以读写的 */
	R_ASSERT(VirtualProtect( ( LPVOID)lpSrcFunc,6,PAGE_EXECUTE_READWRITE, &dwOldProtect ) != 0, -1);
	
	//memcpy( (void *)lpSrcFunc,pApiHook->pHookData[i].byHeaderCode,6 ); //为支持对memcpy打桩修改
	UINT uiIndex;
	for (uiIndex = 0; uiIndex < 6; uiIndex++)
	{
		*(((unsigned char *)lpSrcFunc) + uiIndex) = pApiHook->pHookData[i].byHeaderCode[uiIndex];
	}
	
	/* 恢复lpSrcFunc指向的内存属性 */
	R_ASSERT(VirtualProtect( (LPVOID)lpSrcFunc,6,dwOldProtect,&dwNewProtect ) != 0, -1);
	
	pApiHook->pHookData[i].wFlag = 0;
	
	return 0;
}


/** 取消某个函数的钩子函数设置

@param HANDLE hApiHook——由STUB_Init()函数生成的句柄 

@param DWORD dwSrcFuncAddr——源函数地址 

@return INT (by default) ——返回-1表示失败;返回0表示在HOOK数组中的序号

*/

 INT STUB_UnSetByAddr(HANDLE hApiHook,DWORD dwSrcFuncAddr)

{	
	UINT i;
	
	APIHOOK *pApiHook = (APIHOOK *)hApiHook;

	R_ASSERT(NULL != hApiHook, -1);

	CRiticalSafe wait(g_objCritical);
	
	for ( i = 0; i < pApiHook->uMaxFunctions; i++ )		
	{		
		if ( pApiHook->pHookData[i].dwSrcFuncAddr == dwSrcFuncAddr )			
		{			
			break;			
		}		
	}
	
	if ( i == pApiHook->uMaxFunctions )		
	{		
		return 0;		// 要清除的桩不存在，认为成功
	}

	return STUB_UnSetByIndex(hApiHook, i);	
	
}


 void STUB_UnSetAll(HANDLE hApiHook)
{
	CRiticalSafe wait(g_objCritical);

	V_ASSERT(NULL != hApiHook);

	APIHOOK *pApiHook = (APIHOOK *)hApiHook;

	for (UINT i = 0; i < pApiHook->uMaxFunctions; i++ )		
	{
		STUB_UnSetByIndex(hApiHook, i);		
	}	
}

 // inline函数展开也还是不能打印调用处的文件名行号
 void STUB_Log(char *file, int line, char *format, ...)
 {
	 FILE *fLog;
	 char szBuf[500];
	 va_list vList;

	 CRiticalSafe wait(g_objCritical);
	 
	 va_start(vList, format);
	 memset(szBuf, 0, sizeof(szBuf));
	 vsprintf(szBuf, format, vList);
	 va_end(vList);
	 
	 fLog = fopen(g_szLogFile, "a+");
	 V_ASSERT(NULL != fLog);
	 
	 char szCurTime[30];
	 time_t tCurTime;
	 if (-1 != time( &tCurTime ))
	 {
		 (void)strftime(szCurTime, sizeof(szCurTime), "%Y.%m.%d-%H.%M.%S", localtime(&tCurTime) );
	 }
	 else
	 {
		 sprintf(szCurTime, "%s", "Time Error");
	 }

	 char *pPathEnd = strrchr(file, '\\');
	 if (pPathEnd != NULL)
	 {
		 file = pPathEnd + 1;
	 }
	 
	 fprintf(fLog, "%s    (%s, %d) %s\r\n", szCurTime, file, line, szBuf); 
	 
	 fclose(fLog);
 }
 
#ifdef __cplusplus
}
#endif