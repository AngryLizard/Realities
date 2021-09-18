// The Gateway of Realities: Planes of Existence. By Salireths.

#include "TGOR_CoreContent.h"
#include "CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "CoreSystem/Content/TGOR_Category.h"
#include "CoreSystem/TGOR_Singleton.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

UTGOR_CoreContent::UTGOR_CoreContent()
	: Super()
{
}

void UTGOR_CoreContent::BuildResource()
{
	Super::BuildResource();
	SINGLETON_CHK;
}

void UTGOR_CoreContent::PostBuildResource()
{
	Super::PostBuildResource();

	if (DisplayName.IsEmpty())
	{
		DisplayName = FText::FromString(DefaultName);
	}

	// Add display name if no alias was defined
	if (ChatAlias.Num() == 0)
	{
		if (!DisplayName.IsEmpty())
		{
			ChatAlias.Add(DisplayName.ToString());
		}
		else
		{
			ChatAlias.Add(DefaultName);
		}
	}

	// Create display struct
	Display.Description = DefaultDescription;
	Display.Icon = DefaultIcon;
	Display.Aspect = Aspect;
	Display.DisplayName = DisplayName;
	Display.CustomColor = CustomColor;
	Display.UseCustomColor = UseCustomColor;
}

void UTGOR_CoreContent::Reset()
{
	Super::Reset();
	Singleton = nullptr;
}

const TArray<FString>& UTGOR_CoreContent::GetAliases() const
{
	return ChatAlias;
}

FString UTGOR_CoreContent::GetFirstAlias() const
{
	if (ChatAlias.Num() > 0)
	{
		return ChatAlias[0];
	}
	return GetDefaultName();
}


#ifdef WITH_EDITOR

void UTGOR_CoreContent::SetRawDisplayName(const FText& Name)
{
	DisplayName = Name;
	MarkPackageDirty();
}

#endif


UTGOR_Content* UTGOR_CoreContent::GetContentStatic(const UObject* WorldContextObject, TSubclassOf<UTGOR_Content> Class)
{
	check(WorldContextObject);

	UWorld* World = WorldContextObject->GetWorld();
	check(World);

	UTGOR_Singleton* Singleton = UTGOR_GameInstance::EnsureSingletonFromWorld(World);
	if (Singleton)
	{
		UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
		if (ContentManager)
		{
			return(ContentManager->GetContentFromType(Class));
		}
	}

	ERRET("Singleton/ContentManager not found", Error, nullptr);
}

void UTGOR_CoreContent::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(CategoryInsertions);
}

const FTGOR_Display& UTGOR_CoreContent::GetDisplay() const
{
	return Display;
}