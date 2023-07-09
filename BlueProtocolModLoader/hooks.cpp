#include "hooks.h"
#include <string>
#include "GameInfo/GameInfo.h"
#include "UE4/Ue4.hpp"
bool GameStateClassInitNotRan = true;

namespace Hooks {

	namespace HookedFunctions
	{
		PVOID(*origProcessFunction)(UE4::UObject*, UE4::FFrame*, void* const);
		PVOID hookProcessFunction(UE4::UObject* obj, UE4::FFrame* Frame, void* const Result)
		{
			std::cout << "ProcessFunction" << std::endl;
			return origProcessFunction(obj, Frame, Result);

		}

		PVOID(*origInitGameState)(void*);
		PVOID hookInitGameState(void* Ret)
		{
			std::cout << "InitGameState" << std::endl;
			if (GameStateClassInitNotRan)
			{
				UE4::InitSDK();

				UE4::FTransform transform = UE4::FTransform::FTransform();
				UE4::FActorSpawnParameters spawnParams = UE4::FActorSpawnParameters::FActorSpawnParameters();
				if (GameProfile::Instance.StaticLoadObject)
				{
					std::cout << "StaticLoadObject Found" << std::endl;
				}
				else
				{
					std::cout << "StaticLoadObject Not Found" << std::endl;
				}
				GameStateClassInitNotRan = false;
			}
			return origInitGameState(Ret);
		}

		PVOID(*origBeginPlay)(UE4::AActor*);
		PVOID hookBeginPlay(UE4::AActor* Actor)
		{
			std::cout << "BeginPlay" << std::endl;
			return origBeginPlay(Actor);
		}

		PVOID(*origProcessEvent)(UE4::UObject*, UE4::UFunction*, void*);
		PVOID hookProcessEvent(UE4::UObject* obj, UE4::UFunction* function, void* parms)
		{
			std::cout << "ProcessEvent" << std::endl;
			return origProcessEvent(obj, function, parms);
		}
	};


	static void Init()
	{
		if (MH_Initialize() != MH_OK)
		{
			// TODO ERROR LOG
		}
	}
	template <typename T>
	static void Add(DWORD_PTR pTarget, LPVOID pDetour, T** ppOriginal, std::string displayName = "")
	{
		if (MH_CreateHook((LPVOID)pTarget, pDetour, reinterpret_cast<LPVOID*>(ppOriginal)) != MH_OK)
		{
			// TODO ERROR LOG
			return;
		}

		if (MH_EnableHook((LPVOID)pTarget) != MH_OK)
		{
			// TODO ERROR LOG

			return;
		}
	}


	DWORD __stdcall InitHooks(LPVOID)
	{
		std::cout << "Init" << std::endl;
		Init();
		// TODO LoadCoreMods
		Sleep(10);
		Add(GameProfile::Instance.GameStateInit, &HookedFunctions::hookInitGameState, &HookedFunctions::origInitGameState, "AGameModeBase::InitGameState");
		Add(GameProfile::Instance.BeginPlay, &HookedFunctions::hookBeginPlay, &HookedFunctions::origBeginPlay, "AActor::BeginPlay");
		Add(GameProfile::Instance.ProcessEvent, &HookedFunctions::hookProcessEvent, &HookedFunctions::origProcessEvent, "ProcessEvent");
		//LoaderUI::GetUI()->CreateUILogicThread();
		//if (!GameProfile::Instance.bDelayGUISpawn)
		//{
		//	LoaderUI::HookDX();
		//}
		return NULL;
	}

	void SetupHooks()
	{
		std::cout << "SetupHooks" << std::endl;
		CreateThread(0, 0, InitHooks, 0, 0, 0);
	}
}