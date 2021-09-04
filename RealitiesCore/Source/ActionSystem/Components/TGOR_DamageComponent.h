// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "TargetSystem/TGOR_AimInstance.h"

#include "Components/SphereComponent.h"
#include "TGOR_DamageComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class ACTIONSYSTEM_API UTGOR_DamageComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	UTGOR_DamageComponent();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Apply forces on interactables within given params */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Combat|Internal", Meta = (Keywords = "C++"))
		void HitAround(const FVector& Origin, float Radius, const FTGOR_ForceInstance& Forces);

	/** Apply forces on interactables within this sphere component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Combat|Internal", Meta = (Keywords = "C++"))
		void HitAroundSelf(const FTGOR_ForceInstance& Forces);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Channel used for hits */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Combat")
		TEnumAsByte<ECollisionChannel> HitChannel;
};
