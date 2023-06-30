// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "DInput8.h"
#include <codecvt>
#include <string>
#include <filesystem>

void Init()
{
	// Load the original dinput8.dll from the system directory
	char DInputDllName[MAX_PATH];
	GetSystemDirectoryA(DInputDllName, MAX_PATH);
	strcat_s(DInputDllName, "\\dinput8.dll");
	DInput8DLL = LoadLibraryA(DInputDllName);
	if (DInput8DLL > (HMODULE)31)
	{
		OriginalFunction = (DirectInput8Create_t)GetProcAddress(DInput8DLL, "DirectInput8Create");
	}
}

void LoadMods() {
	std::string modsPath = "./plugins";

	CreateDirectoryA(modsPath.c_str(), NULL);
	printf("Create Mods Directory [%s]", modsPath.c_str());

	printf("Start Loading Plugins.");
	std::string fileName = "\\*.dll";
	std::string searchName = modsPath + fileName;

	WIN32_FIND_DATAA  fd;
	HANDLE find = FindFirstFileA(searchName.c_str(), &fd);

	if (find != INVALID_HANDLE_VALUE) {
		do {
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				std::string dllPath = modsPath + "\\" + fd.cFileName;
				std::wstring wideDllPath(dllPath.begin(), dllPath.end());
				HMODULE hModule = LoadLibraryW(wideDllPath.c_str());
				if (hModule != NULL) {
					printf("Loaded [%s]", modsPath.c_str());
				}
				else {
					printf("Failed [%s]", modsPath.c_str());
				}
			}
		} while (FindNextFileA(find, &fd));
	}
	printf("End Loading Plugins.");

	FindClose(find);
}

BOOL APIENTRY DllMain(HMODULE Module,
	DWORD  ReasonForCall,
	LPVOID Reserved)
{
	switch (ReasonForCall)
	{
	case DLL_PROCESS_ATTACH:
		Init();
		LoadMods();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

