
#include <UnitTest++.h>

#pragma comment(lib, "UnitTest++.lib")

// 使用CppStub的工程必须去掉Link->General->Link incrementally前面的对钩，否则会出现异常
#pragma comment(lib, "..\\lib\\Debug\\CppStub.lib")


int main(int argc, char **argv)
{
// #ifdef __COVERAGESCANNER__
//     __coveragescanner_install(argv[0]);
// #endif
	return UnitTest::RunAllTests();
}