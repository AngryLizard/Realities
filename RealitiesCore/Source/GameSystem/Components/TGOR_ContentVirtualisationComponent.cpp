// The Gateway of Realities: Planes of Existence.


#include "TGOR_ContentVirtualisationComponent.h"
#include "DimensionSystem/Components/TGOR_IdentityComponent.h"
#include "DimensionSystem/Content/TGOR_Spawner.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "GameSystem/TGOR_ContentVirtualisationSingleton.h"
#include "GameSystem/UI/TGOR_ContentVirtualisationWidget.h"
#include "GameSystem/Content/TGOR_GameMod.h"

UTGOR_ContentVirtualisationComponent::UTGOR_ContentVirtualisationComponent()
	: Super()
{
	VirtualSingleton = nullptr;
	VirtualisedMod = UTGOR_GameMod::StaticClass();
}

UTGOR_ContentVirtualisationWidget* UTGOR_ContentVirtualisationComponent::CreateVirtualisationWidget()
{
	if (*WidgetClass)
	{
		UTGOR_ContentVirtualisationWidget* Widget = CreateWidget<UTGOR_ContentVirtualisationWidget>(GetWorld(), WidgetClass);
		if (IsValid(Widget) && IsValid(VirtualSingleton))
		{
			Widget->OverrideSingleton(VirtualSingleton);
			Widget->OnInitialiseWith(this);
			return Widget;
		}
	}
	return nullptr;
}

void UTGOR_ContentVirtualisationComponent::Virtualise(bool Refresh)
{
	if (!IsValid(VirtualSingleton))
	{
		VirtualSingleton = NewObject<UTGOR_ContentVirtualisationSingleton>(this);
		VirtualSingleton->CreateContentManager();

		VirtualRegistry = NewObject<UTGOR_ModRegistry>(this);
		VirtualRegistry->FindUGCPackages();

		OverrideSingleton(VirtualSingleton);
	}
	else if (!Refresh)
	{
		return;
	}

	if (IsValid(VirtualSingleton) && IsValid(VirtualRegistry))
	{
		UTGOR_ContentManager* ContentManager = VirtualSingleton->GetContentManager();
		ModSetup = ContentManager->GetLocalModSetup(VirtualRegistry, VirtualisedMod);
		ContentManager->LoadModSetup(VirtualRegistry, ModSetup);

		// Update ownership on all connected
		AActor* Actor = GetOwner();
		if (IsValid(Actor))
		{
			ITGOR_SingletonInterface* ActorInterface = Cast<ITGOR_SingletonInterface>(Actor);
			if (ActorInterface)
			{
				ActorInterface->OverrideSingleton(VirtualSingleton);
			}

			// Copy instead of reference, components could be added during content update
			const TSet<UActorComponent*> Components = Actor->GetComponents();
			for (UActorComponent* Component : Components)
			{
				ITGOR_SingletonInterface* ComponentInterface = Cast<ITGOR_SingletonInterface>(Component);
				if (ComponentInterface)
				{
					ComponentInterface->OverrideSingleton(VirtualSingleton);
				}
			}
		}

		// Reset identity with default spawner
		UTGOR_IdentityComponent* Identity = GetOwnerComponent<UTGOR_IdentityComponent>();
		if (IsValid(Identity))
		{
			UTGOR_Spawner* Spawner = ContentManager->GetTFromType<UTGOR_Spawner>(Identity->DefaultSpawner);
			if (IsValid(Spawner))
			{
				// Reset to rebuild spawner
				Identity->SetActorSpawner(nullptr);
				Identity->SetActorSpawner(Spawner);
			}
		}

		FEditorScriptExecutionGuard ScriptGuard;

		OnVirtualisation.Broadcast();
	}
}