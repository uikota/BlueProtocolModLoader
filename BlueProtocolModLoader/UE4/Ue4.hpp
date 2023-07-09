#pragma once
#include <Windows.h>
#include <Psapi.h>
#include "../Utilities/EngineDefFinder.h"
#include "Basic.hpp"
#include "../Utilities/Pattern.h"
#include "CoreUObject_classes.hpp"
namespace UE4
{
	static void InitSDK()
	{
		DWORD64   Names_offset = NULL;
		if (!FName::IsUsingNamePool())
		{
			Names_offset = (*(DWORD64*)(GameProfile::Instance.GName));
			FName::GNames = (DWORD*)Names_offset;
		}
		else
		{
			Names_offset = (DWORD64)(GameProfile::Instance.GName);
			FName::GNames = (DWORD*)Names_offset;
		}

		DWORD64   GObjObjects_offset = NULL;
		GObjObjects_offset = (DWORD64)(GameProfile::Instance.GObject);
		UObject::GObjects = (FUObjectArray*)GObjObjects_offset;

		DWORD64   GWorldObjects = NULL;
		GWorldObjects = (DWORD64)(GameProfile::Instance.GWorld);
		UWorld::GWorld = (UWorld**)GWorldObjects;

		#ifdef BlueProtocolModLoader_EXPORTS
		if (GameProfile::Instance.IsUObjectMissing)
		{
			UE4::UObject* CoreUobjectObject;
			UE4::UObject* UEObject;
			if (GameProfile::Instance.IsUsingFChunkedFixedUObjectArray)
			{
				CoreUobjectObject = UE4::UObject::GObjects->GetAsChunckArray().GetByIndex(1).Object;
				UEObject = UE4::UObject::GObjects->GetAsChunckArray().GetByIndex(2).Object;
			}
			else
			{
				CoreUobjectObject = UE4::UObject::GObjects->GetAsTUArray().GetByIndex(1).Object;
				UEObject = UE4::UObject::GObjects->GetAsTUArray().GetByIndex(2).Object;
			}
			ClassDefFinder::FindUObjectDefs(CoreUobjectObject, UEObject);
			GameProfile::Instance.IsUObjectMissing = false;
		}

		if (GameProfile::Instance.IsUFieldMissing)
		{
			ClassDefFinder::FindUFieldDefs();
			GameProfile::Instance.IsUFieldMissing = false;
		}
		if (GameProfile::Instance.IsUStructMissing)
		{
			ClassDefFinder::FindUStructDefs();
			GameProfile::Instance.IsUStructMissing = false;
		}
		if (GameProfile::Instance.IsUFunctionMissing)
		{
			ClassDefFinder::FindUFunctionDefs();
			GameProfile::Instance.IsUFunctionMissing = false;
		}

		if (GameProfile::Instance.IsPropertyMissing)
		{
			ClassDefFinder::FindUEProperty();
		}
		#endif

	}
}
