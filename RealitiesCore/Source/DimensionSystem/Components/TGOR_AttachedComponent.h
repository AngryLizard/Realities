// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"

#include "../TGOR_MobilityInstance.h"

#include "TGOR_MobilityComponent.h"
#include "TGOR_AttachedComponent.generated.h"

/**
 * TGOR_AttachedComponent allows components to attach linearly
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class DIMENSIONSYSTEM_API UTGOR_AttachedComponent : public UTGOR_MobilityComponent
{
	GENERATED_BODY()
	
public:
	UTGOR_AttachedComponent();

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Parents a component to this mobility using the Parent socket at a given world position. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool ParentAttached(UTGOR_PilotComponent* Attachee, int32 Index, const FTGOR_MovementPosition& Position);

	/** Parents a component to this mobility using the Parent socket. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool CanParentAttached(UTGOR_PilotComponent* Attachee, int32 Index) const;
};
