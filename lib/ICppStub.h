#ifndef _CPP_STUB_INTERFACE_H_
#define _CPP_STUB_INTERFACE_H_

#include "IStub.h"
//#include "FastDelegate.h"
//using namespace fastdelegate;

/*
  ����������
  1 C����/��̬��Ա����/һ��⺯����������SET_STUB��׮��
    P2ADDR������Ա����ָ��ת��ΪDWORD���͵ĵ�ַ��
    ��ĳ�Ա����ͨ�������������ӵķ�����׮����Ŀǰֻ֧�ַ����Ա��
  2 ������CStub�������Ľ��д�׮�����׮�Ĳ������������Զ���ɡ���: CStub stub(P2ADDR(CSimple::SimpleMethod), Simple_stub);
  3 �⺯��VirtualProtect���ĸ��ٽ������⺯��[Initialize/Delete/Enter/Leave]CriticalSection������STUBģ���ڲ�ʹ�ò��ܱ���׮��
  4 C�ļ���ʹ��ʱ����IStub.h��CPP�ļ���ʹ��ʱ���԰������ܸ�ǿ��ICppStub.h��
  5 �ڰ���IStub.h/ICppStub.h֮ǰ������ʹ�õĲ��Կ�ܶ����USE_CPPUNIT/USE_UNITTESTCPP���Է���ļ��SET_STUB��׮�Ƿ�ɹ���
  6 Ĭ��ͬʱ���ڵ�׮��������Ϊ1000������ͨ��SET_STUB_NUM(num)���������ֵ�������ʱ����ԭ�е�׮ȫ�����
  7 ��ģ�������еĴ�����Ϣ�����ڿ�ִ���ļ�Ŀ¼�е�CppStub.log�С�
  8 ��ģ���ṩ�Ĵ�׮����Ϊ����׮������׮�õý��٣���֧�֡�
  9 ���صĺ����ĵ�ַ���ܷ���ĵõ������ܴ�׮��
  
  �ɸĽ��㣺
  1 Ŀǰ����֧���麯����׮��
  2 ��Ƕ���͵�����(+ / - / * / �µ�)��Ȼ�޷���׮���������͵Ĳ������������أ��Ƿ��ܴ�׮���Խ�һ���о���

  ���Ա������׮�����ӣ�
  ע�⣺��Ҫ��׮Ҳ��Ϊһ����ĳ�Ա���Һ�������ֵ/�����ĸ��������Ͷ���ԭ������һ����������ܳ��������쳣:
  i386/chkesp.c  line 42   the value of ESP was not properly saved across a function call. this is usually a
  result of calling a function declared with one calling convention with a function pointer declared with a different calling convention.
  �����ࣺ
  class CSimple
  {
  public:
     void SimpleMethod();
	 void ParamMethod(int a);		  
	 virtual void VirtualMethod();
	};
  д��׮�ࣺ������Ҫʵ�ִ���������з�����
   class CSimpleStub
   {
   public:
        void ParamSimple_stub(int a)
		{
		    g_bInStub = TRUE;
		}
	};
  ���Դ��룺
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
       CStub stub1(Simple, Simple_stub); // ��׮

       // ����
   }
*/

class CStub
{
public:
	template<class Src, class Stub>
	CStub(Src src, Stub stub)
	{
		//SET_STUB(src, stub); // ���ﲻ��ʹ�ò��Կ�ܵĺ�
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
		if (NULL == g_hApiHook) // ����֮ǰ������STUB��Դ����
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