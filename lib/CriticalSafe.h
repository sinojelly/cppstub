
#ifndef _CRITICAL_SAFE_H_
#define _CRITICAL_SAFE_H_

/*
使用方法：
class User
{
CRiticalSection m_cs;

  void Function()
  {
  CRiticalSafe wait(m_cs);
  
	// Critical Section
	   }
	   }
*/

class CRiticalSection
{
	CRITICAL_SECTION m_cs;
public:
	CRiticalSection(const CRiticalSection &other)
	{
		if (&other != this)
		{
			m_cs = other.m_cs;
		}
	}
	const CRiticalSection &operator = (const CRiticalSection &other)
	{
		if (&other == this)
		{
			return *this;
		}
		m_cs = other.m_cs;
		return *this;
	}
	CRiticalSection()
	{
		InitializeCriticalSection(&m_cs);
	};
	~CRiticalSection()
	{
		DeleteCriticalSection(&m_cs);
	};
	operator CRITICAL_SECTION()
	{
		return m_cs;
	}
	operator CRITICAL_SECTION *()
	{
		return &m_cs;
	}
};

class CRiticalSafe
{
	CRiticalSection &m_cs;
public:
	CRiticalSafe(CRiticalSection &cs) : m_cs(cs)
	{
		EnterCriticalSection(m_cs);
	}
	~CRiticalSafe()
	{
		LeaveCriticalSection(m_cs);
	}
};

#endif