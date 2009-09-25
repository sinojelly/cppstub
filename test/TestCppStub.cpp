#include <UnitTest++.h>
#include <windows.h>
#include "..\lib\ICppStub.h"
#include "TestFunction.h"


TEST(TestOverLoadStub)
{
	g_ucOverLoadStub = 0;
	InvokeOverLoad();
	CHECK(0x3 == g_ucOverLoadStub);

	g_ucOverLoadStub = 0;
	//SET_STUB(OverLoad, OverLoad_stub); // error C2440: 'type cast' : cannot convert from '' to 'unsigned long'
                                       // Context does not allow for disambiguation of overloaded function
}


TEST(TestCStub)
{
	CStub stub(Simple, Simple_stub);

	g_bInStub = FALSE;
	InvokeSimple();
	CHECK(TRUE == g_bInStub);
}


TEST(TestLog)
{
	SET_STUB_NUM(1);
	CStub stub(Simple, Simple_stub);
	CStub stub2(Simple2, Simple2_stub);
	
	g_bInStub = FALSE;
	g_bInStub2 = TRUE;
	Invoke2Simples();
	CHECK(TRUE == g_bInStub);
	CHECK(FALSE == g_bInStub2);
	SET_STUB_NUM(1000); // 把个数恢复，否则其它用例无法通过
}

TEST(TestCStub_dtor)
{
	SET_STUB_NUM(1); 

	{
		CStub stub2(Simple2, Simple2_stub);
		CStub stub(Simple, Simple_stub);

		g_bInStub = TRUE;
		g_bInStub2 = FALSE;
		Invoke2Simples();
		CHECK(FALSE == g_bInStub); // Simple的桩设置失败
		CHECK(TRUE == g_bInStub2);

		CStub stub3(Simple2, Simple_stub);

		g_bInStub = FALSE;
		g_bInStub2 = FALSE;
		Invoke2Simples();
		CHECK(TRUE == g_bInStub); 
		CHECK(FALSE == g_bInStub2);
	
		g_bInStub2 = TRUE;
		Invoke2Simples();
		CHECK(TRUE == g_bInStub2);
	}

	// dtor释放资源，再次设置stub成功
	CStub stub(Simple, Simple_stub);

	g_bInStub = FALSE;
	g_bInStub2 = TRUE;
	Invoke2Simples();
	CHECK(TRUE == g_bInStub);
	CHECK(FALSE == g_bInStub2);

	SET_STUB_NUM(1000); 
}

TEST(TestCStubModify)
{
	CStub stub(Simple2, Simple_stub);

	g_bInStub = FALSE;
	g_bInStub2 = FALSE;
	Invoke2Simples();
	CHECK(TRUE == g_bInStub);
	CHECK(FALSE == g_bInStub2);

	g_bInStub2 = TRUE;
	Invoke2Simples();
	CHECK(TRUE == g_bInStub2);

	CStub stub2(Simple2, Simple2_stub);

	g_bInStub = FALSE;
	g_bInStub2 = FALSE;
	Invoke2Simples();
	CHECK(FALSE == g_bInStub);
	CHECK(TRUE == g_bInStub2);
}

TEST(TestMethod)
{
	SET_STUB(P2ADDR(&CSimple::SimpleMethod), Simple_stub);

	g_bInStub = FALSE;
	InvokeMethod();
	CHECK(TRUE == g_bInStub);

	CLR_ALL_STUB();
}

TEST(TestParamMethod)
{
	CStub stub(P2ADDR(&CSimple::ParamMethod), P2ADDR(&CSimpleStub::ParamSimple_stub));
	
	g_bInStub = FALSE;
	InvokeParamMethod();
	CHECK(TRUE == g_bInStub);
}

TEST(TestDerivedMethod)
{
	SET_STUB(P2ADDR(&CDerived::DerivedMethod), Simple_stub);
	
	g_bInStub = FALSE;
	InvokeDerivedMethod();
	CHECK(TRUE == g_bInStub);
	
	CLR_ALL_STUB();
}

/*
TEST(TestNewStub)
{
	CStub stub(P2ADDR(operator new), Simple_stub); // error C2914: 'STUB_MethodAddr' : cannot deduce template argument as function argument is ambiguous
	g_bInStub = FALSE;
	new CSimple;
	CHECK(TRUE == g_bInStub);
}
*/

#if 0

TEST(TestVirtualMethod)
{
	InvokeInit();

	//SET_STUB(P2ADDR(CDerived::VirtualMethod), Simple_stub);
	FastDelegate0<> dgt = MakeDelegate(g_pObj, &CDerived::VirtualMethod);
	dgt();
	fprintf(stdout, "    dgt addr: %#x\n", dgt.FuncAddr());
	fprintf(stdout, "    fun addr: %#x\n", &CDerived::VirtualMethod);
	fprintf(stdout, "par fun addr: %#x\n", &CSimple::VirtualMethod);

	PRINT_VTABLE_ITEM(*g_pObj, 0, 0);

	//SET_STUB(dgt.FuncAddr(), Simple_stub);
	SET_STUB(0x4035d0, Simple_stub);   // ????  需要继续寻找已知this和函数指针情况下，如何找到实际调用的函数地址
	
	g_bInStub = FALSE;
	InvokeVirtualMethod();
	CHECK(TRUE == g_bInStub); // 不能对子类覆盖父类的虚函数打桩

	dgt();

	CLR_ALL_STUB();
}

TEST(TestVirtualMethod_parent)
{
	//fprintf(stdout, "addr: %#x\n", P2ADDR(CSimple::VirtualMethod));
	//SET_STUB(P2ADDR(CSimple::VirtualMethod), Simple_stub);
	//SET_STUB(0x403880, Simple_stub);
	FastDelegate0<> dgt = MakeDelegate(&g_obj, &CSimple::VirtualMethod);
	dgt();
	fprintf(stdout, "addr: %#x\n", dgt.FuncAddr());
	SET_STUB(dgt.FuncAddr(), Simple_stub);
	//SET_STUB(P2ADDR(0x403920), Simple_stub);
	
	g_bInStub = FALSE;
	InvokeParentVirtualMethod();
	CHECK(TRUE == g_bInStub); // 也不能对父类的虚函数打桩
	
	CLR_ALL_STUB();
}

#endif