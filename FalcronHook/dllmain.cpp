// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <iostream>
#include <Minhook.h>

using namespace std;

#if _WIN64
#pragma comment(lib, "libMinHook.x64.lib")
#else
#pragma comment(lib, "libMinHook.x86.lib")
#endif

void CreateConsole()
{
    FreeConsole();

    if (AllocConsole())
    {
        FILE* file;
        freopen_s(&file, "CONOUT$", "w", stdout);
        freopen_s(&file, "CONOUT$", "w", stderr);
        freopen_s(&file, "CONOUT$", "w", stdin);

		cout << "Console has been allocated.." << endl;
    }
}
DWORD MainFunction(LPVOID lpParam)
{
    CreateConsole();
    Sleep(500000);

    return 0;

}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, MainFunction, NULL, 0, NULL);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        FreeConsole();
        break;
    }
    return TRUE;
}

