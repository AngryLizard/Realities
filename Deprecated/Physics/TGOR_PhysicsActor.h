// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Components/Physics/TGOR_PhysicsObjectInterface.h"
#include "Realities/Components/Physics/TGOR_PhysicsFreeComponent.h"

#include "Kismet/KismetSystemLibrary.h"

#include "Realities/Actors/TGOR_Actor.h"
#include "TGOR_PhysicsActor.generated.h"

/**
 * ATGOR_PhysicsActor defines actors that are physics relevant
 */
UCLASS()
class REALITIES_API ATGOR_PhysicsActor : public ATGOR_Actor
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_PhysicsActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** */

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	/** Container to hold all physics objects (need to implement UTGOR_PhysicsObjectInterfaces) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TGOR Physics", Meta = (AllowPrivateAccess = "true"))
		UTGOR_PhysicsFreeComponent* PhysicsContainer;

public:

	FORCEINLINE UTGOR_PhysicsFreeComponent* GetPhysics() const { return PhysicsContainer; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Gravity constant for this body */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Physics")
		FVector Gravity;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Applies spin (Doesn't replicate) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		void ApplySpin(FVector Axis);

	/** Applies force (Doesn't replicate) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Physics", Meta = (Keywords = "C++"))
		void ApplyForce(FVector Force);

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:

};
