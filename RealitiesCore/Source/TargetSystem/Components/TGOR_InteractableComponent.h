// The Gateway of Realities: Planes of Existence.

#pragma once

#include "../TGOR_AimInstance.h"

#include "CoreSystem/Interfaces/TGOR_SingletonInterface.h"
#include "Components/SphereComponent.h"
#include "TGOR_InteractableComponent.generated.h"

class UTGOR_Target;

/**
* TGOR_InteractableComponent gives an actor functionality to be interacted with using actions
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class TARGETSYSTEM_API UTGOR_InteractableComponent : public USphereComponent, public ITGOR_SingletonInterface
{
	GENERATED_BODY()

public:	
	UTGOR_InteractableComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** Influence this component (Needs to be implemented by a subclass) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Interaction", meta = (Keywords = "C++"))
		virtual void Influence(const FTGOR_InfluenceInstance& Influence);
private:
};
