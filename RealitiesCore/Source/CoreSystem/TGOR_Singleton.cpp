// The Gateway of Realities: Planes of Existence.

#include "TGOR_Singleton.h"
#include "Engine/ObjectLibrary.h"

#include "RealitiesUGC/Mod/TGOR_ModBlueprintLibrary.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "RealitiesUGC/Mod/TGOR_Mod.h"

#include "CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "CoreSystem/Gameplay/TGOR_WorldSettings.h"
#include "CoreSystem/Gameplay/TGOR_GameState.h"

#include "Data/TGOR_ConfigData.h"

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "CoreSystem/Storage/TGOR_Package.h"

#include "Engine/World.h"

#if WITH_EDITOR
	#include "Editor/UnrealEd/Public/Kismet2/KismetEditorUtilities.h"
#endif


UTGOR_Singleton::UTGOR_Singleton()
:	Super(),
ContentManager(nullptr)
{
}

#define EVERB(S) if(Verbose) EOUT(S)

/*
TArray<UClass*> LoadPaths(UClass* Class, std::wstring Path, bool Plugin, bool Verbose, TArray<UClass*>& Classes)
{
	UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(Class, true, GIsEditor);
	ObjectLibrary->AddToRoot();
	std::wstring str = L"/Game/Mods";

	if (Plugin)
	{
		str.append(L"/Plugins");
	}
	else
	{
		str.append(L"/").append(Path);
	}

	ObjectLibrary->LoadBlueprintsFromPath(str.c_str());
	TArray<UBlueprintGeneratedClass *> ClassesArray;
	ObjectLibrary->GetObjects<UBlueprintGeneratedClass>(ClassesArray);

	for (int32 i = 0; i < ClassesArray.Num(); ++i)
	{
		UBlueprintGeneratedClass * BlueprintClass = ClassesArray[i];
		if (!BlueprintClass->GetName().Contains(FString("SKEL_"), ESearchCase::CaseSensitive))
		{
			Classes.Add(BlueprintClass);
		}
	}

	EVERB(FString::FromInt(Classes.Num()) + " objects found in " + FString(str.c_str()))

	return(Classes);
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ContentManager* UTGOR_Singleton::GetContentManager() const 
{ 
	return ContentManager; 
}

FTGOR_Time UTGOR_Singleton::GetGameTimestamp() const
{ 
	return GameTimestamp; 
}

FTGOR_Version UTGOR_Singleton::GetVersion() const
{
	FTGOR_Version Version;
	if (IsValid(ContentManager))
	{
		TArray<UTGOR_Mod*> ActiveMods = ContentManager->GetMods();
		for (const UTGOR_Mod* Mod : ActiveMods)
		{
			Version.AddIdentifier(Mod->Entry.Name + FString::FromInt(Mod->Entry.Version));
		}
	}
	return Version;
}

UTGOR_Data* UTGOR_Singleton::GetData(TSubclassOf<UTGOR_Data> Type) const
{
	if (*Type)
	{
		for (UTGOR_Data* Data : Datas)
		{
			if (Data->IsA(Type))
			{
				return Data;
			}
		}
	}
	return nullptr;
}

