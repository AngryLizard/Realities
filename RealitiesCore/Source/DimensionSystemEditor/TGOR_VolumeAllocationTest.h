// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "RealitiesUtility/Structures/TGOR_SpatialTree.h"

#include "GameFramework/Actor.h"
#include "TGOR_VolumeAllocationTest.generated.h"

class UTGOR_InteractableComponent;


///////////////////////////////////////////////// STRUCTS ///////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct DIMENSIONSYSTEMEDITOR_API FTGOR_VolumeAllocation
{
	GENERATED_USTRUCT_BODY()

		FTGOR_VolumeAllocation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Instance")
		FVector Center;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Instance")
		FVector Extend;
};

/**
* ATGOR_VolumeAllocationTest is used to show and test dimension allocation
*/
UCLASS()
class DIMENSIONSYSTEMEDITOR_API ATGOR_VolumeAllocationTest : public AActor
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_VolumeAllocationTest(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** */

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Actor, meta = (AllowPrivateAccess = "true"))
		UTGOR_InteractableComponent* Interactable;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Add a volume */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Test", Meta = (Keywords = "C++"))
		bool AddVolume(const FVector& Bounds);

	/** Removes a random volume */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Test", Meta = (Keywords = "C++"))
		bool RemoveVolume();

	/** Resets volumes and sets amount of splices */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Test", Meta = (Keywords = "C++"))
		void ResetVolumes(const FVector& Extend, int32 Splices);

	/** Draws volumes */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Test", Meta = (Keywords = "C++"))
		void DrawVolumes(FLinearColor BranchColor, FLinearColor LeafColor, FLinearColor VolumeColor, float Duration, float Thicc);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Test")
		TArray<FTGOR_VolumeAllocation> Volumes;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:

	CTGOR_SpatialRoot _spatialTree = CTGOR_SpatialRoot(-FVector::OneVector, 2*FVector::OneVector, 2);

};
