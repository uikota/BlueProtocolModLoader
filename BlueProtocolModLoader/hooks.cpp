#include "hooks.h"
#include <string>
#include "GameInfo/GameInfo.h"
#include "UE4/Ue4.hpp"
#include "spdlog/spdlog.h"

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
			//auto Class = Actor->GetClass();
			//if (Class != nullptr && (
			//	Class->GetName() == "SBEnemyMetaInfo" ||
			//	Class->GetName() == "BP_Foxhound_C" ||
			//	Class->GetName() == "BP_Weapon_FishingRod_C" ||
			//	Class->GetName() == "BP_Weapon_HandAxe_C" ||
			//	Class->GetName() == "BP_Weapon_BallHammer_C" ||
			//	Class->GetName() == "BP_Smasher_C" ||
			//	Class->GetName() == "BP_Berserker_C" ||
			//	Class->GetName() == "BP_Weapon_HandSword_C" ||
			//	Class->GetName() == "BP_Weapon_Shield_C" ||
			//	Class->GetName() == "SBProjectile_Parabola" ||
			//	Class->GetName() == "BP_Magician_C" ||
			//	Class->GetName() == "SBProjectile_Fix" ||
			//	Class->GetName() == "BP_NonActive_Urabo_C" ||
			//	Class->GetName() == "SBPlayerState" ||
			//	Class->GetName() == "SBGameState" ||
			//	Class->GetName() == "SBPlayerStart" ||
			//	Class->GetName() == "BP_BattleHUD_C" &&
			//	Class->GetName() == "SBWorldSettings")
			//	) {
			//	std::cout << "BeginPlay" << std::endl;
			//	std::cout << Actor->GetFullName() << std::endl;
			//	spdlog::info("BeginPlay=>{}", Actor->GetFullName());

			//	// Show Functions
			//	if (Actor->IsChunkedArray())
			//	{
			//		for (int i = 0; i < Actor->GObjects->GetAsChunckArray().Num(); ++i)
			//		{
			//			auto object = Actor->GObjects->GetAsChunckArray().GetByIndex(i).Object;

			//			if (object == nullptr)
			//			{
			//				continue;
			//			}
			//			if (object->GetOuter() == Actor->GetClass())
			//			{
			//				spdlog::info("Function1=>{}", object->GetName());
			//			}
			//		}
			//	}
			//	else
			//	{
			//		for (int i = 0; i < Actor->GObjects->GetAsTUArray().Num(); ++i)
			//		{
			//			auto object = Actor->GObjects->GetAsTUArray().GetByIndex(i).Object;

			//			if (object == nullptr)
			//			{
			//				continue;
			//			}
			//			if (object->GetOuter() == Actor->GetClass())
			//			{
			//				spdlog::info("Function2=>{}", object->GetName());
			//			}
			//		}
			//	}
			//}

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
			spdlog::error("Failed to initialize MinHook");
		}
	}
	template <typename T>
	static void Add(DWORD_PTR pTarget, LPVOID pDetour, T** ppOriginal, std::string displayName = "")
	{
		if (MH_CreateHook((LPVOID)pTarget, pDetour, reinterpret_cast<LPVOID*>(ppOriginal)) != MH_OK)
		{
			spdlog::error("Failed to create hook: {}", displayName.c_str());
			return;
		}

		if (MH_EnableHook((LPVOID)pTarget) != MH_OK)
		{
			spdlog::error("Failed to enable hook: {}", displayName.c_str());
			return;
		}
	}


	DWORD __stdcall InitHooks(LPVOID)
	{
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
		spdlog::info("SetupHooks");
		CreateThread(0, 0, InitHooks, 0, 0, 0);
	}
}
