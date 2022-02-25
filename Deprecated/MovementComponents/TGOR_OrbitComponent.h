// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Utility/Structs/TGOR_AutoThreshold.h"

#include "CoreMinimal.h"
#include "Realities/Components/Movement/TGOR_SymplecticComponent.h"
#include "TGOR_OrbitComponent.generated.h"


/**
* TGOR_OrbitComponent moves in a circular orbit
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_OrbitComponent : public UTGOR_SymplecticComponent
{
	GENERATED_BODY()

public:
	UTGOR_OrbitComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void ComputePhysics(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, bool Replay, FTGOR_MovementOutput& Output) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Synchronizes orbit with global (and replicated) GameTimestamp according to a constant angular velocity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		bool SynchronisesOrbitWithTimestamp;

	/** Error threshold for which rotation gets synched */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		FTGOR_AutoThreshold OrbitThreshold;

	/** Orbit angular speed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement", meta = (Keywords = "C++"))
		float Speed;

	/** Orbit plane */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement", meta = (Keywords = "C++"))
		FVector Plane;

	/** Orbit center */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement", meta = (Keywords = "C++"))
		FVector Center;

	/** Orbit correction force */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement", meta = (Keywords = "C++"))
		float CorrectionForce;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	
	/** Orbit Axis */
	UPROPERTY()
		FVector Axis;

	/** Major Axis */
	UPROPERTY()
		FVector Major;

	/** Major axis */
	UPROPERTY()
		float Radius;
};
