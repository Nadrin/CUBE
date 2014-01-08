/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

namespace CUBE {
 
class Lock {};

class LockSection : public Lock
{
private:
	CRITICAL_SECTION hHandle;

public:
	LockSection(CRITICAL_SECTION hSection) : hHandle(hSection)
	{
		EnterCriticalSection(&hHandle);
	}
	~LockSection()
	{
		LeaveCriticalSection(&hHandle);
	}
};

class LockMutex : public Lock
{
private:
	HANDLE hHandle;

public:
	LockMutex(HANDLE hMutex) : hHandle(hMutex)
	{
		if(WaitForSingleObject(hHandle, INFINITE) != WAIT_OBJECT_0)
			throw std::runtime_error("Failed to lock mutex.");
	}
	~LockMutex()
	{
		ReleaseMutex(hHandle);
	}
};

} // CUBE