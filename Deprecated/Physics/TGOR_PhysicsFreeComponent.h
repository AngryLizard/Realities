// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Components/Physics/TGOR_PhysicsComponent.h"
#include "TGOR_PhysicsFreeComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_PhysicsFreeComponent : public UTGOR_PhysicsComponent
{
	GENERATED_BODY()
	
public:
	UTGOR_PhysicsFreeComponent();
	virtual FVector FreedomProject(const FVector& Vector, const FVector& Offset) const override;
	virtual void UpdateStructure() override;

};
