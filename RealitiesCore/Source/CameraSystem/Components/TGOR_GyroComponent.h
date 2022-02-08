// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetSystemLibrary.h"

#include "DimensionSystem/TGOR_PilotInstance.h"
#include "CoreSystem/Components/TGOR_SceneComponent.h"
#include "TGOR_GyroComponent.generated.h"

class ITGOR_PilotSimulationInterface;
class UTGOR_PhysicsComponent;

/**
 * 
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class CAMERASYSTEM_API UTGOR_GyroComponent : public UTGOR_SceneComponent
{
	GENERATED_BODY()
	
public:
	UTGOR_GyroComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Satbilisation strength */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		float Stabilisation;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Gyro world rotation, doesn't change with parenting */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		FQuat GyroRotation;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Associated source for up-vector */
	UPROPERTY()
		TScriptInterface<ITGOR_PilotSimulationInterface> PhysicsComponent;
};
