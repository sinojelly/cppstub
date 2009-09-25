
#ifndef _TEST_FUNCTION_H_
#define _TEST_FUNCTION_H_

#include <windows.h>
#include <stdio.h>
#if 0
#include <iostream.h>
#include <iomanip.h>
#else //VC2005
#include <iostream>
#include <iomanip>
using namespace std;
#endif

extern int g_bInStub;
extern void Simple();
extern void Simple_stub();
extern void InvokeSimple();


extern int g_bInStub2;
extern void Simple2();
extern void Simple2_stub();
extern void Invoke2Simples();

extern int g_bInStub3;
extern void Simple3();
extern void Simple3_stub();
extern void Invoke3Simples();

//extern void ParamSimple_stub(int a);
class CSimpleStub
{
public:
	void ParamSimple_stub(int a)
	{
		g_bInStub = TRUE;
	}
};

extern void memcpy_stub();
extern void InvokeMemCpy();


extern UCHAR g_ucOverLoadStub;
extern void InvokeOverLoad();

class CSimple
{
public:
	void SimpleMethod()
	{
		return;
	}

	void ParamMethod(int a)
	{
		return;
	}
	
	virtual void VirtualMethod()
	{
		fprintf(stdout, "In CSimple::VirtualMethod\r\n");
		return;
	}
};


class CDerived : public CSimple
{
public:
	virtual void VirtualMethod()
	{
		return;
	}
	void DerivedMethod()
	{
		return;
	}
};

extern CSimple g_obj;
extern CDerived *g_pObj;
extern void InvokeMethod();
extern void InvokeParamMethod();
extern void InvokeParentVirtualMethod();
extern void InvokeDerivedMethod();
extern void InvokeInit();
extern void InvokeVirtualMethod();



// 虚函数调用测试函数
#define PRINT_SIZE(CLASS_NAME) \
cout << "The size of " << #CLASS_NAME << " is " << sizeof(CLASS_NAME) << endl;

#define PRINT_DETAIL(CLASS_NAME, OBJECT_NAME) \
	cout << "The detail of " << #CLASS_NAME << " is "; \
	for (int i = 0; i < sizeof(OBJECT_NAME); ++i) { \
	cout.fill('0'); \
	cout << setw(2) << hex << ((short)((char*)(&(OBJECT_NAME)))[i] & 0x00ff); \
	cout << ' '; } \
cout << setw(0) << dec << endl;

#define PRINT_SIZE_DETAIL(CLASS_NAME) \
	PRINT_SIZE(CLASS_NAME) \
	CLASS_NAME _##CLASS_NAME; \
PRINT_DETAIL(CLASS_NAME, _##CLASS_NAME)

#define LF \
cout << endl;

template<class T>
inline void *
get_obj_addr(const T & obj)
{
	return (void*)&obj;
}

inline void *
get_vp_addr(void * start, int offset)
{
	return (void*)((char*)start + offset);
}

inline void *
get_vt_addr(void * vp)
{
	return (void*)*((void**)vp);
}

inline void *
get_vti_val(void * vt, int idx)
{
	return (void*)*((void**)((int*)vt + idx));
}

#define PRINT_VTABLE_ITEM(OBJ, VPOFFSET, INDEX) \
{ \
	cout.fill(' '); \
	cout << setw(8) << setiosflags(ios::left); \
	cout << #OBJ << " :"; \
	void * obj_addr = get_obj_addr(OBJ); \
	cout << hex << " objadr:" << obj_addr; \
	void * vp_addr = get_vp_addr(obj_addr, VPOFFSET); \
	cout << " vpadr:" << vp_addr; \
	void * vt_addr = get_vt_addr(vp_addr); \
	cout << " vtadr:" << vt_addr; \
	void * vti_val = get_vti_val(vt_addr, INDEX); \
	cout << " vtival(" << INDEX << "):" << vti_val << dec << setiosflags(ios::right) << endl; \
}

#define PRINT_OBJ_ADR(OBJ) \
cout << #OBJ << "'s address is : " << hex << get_obj_addr(OBJ) << dec << endl;

#define PRINT_PT(PT) \
cout << #PT << "'s value is : " << hex << PT << dec << endl;




#endif