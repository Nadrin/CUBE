/* CUBE demo toolkit by MasterM/Asenses */

#include <stdafx.h>
#include <core/system.h>
#include <utils/notify.h>
#include <utils/lock.h>

using namespace CUBE;

FileNotify::FileNotify(const std::string& path, unsigned long delay) : eventDelay(delay)
{
	char buffer[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, buffer);
	basePath = std::string(buffer) + "\\" + path + "\\";

	hEventHandle   = CreateEvent(NULL, TRUE, FALSE, NULL);
	eventTimestamp = 0; // Not technically correct but good enough.
	GetSystemTimeAsFileTime(&processTimestamp);

	hNotifyHandle = FindFirstChangeNotificationA(basePath.c_str(), TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE);
	if(INVALID_HANDLE_VALUE == hNotifyHandle) {
		CloseHandle(hEventHandle);
		throw std::runtime_error("FileNotify: FindFirstChangeNotification failed.");
	}

	if(!RegisterWaitForSingleObject(&hWaitHandle, hNotifyHandle, FileNotify::NotifyWaitCallback, (PVOID)this, INFINITE, WT_EXECUTEDEFAULT)) {
		FindCloseChangeNotification(hNotifyHandle);
		CloseHandle(hEventHandle);
		throw std::runtime_error("FileNotify: RegisterWaitForSingleObject failed.");
	}

	Core::System::Instance()->Log("Registered file notification handler for: %s\n", path.c_str());
}

FileNotify::~FileNotify()
{
	UnregisterWait(hWaitHandle);
	FindCloseChangeNotification(hNotifyHandle);
	CloseHandle(hEventHandle);
}

void CALLBACK FileNotify::NotifyWaitCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
	FileNotify* _this = (FileNotify*)lpParameter;
	_this->eventTimestamp = GetTickCount64();
	SetEvent(_this->hEventHandle);
	FindNextChangeNotification(_this->hNotifyHandle);
}

FILETIME FileNotify::GetFileModificationTime(const std::string& name)
{
	const std::string fullPath = basePath + name;
	FILETIME modTime;

	HANDLE hFile = CreateFileA(fullPath.c_str(), 
		GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
	if(INVALID_HANDLE_VALUE == hFile) {
		throw std::runtime_error("FileNotify: Cannot open file for reading.");
	}

	if(!GetFileTime(hFile, NULL, NULL, &modTime)) {
		CloseHandle(hFile);
		throw std::runtime_error("FileNotify: Cannot get file modification time.");
	}

	CloseHandle(hFile);
	return modTime;
}

bool FileNotify::RegisterHandler(const std::string& name, FileNotify::Handler* handler)
{
	auto it = fileMap.find(name);
	if(it != fileMap.end()) {
		return false;
	}

	fileMap[name] = handler;
	return true;
}

bool FileNotify::UnregisterHandler(const std::string& name)
{
	auto it = fileMap.find(name);
	if(it == fileMap.end()) {
		return false;
	}

	fileMap.erase(it);
	return true;
}

void FileNotify::ProcessEvents()
{
	if(WaitForSingleObject(hEventHandle, 0) != WAIT_OBJECT_0)
		return;
	if((GetTickCount64() - eventTimestamp) < eventDelay)
		return;

	for(auto it=fileMap.begin(); it!=fileMap.end(); ++it) {
		FILETIME timestamp = GetFileModificationTime(it->first);
		if(CompareFileTime(&timestamp, &processTimestamp) == 1)
			it->second->operator()(it->first);
	}

	GetSystemTimeAsFileTime(&processTimestamp);
	ResetEvent(hEventHandle);
}