// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Components/BillboardComponent.h"
#include "TGOR_AimTargetComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TARGETSYSTEM_API UTGOR_AimTargetComponent : public UBillboardComponent
{
	GENERATED_BODY()

public:

	UTGOR_AimTargetComponent();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Offset to actor position of targetable */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Aim")
		FVector TargetFeetOffset;

	/** Offset to actor position of targetable */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Aim")
		FVector TargetHeadOffset;

	/** Whether targeting is enabled */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Aim")
		bool CanBeTargeted;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get location clamped to offsets */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Aim", Meta = (Keywords = "C++"))
		FVector GetClampedLocation(float Height);
	
};
