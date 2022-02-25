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
};
