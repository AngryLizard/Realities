// The Gateway of Realities: Planes of Existence.


#include "TGOR_ModularSkeletalMeshComponent.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Base/Creature/TGOR_BodypartNode.h"
#include "Realities/Mod/Customisations/TGOR_Customisation.h"
#include "Realities/Mod/Bodyparts/TGOR_Bodypart.h"
#include "Realities/Mod/Colours/TGOR_Colour.h"
#include "Realities/Mod/Skins/TGOR_Skin.h"
#include "Realities/Mod/Spawner/Creatures/TGOR_Creature.h"
#include "Realities/Components/Player/TGOR_UnlockComponent.h"

#include "Net/UnrealNetwork.h"


UTGOR_ModularSkeletalMeshComponent::UTGOR_ModularSkeletalMeshComponent()
	: Super()
{
	SetIsReplicatedByDefault(false);
}


void UTGOR_ModularSkeletalMeshComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTGOR_ModularSkeletalMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Poll for inactive handles to be removed
	for(auto It = Handles.CreateIterator(); It; ++It)
	{
		const TSet<UTGOR_Content*> ContentContext = It->Value.Register->Execute_GetActiveContent(It->Value.Register.GetObject());
		if (!ContentContext.Contains(It->Key))
		{
			if (*It->Value.Type && IsValid(AppearanceRoot))
			{
				// Remove bodypart
				AppearanceRoot->PurgeAutomatic(It->Value.Type);
			}
			It.RemoveCurrent();
		}
	}
}


bool UTGOR_ModularSkeletalMeshComponent::IsNameStableForNetworking() const
{
	return true;
}

bool UTGOR_ModularSkeletalMeshComponent::IsSupportedForNetworking() const
{
	return true;
}

void UTGOR_ModularSkeletalMeshComponent::UpdateUnlocks_Implementation(UTGOR_UnlockComponent* Component)
{
	Constraints.Reset();
	Constraints.Append(Component->GetUnlockedListFromType(UTGOR_Bodypart::StaticClass()));
	Constraints.Append(Component->GetUnlockedListFromType(UTGOR_Colour::StaticClass()));
	Constraints.Append(Component->GetUnlockedListFromType(UTGOR_Skin::StaticClass()));
}

bool UTGOR_ModularSkeletalMeshComponent::SupportsContent_Implementation(UTGOR_Content* Content) const
{
	if (Constraints.Num() == 0) return true;
	return Constraints.Contains(Content);
}

////////////////////////////////////////////////////////////////////////////////////////////////////


FTGOR_CreatureAppearanceInstance UTGOR_ModularSkeletalMeshComponent::ConstructCurrentAppearance()
{
	FTGOR_CreatureAppearanceInstance Instance;
	if (IsValid(AppearanceRoot))
	{
		AppearanceRoot->ConstructPath(Instance.Bodyparts);
	}
	return(Instance);
}

bool UTGOR_ModularSkeletalMeshComponent::CheckAppearance(const FTGOR_CreatureAppearanceInstance& Appearance, UTGOR_Creature* Creature)
{
	return(IsValid(Creature));
}

bool UTGOR_ModularSkeletalMeshComponent::ApplyAppearance(const FTGOR_CreatureAppearanceInstance& Appearance, UTGOR_Creature* Creature)
{
	SINGLETON_RETCHK(false);

	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();

	if (!IsValid(Creature))
	{
		ERRET("Couldn't find Creature", Error, false)
	}

	UTGOR_Bodypart* Content = ContentManager->GetTFromType<UTGOR_Bodypart>(Creature->Bodypart);
	if (!IsValid(Content))
	{
		ERRET("Bodypart not found", Error, false)
	}

	// Create appearance root
	if (!IsValid(AppearanceRoot))
	{
		AppearanceRoot = NewObject<UTGOR_BodypartNode>(this);
	}

	// Create main body if not available
	if (Appearance.Bodyparts.Num() == 0)
	{
		FTGOR_BodypartInstance Instance;

		// Construct from path
		Instance.Depth = 0;
		Instance.Content = Content;

		AppearanceRoot->ConstructTree(TArray<FTGOR_BodypartInstance>({ Instance }), this);
	}
	else
	{
		// Construct from path
		AppearanceRoot->ConstructTree(Appearance.Bodyparts, this);
	}

	return(true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////


TSubclassOf<UTGOR_Bodypart> UTGOR_ModularSkeletalMeshComponent::GetHandleOfType(TSubclassOf<UTGOR_Bodypart> Type) const
{
	for (auto Pair : Handles)
	{
		if (Pair.Value.Type->IsChildOf(Type))
		{
			return Pair.Value.Type;
		}
	}
	return nullptr;
}

void UTGOR_ModularSkeletalMeshComponent::RegisterHandle(TScriptInterface<ITGOR_RegisterInterface> Register, UTGOR_Content* Content, TSubclassOf<UTGOR_Bodypart> Type)
{
	if (*Type)
	{
		// Add handle
		FTGOR_ModularMeshHandle& Handle = Handles.FindOrAdd(Content);
		Handle.Register = Register;

		// Add bodypart if changed
		if (IsValid(AppearanceRoot) && Handle.Type != Type)
		{
			Handle.Type = Type;
			AppearanceRoot->EnsureAutomatic(Type);
		}
	}
}

void UTGOR_ModularSkeletalMeshComponent::UnregisterHandle(UTGOR_Content* Content)
{
	// Remove handle
	FTGOR_ModularMeshHandle Handle;
	Handles.RemoveAndCopyValue(Content, Handle);
	if (*Handle.Type && IsValid(AppearanceRoot))
	{
		// Remove bodypart
		AppearanceRoot->PurgeAutomatic(Handle.Type);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_BodypartNode* UTGOR_ModularSkeletalMeshComponent::GetAppearanceRoot() const
{
	return AppearanceRoot;
}