#ifndef _STUB_INTERFACE_H_
#define _STUB_INTERFACE_H_

#ifdef __cplusplus
extern "C"
{
#endif

extern HANDLE g_hApiHook;

extern void STUB_SetMaxStubs(UINT uiMaxStubs);
extern void STUB_UnSetAll(HANDLE hApiHook);

// 下面函数失败返回-1，否则返回非-1的值
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
#define TEST_ASSERT(test)    /* STUB测试工程使用的宏(边界测试必然有异常，希望不打印) */  \
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


// 在第一次SET_STUB之前可以调用SET_STUB_NUM设置同时存在的STUB的个数(默认1000个)，设置会使得原有的所有STUB被清除
#define SET_STUB_NUM(num)     STUB_SetMaxStubs(num)

#define SET_STUB(src, dst)    TEST_ASSERT(-1 != STUB_SetByAddr(g_hApiHook, (DWORD)src, (DWORD)dst))
#define CLR_STUB(src)         TEST_ASSERT(-1 != STUB_UnSetByAddr(g_hApiHook, (DWORD)src))

#define CLR_ALL_STUB()        STUB_UnSetAll(g_hApiHook)

#ifdef __cplusplus
}
#endif

#endif