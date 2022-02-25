// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Utility/Error/TGOR_Error.h"

#include "Realities/Interfaces/TGOR_SingletonInterface.h"
#include "UObject/Object.h"
#include "TGOR_Object.generated.h"

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_Object : public UObject, public ITGOR_SingletonInterface
{
	GENERATED_BODY()

public:
	UTGOR_Object(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual UWorld* GetWorld() const override;
	virtual void PostInitProperties() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Assigns world */
	UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (WorldContext = "WorldContextObject", Keywords = "C++"))
		void AssignWorld(UObject* WorldContextObject);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY()
		UWorld* AssignedWorld;

private:
	
};
