#include "../UE4/Ue4.hpp"
namespace ClassDefFinder
{
	bool FindUObjectIndexDefs(UE4::UObject* CoreUObject, UE4::UObject* UEObject)
	{
		bool HasIndexNotBeenFound = true;

		while (HasIndexNotBeenFound)
		{
			GameProfile::Instance.defs.UObject.Index = GameProfile::Instance.defs.UObject.Index + 0x4;
			if (Read<int32_t>((byte*)CoreUObject + GameProfile::Instance.defs.UObject.Index) == 1)
			{
				if (Read<int32_t>((byte*)UEObject + GameProfile::Instance.defs.UObject.Index) == 2)
				{
					HasIndexNotBeenFound = false;
				}
			}
		}
		return true;
	};

	bool FindUObjectNameDefs(UE4::UObject* CoreUObject)
	{
		bool HasNameNotBeenFound = true;
		bool HasHardNameCheck = false;
		while (HasNameNotBeenFound)
		{
			UE4::FName Name;
			if (HasHardNameCheck == false)
			{
				GameProfile::Instance.defs.UObject.Name = 0x18;
				HasHardNameCheck = true;
			}
			Name = *reinterpret_cast<UE4::FName*>((byte*)CoreUObject + GameProfile::Instance.defs.UObject.Name);
			if (GameProfile::Instance.UsesFNamePool)
			{
				if (UE4::FName::GetFNamePool().IsValidIndex(Name.ComparisonIndex))
				{
					if (Name.GetName() == "Object")
					{
						HasNameNotBeenFound = false;
					}
				}
			}
			else
			{
				if (UE4::FName::GetTNameArray().IsValidIndex(Name.ComparisonIndex))
				{
					if (Name.GetName() == "Object")
					{
						HasNameNotBeenFound = false;
					}
				}
			}
			if (HasNameNotBeenFound == true)
			{
				GameProfile::Instance.defs.UObject.Name = GameProfile::Instance.defs.UObject.Name + 0x8;
			}
		}
		return true;
	}

	bool FindUObjectClassDefs(UE4::UObject* CoreUObject)
	{
		bool HasClassNotBeenFound = true;
		bool HasFinishedHardCheck = false;
		while (HasClassNotBeenFound)
		{
			UE4::UClass* Class;
			if (HasFinishedHardCheck == false)
			{
				GameProfile::Instance.defs.UObject.Class = 0x10;
				Class = Read<UE4::UClass*>((byte*)CoreUObject + GameProfile::Instance.defs.UObject.Class);
				HasFinishedHardCheck = true;
			}
			else
			{
				Class = Read<UE4::UClass*>((byte*)CoreUObject + GameProfile::Instance.defs.UObject.Class);
			}
			
			auto ClassIndex = Class->GetIndex();
			UE4::UObject* ClassCheck;
			if (GameProfile::Instance.IsUsingFChunkedFixedUObjectArray)
			{
				ClassCheck = UE4::UObject::GObjects->GetAsChunckArray().GetByIndex(ClassIndex).Object;
			}
			else
			{
				ClassCheck = UE4::UObject::GObjects->GetAsTUArray().GetByIndex(ClassIndex).Object;
			}

			if (ClassCheck != nullptr)
			{
				if (ClassCheck->GetName() == "Class")
				{
					HasClassNotBeenFound = false;
				}
			}
			if (HasClassNotBeenFound)
			{
				GameProfile::Instance.defs.UObject.Class = GameProfile::Instance.defs.UObject.Class + 0x8;
			}
		}
		return true;
	}

	bool FindUObjectOuterDefs(UE4::UObject* CoreUObject)
	{
		bool HasOuterNotBeenFound = true;
		while (HasOuterNotBeenFound)
		{
			auto Outer = Read<UE4::UObject*>((byte*)CoreUObject + GameProfile::Instance.defs.UObject.Outer);
			UE4::UObject* ObjectCheck;
			if (GameProfile::Instance.IsUsingFChunkedFixedUObjectArray)
			{
				ObjectCheck = UE4::UObject::GObjects->GetAsChunckArray().GetByIndex(0).Object;
			}
			else
			{
				ObjectCheck = UE4::UObject::GObjects->GetAsTUArray().GetByIndex(0).Object;
			}

			if (Outer == ObjectCheck)
			{
				HasOuterNotBeenFound = false;
			}
			if (HasOuterNotBeenFound)
			{
				GameProfile::Instance.defs.UObject.Outer = GameProfile::Instance.defs.UObject.Outer + 0x8;
			}
		}
		return true;
	}

	bool FindUObjectDefs(UE4::UObject* CoreUObject, UE4::UObject* UEObject)
	{
		if (FindUObjectIndexDefs(CoreUObject, UEObject) && FindUObjectNameDefs(CoreUObject) && FindUObjectClassDefs(CoreUObject) && FindUObjectOuterDefs(CoreUObject))
		{
			return true;
		}
		else
		{
		}
		return false;
	}

	bool FindUFieldNextDef()
	{
		bool HasNextNotBeenFound = true;
		auto UserConstructionScript = UE4::UObject::FindObject<UE4::UFunction>("Function Engine.Actor.UserConstructionScript");
		GameProfile::Instance.defs.UField.Next = GameProfile::Instance.defs.UObject.Outer; // Prevents scanning same area over and over.
		while (HasNextNotBeenFound)
		{
			auto NextObject = Read<UE4::UField*>((byte*)UserConstructionScript + GameProfile::Instance.defs.UField.Next);
			if (NextObject && NextObject->GetOuter() == UserConstructionScript->GetOuter())
			{
				HasNextNotBeenFound = false;
			}
			if (HasNextNotBeenFound)
			{
				GameProfile::Instance.defs.UField.Next = GameProfile::Instance.defs.UField.Next + 0x8;
			}
		}
		return true;
	}

	bool FindUFieldDefs()
	{
		if (FindUFieldNextDef())
		{
			return true;
		}
		else
		{
		}
		return false;
	}

	bool FindUStructSuperFieldDef()
	{
		auto StructObject = UE4::UObject::FindObject<UE4::UStruct>("Class CoreUObject.Struct");
		auto FieldObject = UE4::UObject::FindObject<UE4::UField>("Class CoreUObject.Field");
		bool HasSuperFieldNotBeenFound = true;
		GameProfile::Instance.defs.UStruct.SuperStruct = GameProfile::Instance.defs.UField.Next; // Prevents scanning same area over and over.
		while (HasSuperFieldNotBeenFound)
		{
			auto SuperFieldObject = Read<UE4::UObject*>((byte*)StructObject + GameProfile::Instance.defs.UStruct.SuperStruct);
			if (SuperFieldObject == FieldObject)
			{
				HasSuperFieldNotBeenFound = false;
			}
			if (HasSuperFieldNotBeenFound)
			{
				GameProfile::Instance.defs.UStruct.SuperStruct = GameProfile::Instance.defs.UStruct.SuperStruct + 0x8;
			}
		}
		return true;
	}

	bool FindUStructChildrenDef()
	{
		auto VectorObject = UE4::UObject::FindObject<UE4::UObject>("ScriptStruct CoreUObject.Vector");
		auto VectorFirstChildObject = UE4::UObject::FindObject<UE4::UObject>("FloatProperty CoreUObject.Vector.X");
		bool HasChildrenNotBeenFound = true;
		GameProfile::Instance.defs.UStruct.Children = GameProfile::Instance.defs.UStruct.SuperStruct;
		while (HasChildrenNotBeenFound)
		{
			auto ChildObject = Read<UE4::UObject*>((byte*)VectorObject + GameProfile::Instance.defs.UStruct.Children);
			if (VectorFirstChildObject)
			{
				if (VectorFirstChildObject == ChildObject)
				{
					HasChildrenNotBeenFound = false;
				}
				if (HasChildrenNotBeenFound)
				{
					GameProfile::Instance.defs.UStruct.Children = GameProfile::Instance.defs.UStruct.Children + 0x8;
				}
			}
			else
			{
				if (auto Class = ChildObject->GetClass())
				{
					if (Class->GetName() == "FloatProperty" || "Vector")
					{
						HasChildrenNotBeenFound = false;
					}
					else
					{
						GameProfile::Instance.defs.UStruct.Children = GameProfile::Instance.defs.UStruct.Children + 0x8;
					}
				}
				else
				{
					GameProfile::Instance.defs.UStruct.Children = GameProfile::Instance.defs.UStruct.Children + 0x8;
				}
			}
		}
		return true;
	}

	bool FindUStructPropertySizeDef()
	{
		auto VectorObject = UE4::UObject::FindObject<UE4::UObject>("ScriptStruct CoreUObject.Vector");
		while (Read<int32_t>((byte*)VectorObject + GameProfile::Instance.defs.UStruct.PropertiesSize) != 12)
		{
			GameProfile::Instance.defs.UStruct.PropertiesSize = GameProfile::Instance.defs.UStruct.PropertiesSize + 0x4;
		}
		return true;
	}

	int GetOverallUStructSize()
	{
		auto UStruct = UE4::UObject::FindObject<UE4::UStruct>("Class CoreUObject.Struct");
		if (UStruct)
		{
			return UStruct->GetPropertySize();
		}
	}

	bool FindUStructDefs()
	{
		if (FindUStructSuperFieldDef() && FindUStructChildrenDef() && FindUStructPropertySizeDef())
		{
			GameProfile::Instance.defs.UStruct.OverallUStructSize = GetOverallUStructSize();
			return true;
		}
		else
		{
		}
		return false;
	}

	bool FindUFunctionFunctionFlagsDef()
	{
		bool HasFunctionFlagsNotBeenFound = true;
		auto WasRecentlyRendered = UE4::UObject::FindObject<UE4::UFunction>("Function Engine.Actor.WasRecentlyRendered");
		GameProfile::Instance.defs.UFunction.FunctionFlags = GameProfile::Instance.defs.UStruct.OverallUStructSize; // Prevents scanning same area over and over.
		while (HasFunctionFlagsNotBeenFound)
		{
			auto FunctionFlags = Read<int>((byte*)WasRecentlyRendered + GameProfile::Instance.defs.UFunction.FunctionFlags);
			if (FunctionFlags == 1409418241)
			{
				HasFunctionFlagsNotBeenFound = false;
			}
			if (HasFunctionFlagsNotBeenFound)
			{
				GameProfile::Instance.defs.UFunction.FunctionFlags = GameProfile::Instance.defs.UFunction.FunctionFlags + 0x4;
			}
		}
		return true;
	}

	bool FindUFunctionFunc()
	{
		auto UFunction = UE4::UObject::FindObject<UE4::UFunction>("Class CoreUObject.Function");
		if (UFunction)
		{
			GameProfile::Instance.defs.UFunction.Func = UFunction->GetPropertySize() - 0x8;
			return true;
		}
		return false;
	}

	bool FindUFunctionDefs()
	{
		if (FindUFunctionFunctionFlagsDef() && FindUFunctionFunc())
		{
			return true;
		}
		else
		{
		}
		return false;
	}

	bool FindFField()
	{
		auto VectorObject = (UE4::UStruct*)UE4::UObject::FindObject<UE4::UObject>("ScriptStruct CoreUObject.Vector");
		auto FieldChild = (UE4::FField*)VectorObject->GetChildren();
		bool NameFound = false;
		bool NextFound = false;
		while (!NameFound)
		{
			auto Name = *reinterpret_cast<UE4::FName*>((byte*)FieldChild + GameProfile::Instance.defs.FField.Name);
			if (UE4::FName::GetFNamePool().IsValidIndex(Name.ComparisonIndex))
			{
				if (Name.GetName() == "X")
				{
					NameFound = true;
				}
			}
			if (NameFound == false)
			{
				GameProfile::Instance.defs.FField.Name = GameProfile::Instance.defs.FField.Name + 0x8;
			}
		}
		while (!NextFound)
		{
			// 9 times out of 10, its right behind the Name, so we do that check to save possible issues
			auto NextField = Read<UE4::FField*>((byte*)FieldChild + GameProfile::Instance.defs.FField.Name - 0x8);
			if (NextField && NextField->GetName() == "Y")
			{
				NextFound = true;
				GameProfile::Instance.defs.FField.Next = GameProfile::Instance.defs.FField.Name - 0x8;
				break;
			}

			NextField = Read<UE4::FField*>((byte*)FieldChild + GameProfile::Instance.defs.FField.Next);
			if (NextField && NextField->GetName() == "Y")
			{
				NextFound = true;
			}
			if (NextFound == false)
			{
				GameProfile::Instance.defs.FField.Next = GameProfile::Instance.defs.FField.Next + 0x8;
			}
		}
		return true;
	}

	bool FindUEPropertyDefs()
	{
		auto VectorObject = (UE4::UStruct*)UE4::UObject::FindObject<UE4::UObject>("ScriptStruct CoreUObject.Vector");
		bool ArrayDimFound = false;
		bool OffsetFound = false;
		if (GameProfile::Instance.bIsFProperty)
		{
			auto FieldChild = (UE4::FField*)VectorObject->GetChildren();
			while (!ArrayDimFound)
			{
				if (Read<int64_t>((byte*)FieldChild + GameProfile::Instance.defs.Property.ArrayDim) == 17179869185) // Array Dim and Element Size are Side By Side
				{
					ArrayDimFound = true;
				}

				if(ArrayDimFound == false)
				{ 
					GameProfile::Instance.defs.Property.ArrayDim = GameProfile::Instance.defs.Property.ArrayDim + 0x8;
				}
			}

			auto FieldChildY = FieldChild->GetNext();
			auto FieldChildZ = FieldChildY->GetNext();
			GameProfile::Instance.defs.Property.Offset = GameProfile::Instance.defs.Property.ArrayDim + 0x8;
			while (!OffsetFound)
			{
				if (Read<int32_t>((byte*)FieldChildY + GameProfile::Instance.defs.Property.Offset) == 4 && Read<int32_t>((byte*)FieldChildZ + GameProfile::Instance.defs.Property.Offset) == 8)
				{
					OffsetFound = true;
				}
				if (OffsetFound == false)
				{
					GameProfile::Instance.defs.Property.Offset = GameProfile::Instance.defs.Property.Offset + 0x4;
				}
			}
			
		}
		else
		{
			auto FieldChild = (UE4::UField*)VectorObject->GetChildren();
			while (!ArrayDimFound)
			{
				if (Read<int64_t>((byte*)FieldChild + GameProfile::Instance.defs.Property.ArrayDim) == 17179869185) // Array Dim and Element Size are Side By Side
				{
					ArrayDimFound = true;
				}

				if (ArrayDimFound == false)
				{
					GameProfile::Instance.defs.Property.ArrayDim = GameProfile::Instance.defs.Property.ArrayDim + 0x8;
				}
			}
			auto FieldChildY = FieldChild->GetNext();
			auto FieldChildZ = FieldChildY->GetNext();
			GameProfile::Instance.defs.Property.Offset = GameProfile::Instance.defs.Property.ArrayDim + 0x8;
			while (!OffsetFound)
			{
				if (Read<int32_t>((byte*)FieldChildY + GameProfile::Instance.defs.Property.Offset) == 4 && Read<int32_t>((byte*)FieldChildZ + GameProfile::Instance.defs.Property.Offset) == 8)
				{
					OffsetFound = true;
				}
				if (OffsetFound == false)
				{
					GameProfile::Instance.defs.Property.Offset = GameProfile::Instance.defs.Property.Offset + 0x4;
				}
			}
		}
		return true;
	}

	bool FindUEProperty()
	{
		auto VectorObject = (UE4::UStruct*)UE4::UObject::FindObject<UE4::UObject>("ScriptStruct CoreUObject.Vector");
		
		if (!VectorObject->GetChildren()->IsA(UE4::UObject::StaticClass()))
		{
			GameProfile::Instance.bIsFProperty = true;
			if (FindFField())
			{
				GameProfile::Instance.defs.Property.ArrayDim = GameProfile::Instance.defs.FField.Name;
				FindUEPropertyDefs();
			}
		}
		else
		{
			GameProfile::Instance.bIsFProperty = false;
			GameProfile::Instance.defs.Property.ArrayDim = GameProfile::Instance.defs.UField.Next;
			FindUEPropertyDefs();
		}
		return true;
	}
};