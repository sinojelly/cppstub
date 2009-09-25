#include <iostream.h>
#include "ICppStub.h"

#pragma comment(lib, "..\\lib\\Debug\\CppStub.lib")

void Test()
{
	cout<<"In Test \n";
}

void Test_stub()
{
	cout<<"In Test_stub \n";
}

int memcpy_stub()
{
	cout << "in memcpy_stub \n";
	return 0;
}

int main()
{
	cout << "Before SET_STUB: \n";
	Test();

	cout << "\nSET_STUB ... \n";
	SET_STUB(Test, Test_stub);
	SET_STUB(memcpy, memcpy_stub);

	cout << "\nAfter SET_STUB: \n";
	Test();
	memcpy((void *)0, (const void *)0, 0);

	// ÕâÀï

	cout << "\nCLR_ALL_STUB ... \n";
	CLR_ALL_STUB();

	cout << "\nAfter CLR_ALL_STUB: \n";
	Test();
	//memcpy((void *)0, (const void *)0, 0);

	return 0;
}

