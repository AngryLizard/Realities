// The Gateway of Realities: Planes of Existence.

#include "TGOR_CraftingComponent.h"

#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Base/Inventory/Modules/TGOR_MatterModule.h"
#include "Realities/Mod/Targets/Sockets/TGOR_NamedSocket.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameState.h"
#include "Net/UnrealNetwork.h"


UTGOR_CraftingComponent::UTGOR_CraftingComponent()
	: Super(),
	CraftingState(ETGOR_CraftingEnumeration::Idle)
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UTGOR_CraftingComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UTGOR_CraftingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UTGOR_CraftingComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}


void UTGOR_CraftingComponent::Process(float Mass)
{
	if (CraftingState == ETGOR_CraftingEnumeration::Assembling)
	{
		OnAssemble.Broadcast();
	}
	if (CraftingState == ETGOR_CraftingEnumeration::Disassembling)
	{
		OnDisassemble.Broadcast();
	}
	Super::Process(Mass);
}

bool UTGOR_CraftingComponent::IsValidProcess() const
{
	if (CraftingState == ETGOR_CraftingEnumeration::Idle)
	{
		return false;
	}
	if (Super::IsValidProcess())
	{
		return true;
	}
	if (CraftingState == ETGOR_CraftingEnumeration::Assembling)
	{
		return Recipes.Num() > 0;
	}
	return false;
}

bool UTGOR_CraftingComponent::IsValidInput(UTGOR_ItemStorage* Storage) const
{

	return Super::IsValidInput(Storage);
}

////////////////////////////////////////////////////////////////////////////////////////////////////


UTGOR_NamedSocket* UTGOR_CraftingComponent::GetFreeSocket(UTGOR_Item* Item) const
{
	for (TSubclassOf<UTGOR_NamedSocket> Type : InputSockets)
	{
		UTGOR_NamedSocket* Socket = GetSOfType<UTGOR_NamedSocket>(Type);
		if (IsValid(Socket) && CanFitToSocket(Item->GetShape(), Socket))
		{
			return Socket;
		}
	}
	return nullptr;
}

void UTGOR_CraftingComponent::AddRecipe(UTGOR_Item* Recipe)
{
	Recipes.Emplace(Recipe);
}
