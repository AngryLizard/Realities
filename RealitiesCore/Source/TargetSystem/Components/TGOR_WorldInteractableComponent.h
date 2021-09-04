// The Gateway of Realities: Planes of Existence.

#pragma once

#include "../TGOR_AimInstance.h"

#include "TGOR_InteractableComponent.h"
#include "TGOR_WorldInteractableComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInfluenceServerDelegate, const FTGOR_InfluenceInstance&, Influence);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInfluenceDelegate);

/**
* TGOR_InteractableComponent gives an actor functionality to be interacted with using actions
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class TARGETSYSTEM_API UTGOR_WorldInteractableComponent : public UTGOR_InteractableComponent
{
	GENERATED_BODY()

public:	
	UTGOR_WorldInteractableComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;

	virtual void Influence(const FTGOR_InfluenceInstance& Influence) override;

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Influence")
		FInfluenceDelegate OnInfluence;

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Influence")
		FInfluenceServerDelegate OnServerInfluence;

	/** Sends a replication event to all clients and server */
	UFUNCTION(BlueprintCallable, Reliable, NetMulticast, Category = "!TGOR Influence", Meta = (Keywords = "C++"))
		void ReplicateInfluence();
		virtual void ReplicateInfluence_Implementation();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	
	/** Array of automatically loaded target classes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Target")
		TArray<TSubclassOf<UTGOR_Target>> TargetClasses;

private:
};
