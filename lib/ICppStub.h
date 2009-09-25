#ifndef _CPP_STUB_INTERFACE_H_
#define _CPP_STUB_INTERFACE_H_

#include "IStub.h"
//#include "FastDelegate.h"
//using namespace fastdelegate;

/*
  功能描述：
  1 C函数/静态成员函数/一般库函数均可以用SET_STUB打桩。
    P2ADDR宏把类成员函数指针转换为DWORD类型的地址，
    类的成员函数通过类似下面例子的方法打桩。（目前只支持非虚成员）
  2 可以用CStub类更方便的进行打桩，清除桩的操作在析构中自动完成。如: CStub stub(P2ADDR(CSimple::SimpleMethod), Simple_stub);
  3 库函数VirtualProtect和四个临界区互斥函数[Initialize/Delete/Enter/Leave]CriticalSection由于在STUB模块内部使用不能被打桩。
  4 C文件中使用时包含IStub.h，CPP文件中使用时可以包含功能更强的ICppStub.h。
  5 在包含IStub.h/ICppStub.h之前，根据使用的测试框架定义宏USE_CPPUNIT/USE_UNITTESTCPP可以方便的检查SET_STUB打桩是否成功。
  6 默认同时存在的桩函数个数为1000，可以通过SET_STUB_NUM(num)宏重设这个值，重设的时候会把原有的桩全清除。
  7 本模块运行中的错误信息保存在可执行文件目录中的CppStub.log中。
  8 本模块提供的打桩功能为覆盖桩。插入桩用得较少，不支持。
  9 重载的函数的地址不能方便的得到，不能打桩。
  
  可改进点：
  1 目前不能支持虚函数打桩。
  2 内嵌类型的运算(+ / - / * / ÷等)必然无法打桩，其它类型的操作符可以重载，是否能打桩可以进一步研究。

  类成员函数打桩的例子：
  注意：需要把桩也作为一个类的成员。且函数返回值/参数的个数和类型都与原来函数一样，否则可能出现下面异常:
  i386/chkesp.c  line 42   the value of ESP was not properly saved across a function call. this is usually a
  result of calling a function declared with one calling convention with a function pointer declared with a different calling convention.
  待测类：
  class CSimple
  {
  public:
     void SimpleMethod();
	 void ParamMethod(int a);		  
	 virtual void VirtualMethod();
	};
  写的桩类：（不需要实现待测类的所有方法）
   class CSimpleStub
   {
   public:
        void ParamSimple_stub(int a)
		{
		    g_bInStub = TRUE;
		}
	};
  测试代码：
  TEST(TestParamMethod)
  {
      CStub stub(P2ADDR(CSimple::ParamMethod), P2ADDR(CSimpleStub::ParamSimple_stub));
  
	  g_bInStub = FALSE;
	  InvokeParamMethod();
	  CHECK(TRUE == g_bInStub);
	}
*/

extern "C" void STUB_Log(char *file, int line, char *format, ...);

/*
   demo:
   TestFunc()
   {
       CStub stub1(Simple, Simple_stub); // 打桩

       // 代码
   }
*/

class CStub
{
public:
	template<class Src, class Stub>
	CStub(Src src, Stub stub)
	{
		//SET_STUB(src, stub); // 这里不能使用测试框架的宏
		if (-1 == STUB_SetByAddr(g_hApiHook, (DWORD)src, (DWORD)stub))
		{
			STUB_Log(__FILE__, __LINE__, "Set stub fail in CStub ctor. g_hApiHook = %#x.", g_hApiHook);
			//fprintf(stdout, "(ICppStub.h, %d) Set stub fail in CStub ctor. g_hApiHook = %#x.\r\n", __LINE__, g_hApiHook);
		}
		m_dwSrcFunc = (DWORD)src;
	}
	~CStub()
	{
		//CLR_STUB(m_dwSrcFunc);
		if (NULL == g_hApiHook) // 析构之前重设了STUB资源个数
		{
			return;
		}
		if (-1 == STUB_UnSetByAddr(g_hApiHook, (DWORD)m_dwSrcFunc))
		{
			STUB_Log(__FILE__, __LINE__, "UnSet stub fail in CStub dtor. g_hApiHook = %#x.", g_hApiHook);
		}
	}
private:
	DWORD m_dwSrcFunc;	
};

template<class MethodPointer>
inline DWORD STUB_MethodAddr(MethodPointer p)
{
	DWORD dwAddr;
	char szBuf[20];
	sprintf(szBuf, "%d", p); // CSimple::SimpleMethod
	sscanf(szBuf, "%d", &dwAddr);
	return dwAddr;
}

// Pointer to address
#define P2ADDR(MethodPointer)   STUB_MethodAddr(MethodPointer)

#endif