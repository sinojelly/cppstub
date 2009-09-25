#include "TestFunction.h"


int g_bInStub;

void Simple()
{
	g_bInStub = FALSE;
}

void Simple_stub()
{
	g_bInStub = TRUE;
}

// void ParamSimple_stub(int a)
// {
// 	g_bInStub = TRUE;
// }

void InvokeSimple()
{
	Simple();
}


int g_bInStub2;
void Simple2()
{
	g_bInStub2 = FALSE;
}

void Simple2_stub()
{
	g_bInStub2 = TRUE;
}

void Invoke2Simples()
{
	Simple();
	Simple2();
}


// 桩函数类型可以与原类型不同
void memcpy_stub()
{
	g_bInStub = TRUE;
}

void InvokeMemCpy()
{
	memcpy((void*)0, (const void *)0, 0);
}


int g_bInStub3;

void Simple3()
{
	g_bInStub3 = FALSE;
}

void Simple3_stub()
{
	g_bInStub3 = TRUE;
}

void Invoke3Simples()
{
	Simple();
	Simple2();
	Simple3();
}


UCHAR g_ucOverLoadStub;

void OverLoad(int a)
{
	g_ucOverLoadStub |= 0x01;
	return;
}

void OverLoad(int a, int b)
{
	g_ucOverLoadStub |= 0x2;
	return;
}

void OverLoad_stub()
{
	g_ucOverLoadStub |= 0x4;
}

void InvokeOverLoad()
{
	OverLoad(1);
	OverLoad(1, 2);
}



void InvokeMethod()
{
	CSimple a;
	a.SimpleMethod();
}

void InvokeParamMethod()
{
	CSimple a;
	a.ParamMethod(10);
}

CSimple g_obj;
void InvokeParentVirtualMethod()
{	
	g_obj.VirtualMethod();
}


void InvokeDerivedMethod()
{
	CDerived *p;
	p = new CDerived;
	p->DerivedMethod();
}

CDerived *g_pObj;
void InvokeInit()
{
	g_pObj = new CDerived;
}
void InvokeVirtualMethod()
{
	//CSimple *p;		
	g_pObj->VirtualMethod();
}
