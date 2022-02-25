// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Datastructures/TGOR_Time.h"
#include "Realities/Utility/TGOR_CustomEnumerations.h"

#include "Realities/Interfaces/TGOR_SaveInterface.h"
#include "Realities/Components/Inventory/TGOR_ProcessComponent.h"
#include "TGOR_CraftingComponent.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////


///////////////////////////////////////////// STRUCTS ///////////////////////////////////////////////////


UENUM(BlueprintType)
enum class ETGOR_CraftingEnumeration : uint8
{
	/** Waiting for commands */
	Idle,
	/** Assembling an item */
	Assembling,
	/** Disassembling an item */
	Disassembling
};

/**
 * TGOR_CraftingComponent allows storing crafting ingredients
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_CraftingComponent : public UTGOR_ProcessComponent
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_CraftingComponent();

	void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	virtual void Process(float Mass) override;
	virtual bool IsValidProcess() const override;
	virtual bool IsValidInput(UTGOR_ItemStorage* Storage) const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	/** Called when items are being assembled */
	UPROPERTY(BlueprintAssignable, Category = "TGOR Inventory")
		FProcessTerminalDelegate OnAssemble;

	/** Called when items are being disassembled */
	UPROPERTY(BlueprintAssignable, Category = "TGOR Inventory")
		FProcessTerminalDelegate OnDisassemble;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Recipes to assemble */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Inventory")
		TArray<UTGOR_Item*> Recipes;

	/** Current state of the crafting */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Inventory")
		ETGOR_CraftingEnumeration CraftingState;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get free socket that can hold a given item */
	UFUNCTION(BlueprintPure, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		UTGOR_NamedSocket* GetFreeSocket(UTGOR_Item* Item) const;

	/** Add a recipe to assembly queue */
	UFUNCTION(BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		void AddRecipe(UTGOR_Item* Recipe);

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:

};
