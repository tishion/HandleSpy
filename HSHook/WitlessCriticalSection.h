#pragma once

class CWitlessCriticalSection
{
public:
	CWitlessCriticalSection(void);
	~CWitlessCriticalSection(void);

	void Enter();
	void Leave();

private:
	volatile long m_lLockCount;
	unsigned long m_lOwninThread;
	unsigned long m_lRecursionCount;
};
