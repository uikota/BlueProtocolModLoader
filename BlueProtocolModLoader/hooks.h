#pragma once
#include "MinHook.h"
#ifdef BlueProtocolModLoader_EXPORTS
#pragma comment(lib, "libMinHook-x64-v141-mtd.lib")
#endif

namespace Hooks
{
	void SetupHooks();
};