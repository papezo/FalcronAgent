// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <iostream>
#include "C:/Users/papez/source/repos/TeverusAgent/minhook/include/MinHook.h"
#include "logger.hpp"


#if _WIN64
#pragma comment(lib, "libMinHook.x64.lib")
#else
#pragma comment(lib, "libMinHook.x86.lib")
#endif

typedef DWORD(NTAPI* pNtAllocateVirtualmemory)
(
    HANDLE ProcessHandle,
    PVOID* BaseAddress,
    ULONG_PTR ZeroBits,
	PSIZE_T RegionSize,
	ULONG AllocationType,
	ULONG Protect
);

typedef DWORD(WINAPI* pNtProtectVirtualMemory)
(
    HANDLE ProcessHandle,
    PVOID* BaseAddress,
    PSIZE_T RegionSize,
    ULONG NewProtect,
    PULONG OldProtect
);

pNtAllocateVirtualmemory pOriginalNtAllocateVirtualMemory = nullptr;
pNtProtectVirtualMemory pOriginalNtProtectVirtualMemory = nullptr;

DWORD NTAPI HookedNtAllocateVirtualMemory(
    HANDLE ProcessHandle,
    PVOID* BaseAddress,
	ULONG_PTR ZeroBits,
    PSIZE_T RegionSize,
	ULONG AllocationType,
	ULONG Protect) {

    if (Protect == PAGE_EXECUTE_READWRITE) {
        std::cout << "PAGE_EXECUTE_READWRITE permission detected in NtAllocateVirtualMemory function" << std::endl;
        // if protections enabled then close the process, prevent the call and so on.,.
    }

    return pOriginalNtAllocateVirtualMemory(
        ProcessHandle,
        BaseAddress,
        ZeroBits,
        RegionSize,
        AllocationType,
        Protect);
}

DWORD NTAPI HookedNtProtectVirtualMemory(
    HANDLE ProcessHandle,
    PVOID* BaseAddress,
    PSIZE_T RegionSize,
    ULONG NewProtect,
    PULONG OldProtect) {

    if (NewProtect == PAGE_EXECUTE_READWRITE) {
        Logger::LogMessage("PAGE_EXECUTE_READWRITE permission detected in NtAllocateVirtualMemory function");
        // if protections enabled then close the process, prevent the call and so on.,.
    }

    return pOriginalNtProtectVirtualMemory(
        ProcessHandle,
        BaseAddress,
        RegionSize,
        NewProtect,
        OldProtect);
}

void InitHooks()
{
	MH_STATUS status = MH_Initialize();

    if (status != MH_OK) {
        Logger::LogMessage("Failed to initialize MinHook: " + status);
        return;
	}

    if (MH_CreateHookApi(L"ntdll", "NtProtectVirtualMemory", &HookedNtProtectVirtualMemory, (LPVOID*)&pOriginalNtAllocateVirtualMemory) != MH_OK)
    {
        Logger::LogMessage("Failed to create hook NtProtectVirtualMemory: " + status);
    }

    if (MH_CreateHookApi(L"ntdll", "HookedNtAllocateVirtualMemory", &HookedNtAllocateVirtualMemory, (LPVOID*)&pOriginalNtAllocateVirtualMemory) != MH_OK)
    {
        Logger::LogMessage("Failed to create hook NtProtectVirtualMemory: " + status);
    }

    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
    {
        Logger::LogMessage("Failed to enable hooks: ");
    }

    Logger::LogMessage("Hooks initialized successfully.");
}

DWORD MainFunction(LPVOID lpParam)
{
    InitHooks();
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
        Logger::LogMessage("Injected into process");
        MainFunction(NULL);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

