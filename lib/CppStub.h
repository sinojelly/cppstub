#ifndef _CPP_STUB_H_
#define _CPP_STUB_H_

// 注意: 编译静态lib库，使用编译参数/Zl即可解决多种工程的链接问题。函数声明不需要dll导出__declspec(dllexport) 关键字。
//   /Zl 忽略 .OBJ 中的默认库名 omit default library name in .OBJ 

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct APIHOOKDATA_st {
	
	DWORD dwSrcFuncAddr; /* 源函数的地址 */
	
	DWORD dwNewFuncAddr; /* 钩子函数的地址 */
	
	BYTE byHeaderCode[6]; /* 源函数起始6字节 */
	
	WORD wFlag; /* 用来表示是否设置了HOOK的标志 */
	
} APIHOOKDATA;

typedef struct APIHOOK_st {
	
	APIHOOKDATA *pHookData;
	
	UINT uMaxFunctions;
	
} APIHOOK;

extern void STUB_UnSetAll(HANDLE hApiHook);
extern void STUB_Log(char *file, int line, char *format, ...);

#define LOG(str)                          STUB_Log(__FILE__, __LINE__, str)
#define LOG_1(str, para1)                 STUB_Log(__FILE__, __LINE__, str, para1)
#define LOG_2(str, para1, para2)          STUB_Log(__FILE__, __LINE__, str, para1, para2)
#define LOG_3(str, para1, para2, para3)   STUB_Log(__FILE__, __LINE__, str, para1, para2, para3)

#define R_ASSERT(test, ret)        \
{ \
    if (!(test)) \
	{ \
		LOG_1("Assert: %s", #test); \
		return ret; \
	} \
}

#define R_ASSERT_1(test, ret, para1)        \
{ \
    if (!(test)) \
	{ \
		LOG_2("Assert: %s (" #para1 " = %d).", #test, para1); \
		return ret; \
	} \
}

#define R_ASSERT_2(test, ret, para1, para2)        \
{ \
    if (!(test)) \
	{ \
		LOG_2("Assert: %s (" #para1 " = %d, " #para2 " = %d)." , #test, para1, para2); \
		return ret; \
	} \
}

#define V_ASSERT(test)        \
{ \
    if (!(test)) \
	{ \
		LOG_1("Assert: %s", #test); \
		return; \
	} \
}

#define V_ASSERT_1(test, para1)        \
{ \
    if (!(test)) \
	{ \
		LOG_2("Assert: %s (" #para1 " = %d).", #test, para1); \
		return; \
	} \
}

#define V_ASSERT_2(test, para1, para2)        \
{ \
    if (!(test)) \
	{ \
		LOG_2("Assert: %s (" #para1 " = %d, " #para2 " = %d)." , #test, para1, para2); \
		return; \
	} \
}

#define R_FALSE_RET(test, ret)  \
{ \
	if (!(test)) \
	{ \
		LOG("Exception: " #test); \
		return ret; \
	} \
}

#define R_FALSE_RET_1(test, ret, para1)  \
{ \
	if (!(test)) \
	{ \
		LOG_1("Exception: " #test " (" #para1 " = %d).", para1); \
		return ret; \
	} \
}

#define R_FALSE_RET_2(test, ret, para1, para2)  \
{ \
	if (!(test)) \
	{ \
		LOG_2("Exception: " #test " (" #para1 " = %d, " #para2 " = %d).", para1, para2); \
		return ret; \
	} \
}

#define R_FALSE_RET_S(test, ret, str)  \
{ \
	if (!(test)) \
	{ \
		LOG_1("Exception: " #test " %s", str); \
		return ret; \
	} \
}

#define R_FALSE_RET_S_1(test, ret, str, para1)  \
{ \
	if (!(test)) \
	{ \
		LOG_2("Exception: " #test " %s (" #para1 " = %d).", str, para1); \
		return ret; \
	} \
}

#define R_FALSE_RET_S_2(test, ret, str, para1, para2)  \
{ \
	if (!(test)) \
	{ \
		LOG_3("Exception: " #test " %s (" #para1 " = %d, " #para2 " = %d).", str, para1, para2); \
		return ret; \
	} \
}

#define V_FALSE_RET(test)  \
{ \
	if (!(test)) \
	{ \
		LOG("Exception: " #test); \
		return; \
	} \
}

#define V_FALSE_RET_1(test, para1)  \
{ \
	if (!(test)) \
	{ \
		LOG_1("Exception: " #test " (" #para1 " = %d).", para1); \
		return; \
	} \
}

#define V_FALSE_RET_2(test, para1, para2)  \
{ \
	if (!(test)) \
	{ \
		LOG_2("Exception: " #test " (" #para1 " = %d, " #para2 " = %d).", para1, para2); \
		return; \
	} \
}

#define V_FALSE_RET_S(test, str)  \
{ \
	if (!(test)) \
	{ \
		LOG_1("Exception: " #test " %s", str); \
		return; \
	} \
}

#define V_FALSE_RET_S_1(test, str, para1)  \
{ \
	if (!(test)) \
	{ \
		LOG_2("Exception: " #test " %s (" #para1 " = %d).", str, para1); \
		return; \
	} \
}

#define V_FALSE_RET_S_2(test, str, para1, para2)  \
{ \
	if (!(test)) \
	{ \
		LOG_3("Exception: " #test " %s (" #para1 " = %d, " #para2 " = %d).", str, para1, para2); \
		return; \
	} \
}

#ifdef __cplusplus
}
#endif

#endif