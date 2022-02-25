// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Base/Instances/Combat/TGOR_AimInstance.h"
#include "Realities/Base/Content/TGOR_Content.h"

#include "Realities/Interfaces/TGOR_SingletonInterface.h"
#include "Components/SphereComponent.h"
#include "TGOR_InteractableComponent.generated.h"

class ATGOR_PlayerController;
class UTGOR_ActionComponent;
class UTGOR_MobilityComponent;
class ATGOR_CreatureCharacter;
class UTGOR_Action;
class UTGOR_Target;
class UTGOR_Creature;
class ATGOR_HUD;

/**
* TGOR_InteractableComponent gives an actor functionality to be interacted with using actions
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_InteractableComponent : public USphereComponent, public ITGOR_SingletonInterface
{
	GENERATED_BODY()

public:	
	UTGOR_InteractableComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** Influence this component (Needs to be implemented by a subclass) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Interaction", meta = (Keywords = "C++"))
		virtual void Influence(const FTGOR_InfluenceInstance& Influence);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Fills targets from creature setup */
	UFUNCTION(BlueprintCallable, Category = "TGOR Creature", Meta = (Keywords = "C++"))
		void BuildTargets(UTGOR_Content* Content);

	/** Array of currently supported targets */
	UPROPERTY(BlueprintReadWrite, Category = "TGOR Target")
		TArray<UTGOR_Target*> Targets;

private:
};
