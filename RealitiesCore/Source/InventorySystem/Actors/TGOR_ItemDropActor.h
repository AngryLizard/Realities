// TGOR (C) // CHECKED //
#pragma once

#include "Components/CapsuleComponent.h"

#include "DimensionSystem/Actors/TGOR_DimensionActor.h"
#include "TGOR_ItemDropActor.generated.h"

///////////////////////////////////////////////// DECL ///////////////////////////////////////////////////

class UTGOR_ItemComponent;
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
	UFUNCTION(BlueprintImplementableEvent, BlueprintAuthorityOnly, Category = "!TGOR Volume", Meta = (Keywords = "C++"))
		void OnItemAssigned(UTGOR_ItemStorage* Storage);

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	/** Movement container */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Actor, meta = (AllowPrivateAccess = "true"))
		UTGOR_PilotComponent* PilotComponent;

	/** Item container */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Actor, meta = (AllowPrivateAccess = "true"))
		UTGOR_ItemComponent* ItemComponent;

public:

	FORCEINLINE UTGOR_ItemComponent* GetItem() const { return ItemComponent; }
	FORCEINLINE UTGOR_PilotComponent* GetPilot() const { return PilotComponent; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Assign an item to this drop and return residual. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		UTGOR_ItemStorage* AssignItem(UTGOR_ItemStorage* Storage, const FTGOR_MovementSpace& ParentSpace, const FVector& Impulse);

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

private:
	
};
