#pragma once

namespace CUBE {

class FileNotify
{
public:
	struct Handler {
		virtual void operator()() = 0;
	};

protected:
	HANDLE hDirectoryHandle;
	HANDLE hNotifyHandle;
	HANDLE hWaitHandle;
	CRITICAL_SECTION hLock;

	std::map<std::string, ULONGLONG> pendingEvents;
	std::map<std::string, FileNotify::Handler*> eventHandlers;
protected:
	static void CALLBACK NotifyWaitCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
	bool PushEvent(const std::string& name);
public:
	FileNotify(const std::string& path);
	~FileNotify();

	bool RegisterHandler(const std::string& name, FileNotify::Handler* handler);
	bool UnregisterHandler(const std::string& name);
	void ProcessEvents();
};

} // CUBE