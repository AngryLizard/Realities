#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/System/TGOR_ActionInstance.h"

#include "Realities/Base/Inventory/Modules/TGOR_Module.h"
#include "TGOR_CraftingModule.generated.h"

class UTGOR_ItemStorage;
class UTGOR_MatterModule;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class REALITIES_API UTGOR_CraftingModule : public UTGOR_Module
{
	GENERATED_BODY()

public:

	UTGOR_CraftingModule();
	virtual bool Equals(const UTGOR_Module* Other) const override;
	virtual bool Merge(const UTGOR_Module* Other, ETGOR_NetvarEnumeration Priority) override;

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;
	virtual void Send_Implementation(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Recv_Implementation(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context) override;

	virtual void Clear() override;
	virtual FString Print() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Set target recipe to assemble. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void SetRecipe(UTGOR_Item* Recipe);

	/** Get target recipe to assemble. */
	UFUNCTION(BlueprintPure, Category = "TGOR Action", Meta = (Keywords = "C++"))
		UTGOR_Item* GetRecipe() const;

	/** Assemble target recipe if payment can be made. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		UTGOR_ItemStorage* Assemble(UTGOR_MatterModule* Buyer, ETGOR_ComputeEnumeration& Branches);

	/** Disassemble given item if payment can be made, returns leftover. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		UTGOR_ItemStorage* Disassemble(UTGOR_MatterModule* Seller, UTGOR_ItemStorage* Storage, ETGOR_ComputeEnumeration& Branches);

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Recipe to assemble */
	UPROPERTY()
		UTGOR_Item* AssembleRecipe;
};