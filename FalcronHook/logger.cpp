#include "pch.h"
#include "logger.hpp"
#include <Windows.h>
#include <queue>
#include <string>

using namespace std;
namespace Logger {
	HANDLE hPipe = INVALID_HANDLE_VALUE;
	string processName;
	DWORD processId = 0;	


	void GetCurrentProcessInfo(string& processName, DWORD& processId)
	{
		processId = GetCurrentProcessId();
		char processPath[MAX_PATH];

		if (GetModuleFileNameA(NULL, processPath, MAX_PATH) > 0)
		{
			char* baseName = strrchr(processPath, '\\');
			if (baseName != NULL)
			{
				processName = string(baseName + 1);
			}
			else {
				processName = string(processPath);
			}
		}
		else {
			processName = "Unknown Process";
		}
	}

	bool OpenPipeConnection()
	{
		if(hPipe == INVALID_HANDLE_VALUE) {
			hPipe = CreateFile(TEXT("\\\\.\\pipe\TeverusPipe"),
				GENERIC_WRITE,
				0, NULL, OPEN_EXISTING, 0, NULL);

			if(hPipe == INVALID_HANDLE_VALUE) {
				return false;
			}
		}
		return true;
	}

	void LogMessage(const string& message)
	{
		if (processId == 0)
		{
			GetCurrentProcessInfo(processName, processId);
		}

		DWORD dwWritten;

		if (!OpenPipeConnection())
		{
			return;
		}

		string logMessage = "PRoccess name: " + processName + " (PID: " + to_string(processId) + "): " + message + "\n";

		if (!WriteFile(hPipe, logMessage.c_str(), logMessage.length(), &dwWritten, NULL))
		{
			CloseHandle(hPipe);
			hPipe = INVALID_HANDLE_VALUE;
		}
	}


	void CleanUp()
	{
		if (hPipe != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hPipe);
		}
	}
}