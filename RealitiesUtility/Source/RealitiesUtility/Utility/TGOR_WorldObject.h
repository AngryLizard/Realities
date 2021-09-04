// The Gateway of Realities: Planes of Existence.

#pragma once

#include "UObject/Object.h"
#include "TGOR_WorldObject.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class REALITIESUTILITY_API UTGOR_WorldObject : public UObject
{
	GENERATED_BODY()

public:
	UTGOR_WorldObject(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual UWorld* GetWorld() const override;
	virtual void PostInitProperties() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Assigns world */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		void AssignWorld(UObject* WorldContextObject);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY()
		UWorld* AssignedWorld;

private:
	
};
