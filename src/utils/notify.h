/* CUBE demo toolkit by MasterM/Asenses */

#pragma once

namespace CUBE {

class FileNotify
{
public:
	struct Handler {
		virtual void operator()() = 0;
	};
protected:
	HANDLE hNotifyHandle;
	HANDLE hWaitHandle;
	HANDLE hEventHandle;

	std::string basePath;
	ULONGLONG eventTimestamp;
	unsigned long eventDelay;
	FILETIME processTimestamp;

	std::map<std::string, FileNotify::Handler*> fileMap;
protected:
	static void CALLBACK NotifyWaitCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
	FILETIME GetFileModificationTime(const std::string& name);
public:
	FileNotify(const std::string& path, unsigned long delay=100);
	~FileNotify();

	bool RegisterHandler(const std::string& name, FileNotify::Handler* handler);
	bool UnregisterHandler(const std::string& name);
	void ProcessEvents();
};

} // CUBE