// The Gateway of Realities: Planes of Existence.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "TGOR_FloatingMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_FloatingMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	UTGOR_FloatingMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:

	virtual void InitializeComponent() override;
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
};
