// TGOR (C) // CHECKED //
#pragma once

#include "Components/CapsuleComponent.h"

#include "DimensionSystem/Actors/TGOR_DimensionActor.h"
#include "TGOR_ItemDropActor.generated.h"

///////////////////////////////////////////////// DECL ///////////////////////////////////////////////////

class UTGOR_DropComponent;
class UTGOR_PilotComponent;

/**
* 
*/
UCLASS()
class INVENTORYSYSTEM_API ATGOR_ItemDropActor : public ATGOR_DimensionActor
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_ItemDropActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** Called after an item has been assigned to this actor. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintAuthorityOnly, Category = "!TGOR Inventory", Meta = (Keywords = "C++"))
		void OnItemAssigned(UTGOR_ItemTask* Item);

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	/** Movement container */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Actor, meta = (AllowPrivateAccess = "true"))
		UTGOR_PilotComponent* PilotComponent;

	/** Drop container */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Actor, meta = (AllowPrivateAccess = "true"))
		UTGOR_DropComponent* DropComponent;

public:

	FORCEINLINE UTGOR_DropComponent* GetDrop() const { return DropComponent; }
	FORCEINLINE UTGOR_PilotComponent* GetPilot() const { return PilotComponent; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

private:
	
};
