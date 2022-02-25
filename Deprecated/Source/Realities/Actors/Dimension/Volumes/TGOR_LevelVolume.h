// TGOR (C) // CHECKED //
#pragma once

#include "Realities/Utility/TGOR_CustomEnumerations.h"

#include "Realities/Actors/Dimension/Volumes/TGOR_BoxPhysicsVolume.h"
#include "TGOR_LevelVolume.generated.h"

///////////////////////////////////////////////// DECL ///////////////////////////////////////////////////

class UTGOR_DimensionData;
class UTGOR_WorldInteractableComponent;

/**
* .
*/

UCLASS()
class REALITIES_API ATGOR_LevelVolume : public ATGOR_BoxPhysicsVolume
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_LevelVolume(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual bool PreAssemble(UTGOR_DimensionData* Dimension) override;

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** */

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = "true"))
		UTGOR_WorldInteractableComponent* InteractableComponent;

public:

	FORCEINLINE UTGOR_WorldInteractableComponent* GetInteractable() const { return InteractableComponent; }

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

	/** Sets gravity according to given force */
	UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void SetExternal(const FVector& Force);

protected:

private:
	
};
