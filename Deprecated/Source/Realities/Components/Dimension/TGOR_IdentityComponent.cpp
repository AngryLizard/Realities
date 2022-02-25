// The Gateway of Realities: Planes of Existence.


#include "TGOR_IdentityComponent.h"
#include "EngineUtils.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Mod/Dimensions/TGOR_Dimension.h"
#include "Realities/Mod/Spawner/TGOR_Spawner.h"

#include "Realities/Base/Controller/TGOR_PlayerController.h"
#include "Realities/Interfaces/TGOR_SaveInterface.h"
#include "Realities/Utility/Storage/TGOR_Package.h"

#include "Net/UnrealNetwork.h"

UTGOR_IdentityComponent::UTGOR_IdentityComponent()
	: Super(),
	IgnoreStorage(false)
{
	SetIsReplicatedByDefault(true);
}

void UTGOR_IdentityComponent::PostInitProperties()
{
	Super::PostInitProperties();

}

void UTGOR_IdentityComponent::BeginPlay()
{
	Super::BeginPlay();

	//ActorInstance.ClassName = GetOwner()->GetClass()->GetName();
	SINGLETON_CHK;
}

void UTGOR_IdentityComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_IdentityComponent, Identity, COND_None);
}


void UTGOR_IdentityComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (DimensionData.IsValid())
	{
		DimensionData->RemoveDimensionObject(Identity.Identifier);
	}

	Super::EndPlay(EndPlayReason);
}


bool UTGOR_IdentityComponent::PreAssemble(UTGOR_DimensionData* Dimension)
{
	ITGOR_DimensionInterface::PreAssemble(Dimension);
		
	DimensionObjects.Reset();
	SaveObjects.Reset();

	// Assemble owner and associated dimension components
	AActor* Actor = GetOwner();
	if (IsValid(Actor))
	{
		if (Actor->Implements<UTGOR_DimensionInterface>())
		{
			DimensionObjects.Add(Actor);
		}
		
		if (Actor->Implements<UTGOR_SaveInterface>())
		{
			SaveObjects.Add("Owner", Actor);
		}

		const TSet<UActorComponent*>& Components = Actor->GetComponents();
		for (UActorComponent* Component : Components)
		{
			// Don't create loops with myself or other identitycomponents
			if (!Component->IsA(UTGOR_IdentityComponent::StaticClass()))
			{
				if (Component->Implements<UTGOR_DimensionInterface>())//Cast<ITGOR_DimensionInterface>(Component))
				{
					DimensionObjects.Add(Component);
				}

				if (Component->Implements<UTGOR_SaveInterface>())
				{
					SaveObjects.Add(Component->GetName(), Component);
				}
			}
		}
	}

	// Preassemble all associated components
	for (TScriptInterface<ITGOR_DimensionInterface>& DimensionObject : DimensionObjects)
	{
		DimensionObject->PreAssemble(Dimension);
	}
	return true;
}

bool UTGOR_IdentityComponent::Assemble(UTGOR_DimensionData* Dimension)
{
	ITGOR_DimensionInterface::Assemble(Dimension);
	for (TScriptInterface<ITGOR_DimensionInterface>& DimensionObject : DimensionObjects)
	{
		DimensionObject->Assemble(Dimension);
	}
	return true;
}

bool UTGOR_IdentityComponent::PostAssemble(UTGOR_DimensionData* Dimension)
{
	ITGOR_DimensionInterface::PostAssemble(Dimension);
	for (TScriptInterface<ITGOR_DimensionInterface>& DimensionObject : DimensionObjects)
	{
		DimensionObject->PostAssemble(Dimension);
	}
	return true;
}

TSet<UTGOR_Content*> UTGOR_IdentityComponent::GetActiveContent_Implementation() const
{
	TSet<UTGOR_Content*> ContentContext;
	if (IsValid(Identity.Spawner)) ContentContext.Emplace(Identity.Spawner);
	if (IsValid(Identity.Dimension)) ContentContext.Emplace(Identity.Dimension);
	return ContentContext;
}

void UTGOR_IdentityComponent::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	for (auto& Pair : SaveObjects)
	{
		Package.WriteEntry(Pair.Key, Pair.Value);
	}
}

bool UTGOR_IdentityComponent::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	for (auto& Pair : SaveObjects)
	{
		Package.ReadEntry(Pair.Key, Pair.Value);
	}
	return true;
}


int32 UTGOR_IdentityComponent::GetActorIdentifier() const
{
	return Identity.Identifier;
}

UTGOR_Spawner* UTGOR_IdentityComponent::GetActorSpawner() const
{
	return Identity.Spawner;
}

void UTGOR_IdentityComponent::SetActorSpawner(UTGOR_Spawner* Spawner)
{
	Identity.Spawner = Spawner;
	OnIdentityUpdate.Broadcast(Spawner);
}

UTGOR_Dimension* UTGOR_IdentityComponent::GetActorDimension() const
{
	return Identity.Dimension;
}


void UTGOR_IdentityComponent::OnIdentityRepNotify()
{
	OnIdentityUpdate.Broadcast(Identity.Spawner);
}