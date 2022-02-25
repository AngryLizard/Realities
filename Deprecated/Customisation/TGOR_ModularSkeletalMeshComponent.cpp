// The Gateway of Realities: Planes of Existence.


#include "TGOR_ModularSkeletalMeshComponent.h"

#include "Realities/CoreSystem/TGOR_Singleton.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "Realities/CustomisationSystem/Components/TGOR_ChildSkeletalMeshComponent.h"
#include "Realities/CustomisationSystem/Content/TGOR_Modular.h"
#include "Realities/CustomisationSystem/Content/TGOR_Skeleton.h"
#include "Realities/CreatureSystem/Content/TGOR_Creature.h"
#include "Realities/CustomisationSystem/Content/TGOR_Palette.h"

#include "Realities/CustomisationSystem/UserData/TGOR_SkeletalMerge.h"
#include "Realities/CoreSystem/Handles/Interfaces/TGOR_RegisterInterface.h"

#include "Realities/KnowledgeSystem/TGOR_UnlockComponent.h"
#include "Realities/CoreSystem/Storage/TGOR_Package.h"

#include "Net/UnrealNetwork.h"

#if WITH_EDITOR
#endif

#define LOCTEXT_NAMESPACE "TGOR_ModularSkeletalMeshComponent"

UTGOR_ModularSkeletalMeshComponent::UTGOR_ModularSkeletalMeshComponent()
	: Super(),
	IgnoreFollowsMasterPose(false),
	RootBodypart(nullptr)
{
	SetIsReplicatedByDefault(true);

}


void UTGOR_ModularSkeletalMeshComponent::BeginPlay()
{
	Super::BeginPlay();

	ApplyAppearance(CurrentAppearance);
}

void UTGOR_ModularSkeletalMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Poll for inactive handles to be removed
	for(auto It = Handles.CreateIterator(); It; ++It)
	{
		const TSet<UTGOR_CoreContent*> ContentContext = It->Value.Register->Execute_GetActiveContent(It->Value.Register.GetObject());
		if (!ContentContext.Contains(It->Key))
		{
			if (*It->Value.Type)
			{
				// Remove bodypart
				PurgeAutomatic(It->Value.Type);
			}
			It.RemoveCurrent();
		}
	}
}

void UTGOR_ModularSkeletalMeshComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME_CONDITION(UTGOR_ModularSkeletalMeshComponent, CurrentAppearance, COND_None);
}


bool UTGOR_ModularSkeletalMeshComponent::IsNameStableForNetworking() const
{
	return true;
}

bool UTGOR_ModularSkeletalMeshComponent::IsSupportedForNetworking() const
{
	return true;
}

void UTGOR_ModularSkeletalMeshComponent::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("Appearance", CurrentAppearance);
}

bool UTGOR_ModularSkeletalMeshComponent::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	if (Package.ReadEntry("Appearance", CurrentAppearance))
	{
		ApplyAppearance(CurrentAppearance);
		return true;
	}
	return false;
}

void UTGOR_ModularSkeletalMeshComponent::UpdateContent_Implementation(UTGOR_Spawner* Spawner)
{
	ITGOR_SpawnerInterface::UpdateContent_Implementation(Spawner);

	UTGOR_Modular* Modular = Cast<UTGOR_Modular>(Spawner);
	if (IsValid(Modular))
	{
		RootBodypart = Modular->Instanced_SkeletonInsertion.Collection;
	}
	DefaultAppearance();
}

void UTGOR_ModularSkeletalMeshComponent::AttachAndInitialise()
{
	Master = this;
	Super::AttachAndInitialise();

}

////////////////////////////////////////////////////////////////////////////////////////////////////



bool UTGOR_ModularSkeletalMeshComponent::PreviewAppearance(const FTGOR_AppearanceInstance& Appearance)
{
	if (CanApplyAppearance())
	{
		if (Appearance.Bodyparts.Num() != 0)
		{
			// Construct from path
			ConstructTree(Appearance.Bodyparts, 0);
			UpdateAppearance();
			return true;
		}
	}

	return false;
}

bool UTGOR_ModularSkeletalMeshComponent::ApplyAppearance(const FTGOR_AppearanceInstance& Appearance)
{
	if (CanApplyAppearance())
	{
		if (Appearance.Bodyparts.Num() == 0)
		{
			// Construct fresh from bound armature
			return DefaultAppearance();
		}

		if (IsValid(RootBodypart))
		{
			// Construct from path
			CurrentAppearance = Appearance;
			CurrentAppearance.Bodyparts[0].Content = RootBodypart;

			ConstructTree(Appearance.Bodyparts, 0);
			return ApplyFromCurrentAppearance();
		}
	}

	return false;
}

bool UTGOR_ModularSkeletalMeshComponent::ApplyFromCurrentAppearance()
{
	CurrentAppearance.Bodyparts.Empty();
	ConstructPath(CurrentAppearance.Bodyparts);
	OnAppearanceChanged.Broadcast();
	return true;
}

bool UTGOR_ModularSkeletalMeshComponent::DefaultAppearance()
{
	if (CanApplyAppearance())
	{
		if (IsValid(RootBodypart))
		{
			// Construct default
			FTGOR_BodypartInstance Instance;
			Instance.Depth = 0;
			Instance.Content = RootBodypart;

			ConstructTree(TArray<FTGOR_BodypartInstance>({ Instance }), 0);
			return ApplyFromCurrentAppearance();
		}
	}

	return false;
}

bool UTGOR_ModularSkeletalMeshComponent::CanApplyAppearance()
{
	ENetRole Role = GetOwnerRole();
	return Role == ENetRole::ROLE_Authority || Role == ENetRole::ROLE_None || !IsSupportedForNetworking();
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

void UTGOR_ModularSkeletalMeshComponent::RegisterHandle(TScriptInterface<ITGOR_RegisterInterface> Register, UTGOR_CoreContent* Content, TSubclassOf<UTGOR_Bodypart> Type)
{
	if (*Type)
	{
		// Add handle
		FTGOR_ModularMeshHandle& Handle = Handles.FindOrAdd(Content);
		Handle.Register = Register;

		// Add bodypart if changed
		if (Handle.Type != Type)
		{
			Handle.Type = Type;
			EnsureAutomatic(Type);
		}
	}
}

void UTGOR_ModularSkeletalMeshComponent::UnregisterHandle(UTGOR_CoreContent* Content)
{
	// Remove handle
	FTGOR_ModularMeshHandle Handle;
	Handles.RemoveAndCopyValue(Content, Handle);
	if (*Handle.Type)
	{
		// Remove bodypart
		PurgeAutomatic(Handle.Type);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ModularSkeletalMeshComponent::OnReplicateAppearance()
{
	if (HasBegunPlay())
	{
		ApplyAppearance(CurrentAppearance);
	}
}

#undef LOCTEXT_NAMESPACE