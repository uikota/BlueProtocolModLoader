#include "GameInfo.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <sstream>
#include "../Utilities/Pattern.h"
#include "spdlog/spdlog.h"

GameProfile GameProfile::Instance;

DWORD StringToDWord(std::string str)
{
	unsigned int m_dwIP;

	std::istringstream ss(&str[2]);
	ss >> std::hex >> m_dwIP;
	return m_dwIP;
}

void GameProfile::SetupProfile()
{
	GameProfile::Instance.ProfileName = "BLUEPROTOCOL-Win64-Shipping";

	GameProfile::Instance.UsesFNamePool = 1;
	GameProfile::Instance.IsUsingFChunkedFixedUObjectArray = 1;
	GameProfile::Instance.IsUsingDeferedSpawn = 0;
	GameProfile::Instance.bIsDefaultObjectArrayed = 0;

	GameProfile::Instance.IsUsing4_22 = 0;
	GameProfile::Instance.bDelayGUISpawn = 1;

	GameProfile::Instance.IsGInfoPatterns = 0;
	GameProfile::Instance.BeginPlayOverwrite = "";


	GameProfile::Instance.GName = (DWORD64)GetModuleHandleW(0) + StringToDWord("0x71d7640");
	GameProfile::Instance.GObject = (DWORD64)GetModuleHandleW(0) + StringToDWord("0x7213970");
	GameProfile::Instance.GWorld = (DWORD64)GetModuleHandleW(0) + StringToDWord("0x73558d0");


	GameProfile::Instance.IsPropertyMissing = true;
	GameProfile::Instance.IsFunctionPatterns = 0;


	GameProfile::Instance.GameStateInit = (DWORD64)GetModuleHandleW(0) + StringToDWord("0x3cb1d10");
	GameProfile::Instance.BeginPlay = (DWORD64)GetModuleHandleW(0) + StringToDWord("0x3a19520");
	GameProfile::Instance.StaticLoadObject = (DWORD64)GetModuleHandleW(0) + StringToDWord("0x25f4c60");
	GameProfile::Instance.SpawnActorFTrans = (DWORD64)GetModuleHandleW(0) + StringToDWord("0x3d72610");
	GameProfile::Instance.CallFunctionByNameWithArguments = (DWORD64)GetModuleHandleW(0) + StringToDWord("0x25de100");
	GameProfile::Instance.ProcessEvent = (DWORD64)GetModuleHandleW(0) + StringToDWord("0x18621a0");
	GameProfile::Instance.CreateDefaultObject = (DWORD64)GetModuleHandleW(0) + StringToDWord("");//Missing
	//Output File Initialization

	if (GameProfile::Instance.UsesFNamePool || GameProfile::Instance.IsUsing4_22)
	{
		DWORD64 ProcessAddy = (DWORD64)Pattern::Find("41 F6 C7 02 74 ? 4C 8B C7 48 8B ? ? 8B ? E8");
		if (ProcessAddy)
		{
			auto ProcessAddyOffset = *reinterpret_cast<uint32_t*>(ProcessAddy + 16);
			GameProfile::Instance.ProcessInternals = (ProcessAddy + 20 + ProcessAddyOffset);
			spdlog::info("ProcessInternalFunction: {}", (void*)GameProfile::Instance.ProcessInternals);
		}
	}

	auto StaticConstructObject_Internal = Pattern::Find("48 8B 84 24 ?? ?? 00 00 48 89 44 24 ?? C7 44 24 ?? 00 00 00 00 E8"); // Sig 1
	if (StaticConstructObject_Internal != nullptr)
	{
		StaticConstructObject_Internal += 0x15;
	}
	else
	{
		StaticConstructObject_Internal = Pattern::Find("48 8B C8 89 7C 24 ?? E8");
		if (StaticConstructObject_Internal != nullptr)
		{
			StaticConstructObject_Internal += 0x7;
		}
		else
		{
			GameProfile::Instance.IsUsingUpdatedStaticConstruct = true;
			StaticConstructObject_Internal = Pattern::Find("E8 ? ? ? ? 45 8B 47 70");
			if (!StaticConstructObject_Internal)
			{
				StaticConstructObject_Internal = Pattern::Find("89 6C 24 38 48 89 74 24 ? E8");
				if (StaticConstructObject_Internal != nullptr)
				{
					StaticConstructObject_Internal += 0x9;
				}
				else
				{
					spdlog::warn("StaticConstructObject_Internal Not Found! This will prevent Mods using the ModObjectInstance from working properly.");
				}
			}
		}
	}
	GameProfile::Instance.StaticConstructObject_Internal = (DWORD64)MEM::GetAddressPTR(StaticConstructObject_Internal, 0x1, 0x5);
	spdlog::info("StaticConstructObject_Internal {}", (void*)GameProfile::Instance.StaticConstructObject_Internal);
}
