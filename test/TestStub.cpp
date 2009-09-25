#include <UnitTest++.h>
#include <windows.h>
#include "..\lib\IStub.h"
#include "TestFunction.h"

TEST(TestSimpleStub)
{
	g_bInStub = TRUE;
	InvokeSimple();
	CHECK(g_bInStub == FALSE);

	SET_STUB(Simple, Simple_stub);

	g_bInStub = FALSE;
	InvokeSimple();
	CHECK(g_bInStub == TRUE);

	CLR_STUB(Simple);

	g_bInStub = TRUE;
	InvokeSimple();
	CHECK(g_bInStub == FALSE);
}

TEST(Test2SimpleStub)
{
	g_bInStub = TRUE;
	g_bInStub2 = TRUE;
	Invoke2Simples();
	CHECK(g_bInStub == FALSE);
	CHECK(g_bInStub2 == FALSE);
	
	SET_STUB(Simple, Simple_stub);

	g_bInStub = FALSE;
	g_bInStub2 = TRUE;
	Invoke2Simples();
	CHECK(g_bInStub == TRUE);
	CHECK(g_bInStub2 == FALSE);

	SET_STUB(Simple2, Simple2_stub);
	
	g_bInStub = FALSE;
	g_bInStub2 = FALSE;
	Invoke2Simples();
	CHECK(g_bInStub == TRUE);
	CHECK(g_bInStub2 == TRUE);  ////
	
	CLR_STUB(Simple2);
	
	g_bInStub = FALSE;
	g_bInStub2 = TRUE;
	Invoke2Simples();
	CHECK(g_bInStub == TRUE);
	CHECK(g_bInStub2 == FALSE);

	CLR_STUB(Simple);

	g_bInStub = TRUE;
	g_bInStub2 = TRUE;
	Invoke2Simples();
	CHECK(g_bInStub == FALSE);
	CHECK(g_bInStub2 == FALSE);
}


TEST(TestClearAll)
{
	g_bInStub = TRUE;
	g_bInStub2 = TRUE;
	Invoke2Simples();
	CHECK(g_bInStub == FALSE);
	CHECK(g_bInStub2 == FALSE);
	
	SET_STUB(Simple, Simple_stub);
	SET_STUB(Simple2, Simple2_stub);
	
	g_bInStub = FALSE;
	g_bInStub2 = FALSE;
	Invoke2Simples();
	CHECK(g_bInStub == TRUE);
	CHECK(g_bInStub2 == TRUE); /////

	CLR_ALL_STUB();

	g_bInStub = TRUE;
	g_bInStub2 = TRUE;
	Invoke2Simples();
	CHECK(g_bInStub == FALSE);
	CHECK(g_bInStub2 == FALSE);
}

TEST(TestMemCpy)
{
	g_bInStub = FALSE;
	SET_STUB(memcpy, memcpy_stub);
	InvokeMemCpy();
	CHECK(g_bInStub == TRUE);

	CLR_STUB(memcpy);
}

TEST(TestClearOneWhenNone)
{
	CLR_STUB(memcpy);
	// 应该不会出现异常
}

TEST(TestClearAllWhenNone)
{
	CLR_ALL_STUB();
}

TEST(TestBorder)
{
	g_bInStub = TRUE;
	g_bInStub2 = TRUE;
	g_bInStub3 = TRUE;
	Invoke3Simples();
	CHECK(FALSE == g_bInStub);
	CHECK(FALSE == g_bInStub2);
	CHECK(FALSE == g_bInStub3);

	SET_STUB_NUM(2);
	SET_STUB(Simple, Simple_stub);
	SET_STUB(Simple2, Simple2_stub);
	CHECK(-1 == STUB_SetByAddr(g_hApiHook, (DWORD)Simple3, (DWORD)Simple3_stub));

	g_bInStub = FALSE;
	g_bInStub2 = FALSE;
	g_bInStub3 = TRUE;
	Invoke3Simples();
	CHECK(TRUE  == g_bInStub);
	CHECK(TRUE  == g_bInStub2);
	CHECK(FALSE == g_bInStub3);

	CLR_STUB(Simple2);
	g_bInStub2 = TRUE;
	Invoke3Simples();
	CHECK(FALSE  == g_bInStub2);

	CHECK(-1 != STUB_SetByAddr(g_hApiHook, (DWORD)Simple3, (DWORD)Simple3_stub)); // 删除的资源能重新利用
	g_bInStub = FALSE;
	g_bInStub2 = TRUE;
	g_bInStub3 = FALSE;
	Invoke3Simples();
	CHECK(TRUE  == g_bInStub);
	CHECK(FALSE  == g_bInStub2);
	CHECK(TRUE  == g_bInStub3);

	CLR_ALL_STUB();
	SET_STUB(Simple2, Simple2_stub);	
	SET_STUB(memcpy, memcpy_stub);
	g_bInStub2 = FALSE;
	g_bInStub = FALSE;
	Invoke3Simples();
	InvokeMemCpy();
	CHECK(g_bInStub == TRUE);
	CHECK(g_bInStub2 == TRUE);

	CLR_ALL_STUB();
}

TEST(TestModify)
{
	SET_STUB_NUM(2);

	g_bInStub = TRUE;
	g_bInStub2 = TRUE;
	Invoke3Simples();
	CHECK(FALSE  == g_bInStub);
	CHECK(FALSE  == g_bInStub2);

	SET_STUB(Simple3, Simple_stub);

	g_bInStub = FALSE;
	g_bInStub2 = TRUE;
	Invoke3Simples();
	CHECK(TRUE  == g_bInStub);
	CHECK(FALSE  == g_bInStub2);

	SET_STUB(Simple3, Simple2_stub);

	g_bInStub = FALSE;
	g_bInStub2 = FALSE;
	Invoke3Simples();
	CHECK(FALSE  == g_bInStub);
	CHECK(TRUE  == g_bInStub2);

	CHECK(-1 != STUB_SetByAddr(g_hApiHook, (DWORD)Simple, (DWORD)Simple_stub));
	CHECK(-1 != STUB_SetByAddr(g_hApiHook, (DWORD)Simple, (DWORD)Simple2_stub));
	CHECK(-1 == STUB_SetByAddr(g_hApiHook, (DWORD)Simple2, (DWORD)Simple2_stub));

	CLR_ALL_STUB();
}