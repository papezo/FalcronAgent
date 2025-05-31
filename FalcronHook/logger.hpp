#pragma once
#include <string>

using namespace std;

namespace Logger {
	void LogMessage(const string& message);
	void CleanUp();
}