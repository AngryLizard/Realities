
#include "TGOR_CraftingModule.h"

#include "Realities/Base/Inventory/Modules/TGOR_MatterModule.h"
#include "Realities/Base/Inventory/TGOR_ItemStorage.h"
#include "Realities/Mod/Inputs/TGOR_Input.h"
#include "Realities/Mod/Spawner/Items/TGOR_Item.h"

#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Storage/TGOR_Package.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/TGOR_GameState.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_CraftingModule::UTGOR_CraftingModule()
	: Super()
{
	ModulePriority = ETGOR_NetvarEnumeration::Client;
}

bool UTGOR_CraftingModule::Equals(const UTGOR_Module* Other) const
{
	if (!Super::Equals(Other))
	{
		return false;
	}

	const UTGOR_CraftingModule* OtherModule = static_cast<const UTGOR_CraftingModule*>(Other);
	return AssembleRecipe == OtherModule->AssembleRecipe;
}

bool UTGOR_CraftingModule::Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority)
{
	if (!Super::Merge(Other, Priority))
	{
		return false;
	}

	// Add relevant inputs
	const UTGOR_CraftingModule* OtherModule = static_cast<const UTGOR_CraftingModule*>(Other);
	AssembleRecipe = OtherModule->AssembleRecipe;

	return true;
}

void UTGOR_CraftingModule::Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry("AssembleRecipe", AssembleRecipe);
	Super::Write_Implementation(Package, Context);
}

bool UTGOR_CraftingModule::Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry("AssembleRecipe", AssembleRecipe);
	return Super::Read_Implementation(Package, Context);
}

void UTGOR_CraftingModule::Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const
{
	Package.WriteEntry(AssembleRecipe);
	Super::Send_Implementation(Package, Context);
}

bool UTGOR_CraftingModule::Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context)
{
	Package.ReadEntry(AssembleRecipe);
	return Super::Recv_Implementation(Package, Context);
}


void UTGOR_CraftingModule::Clear()
{
	Super::Clear();
	AssembleRecipe = nullptr;
}

FString UTGOR_CraftingModule::Print() const
{
	if (IsValid(AssembleRecipe))
	{
		return FString::Printf(TEXT("Recipe = %s, %s"), *AssembleRecipe->GetDefaultName(), *Super::Print());
	}
	return FString::Printf(TEXT("Recipe = None, %s"), *Super::Print());
}

void UTGOR_CraftingModule::SetRecipe(UTGOR_Item* Recipe)
{
	AssembleRecipe = Recipe;
	MarkDirty();
}

UTGOR_Item* UTGOR_CraftingModule::GetRecipe() const
{
	return AssembleRecipe;
}

UTGOR_ItemStorage* UTGOR_CraftingModule::Assemble(UTGOR_MatterModule* Buyer, ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;
	if (IsValid(AssembleRecipe))
	{
		// Get available composition
		TMap<UTGOR_Matter*, int32> Composition;
		if (IsValid(Buyer))
		{
			Composition = Buyer->GetComposition();
		}

		// Check whether there is enough matter
		if(AssembleRecipe->CanAssembleWith(Composition))
		{
			// Take matter required by recipe
			Buyer->PayStorageMatter(AssembleRecipe->CompositionCapacity);
			Branches = ETGOR_ComputeEnumeration::Success;

			// Create item
			return AssembleRecipe->CreateStorage();
		}
	}
	return nullptr;
}

UTGOR_ItemStorage* UTGOR_CraftingModule::Disassemble(UTGOR_MatterModule* Seller, UTGOR_ItemStorage* Storage, ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;
	if (IsValid(Storage))
	{
		// Get available composition
		TSet<UTGOR_Matter*> Supported;
		if (IsValid(Seller))
		{
			Supported = Seller->GetSupportedMatter();
		}

		// Check whether there is enough matter
		UTGOR_Item* Item = Storage->GetItem();
		if (Item->CanDisassembleInto(Supported))
		{
			if (IsValid(Seller))
			{
				Seller->InvestStorageMatter(Storage->GetComposition());
			}
			Branches = ETGOR_ComputeEnumeration::Success;
			return nullptr;
		}
	}
	return Storage;
}

