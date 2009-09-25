#ifndef _STUB_INTERFACE_H_
#define _STUB_INTERFACE_H_

#ifdef __cplusplus
extern "C"
{
#endif

extern HANDLE g_hApiHook;

extern void STUB_SetMaxStubs(UINT uiMaxStubs);
extern void STUB_UnSetAll(HANDLE hApiHook);

// ���溯��ʧ�ܷ���-1�����򷵻ط�-1��ֵ
extern INT STUB_UnSetByAddr(HANDLE hApiHook,DWORD dwSrcFuncAddr);
extern INT STUB_SetByAddr(HANDLE hApiHook,DWORD dwSrcFuncAddr, DWORD dwNewFuncAddr);

//#define USE_CPPUNIT
//#define USE_UNITTESTCPP
#define TEST_STUB

#if    defined(USE_CPPUNIT)
#define TEST_ASSERT(test)         CPPUNIT_ASSERT(test)
#elif  defined(USE_UNITTESTCPP)
#define TEST_ASSERT(test)         CHECK(test)
#elif  defined(TEST_STUB)
#define TEST_ASSERT(test)    /* STUB���Թ���ʹ�õĺ�(�߽���Ա�Ȼ���쳣��ϣ������ӡ) */  \
{ \
	if (!(test)) \
	{ \
		/*fprintf(stdout, "(%s, %d) %s\r\n", __FILE__, __LINE__, #test);*/ \
	} \
}
#else
#define TEST_ASSERT(test)         \
{ \
	if (!(test)) \
	{ \
		fprintf(stdout, "(%s, %d) %s\r\n", __FILE__, __LINE__, #test); \
	} \
}
#endif


// �ڵ�һ��SET_STUB֮ǰ���Ե���SET_STUB_NUM����ͬʱ���ڵ�STUB�ĸ���(Ĭ��1000��)�����û�ʹ��ԭ�е�����STUB�����
#define SET_STUB_NUM(num)     STUB_SetMaxStubs(num)

#define SET_STUB(src, dst)    TEST_ASSERT(-1 != STUB_SetByAddr(g_hApiHook, (DWORD)src, (DWORD)dst))
#define CLR_STUB(src)         TEST_ASSERT(-1 != STUB_UnSetByAddr(g_hApiHook, (DWORD)src))

#define CLR_ALL_STUB()        STUB_UnSetAll(g_hApiHook)

#ifdef __cplusplus
}
#endif

#endif