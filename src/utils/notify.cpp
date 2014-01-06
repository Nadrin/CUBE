#include <stdafx.h>
#include <core/global.h>
#include <utils/notify.h>
#include <utils/lock.h>

using namespace CUBE;

FileNotify::FileNotify(const std::string& path)
{
	char buffer[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, buffer);
	strcat_s<MAX_PATH>(buffer, path.c_str());

	InitializeCriticalSection(&hLock);

	hDirectoryHandle = CreateFileA(buffer, FILE_LIST_DIRECTORY, 
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if(INVALID_HANDLE_VALUE == hDirectoryHandle)
		throw std::runtime_error("FileNotfy: Cannot open directory.");

	hNotifyHandle = FindFirstChangeNotificationA(buffer, TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE);
	if(INVALID_HANDLE_VALUE == hNotifyHandle) {
		CloseHandle(hDirectoryHandle);
		throw std::runtime_error("FileNotify: FindFirstChangeNotification failed.");
	}

	if(!RegisterWaitForSingleObject(&hWaitHandle, hNotifyHandle, FileNotify::NotifyWaitCallback, (PVOID)this, INFINITE, WT_EXECUTEDEFAULT)) {
		FindCloseChangeNotification(hNotifyHandle);
		CloseHandle(hDirectoryHandle);
		throw std::runtime_error("FileNotify: RegisterWaitForSingleObject failed.");
	}
}

FileNotify::~FileNotify()
{
	UnregisterWait(hWaitHandle);
	FindCloseChangeNotification(hNotifyHandle);
	CloseHandle(hDirectoryHandle);
	DeleteCriticalSection(&hLock);
}

void CALLBACK FileNotify::NotifyWaitCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
	FileNotify* _this = (FileNotify*)lpParameter;
	FILE_NOTIFY_INFORMATION notifyInfo[2];
	DWORD dwBytesReturned = 0;

	if(ReadDirectoryChangesW(_this->hDirectoryHandle, &notifyInfo, sizeof(notifyInfo),
		TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE, &dwBytesReturned, NULL, NULL)) 
	{
		if(dwBytesReturned > 0) {
			wchar_t wFileName[MAX_PATH];
			std::memset(wFileName, 0, sizeof(wFileName));
			std::memcpy(wFileName, notifyInfo[0].FileName, notifyInfo[0].FileNameLength);

			char fileName[MAX_PATH];
			std::wcstombs(fileName, wFileName, MAX_PATH-1);
			_this->PushEvent(std::string(fileName));
		}
	}

	FindNextChangeNotification(_this->hNotifyHandle);
}

bool FileNotify::PushEvent(const std::string& name)
{
	LockSection lock(hLock);

	auto it = eventHandlers.find(name);
	if(it != eventHandlers.end()) {
		pendingEvents[name] = GetTickCount64();
		return true;
	}
	return false;
}

bool FileNotify::RegisterHandler(const std::string& name, FileNotify::Handler* handler)
{
	LockSection lock(hLock);

	auto it = eventHandlers.find(name);
	if(it != eventHandlers.end())
		return false;

	eventHandlers[name] = handler;
	return true;
}

bool FileNotify::UnregisterHandler(const std::string& name)
{
	LockSection lock(hLock);

	{
		auto it = eventHandlers.find(name);
		if(it == eventHandlers.end())
			return false;
		eventHandlers.erase(it);
	}
	{
		auto it = pendingEvents.find(name);
		if(it != pendingEvents.end())
			pendingEvents.erase(it);
	}
	return true;
}

void FileNotify::ProcessEvents()
{
	LockSection lock(hLock);

	const ULONGLONG time = GetTickCount64();
	for(auto it=pendingEvents.begin(); it!=pendingEvents.end();) {
		if((time - it->second) > 1000) {
			eventHandlers[it->first]->operator()();
			pendingEvents.erase(it++);
		}
		else ++it;
	}
}