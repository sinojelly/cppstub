
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

CRiticalSection g_objCritical; // ������ȫ
UINT g_uiMaxStubs = 1000; // ͬʱ���ڵ�stub�ĸ���

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

/** ApiHook ģ���ʼ������

  @param INT nMaxHookFuncCounts�����������õĹ������� 
  
	@return INT (by default) ��������NULL ��ʾʧ��
	
*/

HANDLE STUB_Init(UINT uMaxFunctions)

{
	CRiticalSafe wait(g_objCritical);

	if (NULL != g_hApiHook)
	{
		return g_hApiHook;
	}

	// ��־��ʼ��
	GetModuleFileName(NULL, g_szLogFile, MAX_PATH);  // ����GetModuleFileName�ǵõ��İ���Ӧ�ó�����������·��
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

/** ͨ����ַ������ĳ�������Ĺ��Ӻ���

@param HANDLE hApiHook������STUB_Init()�������ɵľ�� 

@param DWORD dwSrcFuncAddr����Դ������ַ 

@param DWORD dwNewFuncAddr�������Ӻ�����ַ 

@return INT (by default) ��������-1��ʾʧ��;����0��ʾ��HOOK�����е����

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
	
	/* �����Ƿ��������˹��� */	
	for ( i = 0; i < pApiHook->uMaxFunctions; i++ )		
	{		
		if ( pApiHook->pHookData[i].dwSrcFuncAddr == lpSrcFunc )			
		{			
			/* ����Ѿ������˹���,�����ı� */			
			nAlreadyFlag = 1;			
			break;			
		}		
	}
	
	/* ���û������Դ�����Ĺ��Ӻ���,�ڱ����ҳ�һ���ɹ���¼��λ�� */	
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
	
	/* ���µĹ��Ӻ�����ַ��¼������ */	
	pApiHook->pHookData[i].dwNewFuncAddr = dwNewFuncAddr;
	
	/* ������δ��뽫Դ����ͷ��6���ֽڱ��浽���� */	
	lppNewFunc = (DWORD)(&(pApiHook->pHookData[i].dwNewFuncAddr) );
	
	if ( !nAlreadyFlag )		
	{		
		/* ��Դ������ʼ��6���ֽڱ��浽 byHeaderCode.�� */		
		memcpy( pApiHook->pHookData[i].byHeaderCode,(const void *)lpSrcFunc,6);		
	}
	
	/* ������δ��뽫Դ�����ײ�6���ֽڸĳ�Ϊһ����ת���º�����ַ��ָ�� */
	R_FALSE_RET_1(VirtualProtect( (LPVOID)lpSrcFunc, 6, PAGE_EXECUTE_READWRITE, &dwOldProtect ) != 0, -1, GetLastError());
	
	*(unsigned char *)lpSrcFunc = (unsigned char)0xff;
	
	*(((unsigned char *)lpSrcFunc)+1) = (unsigned char)0x25;
	
	//memcpy( (void *)(lpSrcFunc+2),(const void *)&lppNewFunc,4 );  //Ϊ֧�ֶ�memcpy�Ĵ�׮���޸�����
	for (uiIndex = 2; uiIndex < 6; uiIndex++)
	{
	 *(((unsigned char *)lpSrcFunc)+uiIndex) = *(((unsigned char *)&lppNewFunc) + uiIndex - 2);
	}

	R_FALSE_RET_1(VirtualProtect( (LPVOID)lpSrcFunc,6,dwOldProtect, &dwNewProtect) != 0, -1, GetLastError());
	
	pApiHook->pHookData[i].wFlag = 1;
	
	pApiHook->pHookData[i].dwSrcFuncAddr = lpSrcFunc; 
	
	return (INT)i;
	
}

// �˺�������Ϊ����ӿڣ����Բ���CRiticalSafe wait(g_objCritical);
INT STUB_UnSetByIndex(HANDLE hApiHook, UINT i)
{
	R_ASSERT(NULL != hApiHook, -1);
	
	APIHOOK *pApiHook = (APIHOOK *)hApiHook;
	
	R_ASSERT(i < pApiHook->uMaxFunctions, -1);
	
	if (pApiHook->pHookData[i].wFlag == 0) // �������
	{
		return 0;
	}
	
	DWORD dwOldProtect, dwNewProtect;
	DWORD lpSrcFunc = pApiHook->pHookData[i].dwSrcFuncAddr;
	
	/* ��lpSrcFuncָ����ڴ������޸�Ϊ���Զ�д�� */
	R_ASSERT(VirtualProtect( ( LPVOID)lpSrcFunc,6,PAGE_EXECUTE_READWRITE, &dwOldProtect ) != 0, -1);
	
	//memcpy( (void *)lpSrcFunc,pApiHook->pHookData[i].byHeaderCode,6 ); //Ϊ֧�ֶ�memcpy��׮�޸�
	UINT uiIndex;
	for (uiIndex = 0; uiIndex < 6; uiIndex++)
	{
		*(((unsigned char *)lpSrcFunc) + uiIndex) = pApiHook->pHookData[i].byHeaderCode[uiIndex];
	}
	
	/* �ָ�lpSrcFuncָ����ڴ����� */
	R_ASSERT(VirtualProtect( (LPVOID)lpSrcFunc,6,dwOldProtect,&dwNewProtect ) != 0, -1);
	
	pApiHook->pHookData[i].wFlag = 0;
	
	return 0;
}


/** ȡ��ĳ�������Ĺ��Ӻ�������

@param HANDLE hApiHook������STUB_Init()�������ɵľ�� 

@param DWORD dwSrcFuncAddr����Դ������ַ 

@return INT (by default) ��������-1��ʾʧ��;����0��ʾ��HOOK�����е����

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
		return 0;		// Ҫ�����׮�����ڣ���Ϊ�ɹ�
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

 // inline����չ��Ҳ���ǲ��ܴ�ӡ���ô����ļ����к�
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