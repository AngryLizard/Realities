// The Gateway of Realities: Planes of Existence.


#include "TGOR_VolumeAllocationTest.h"
#include "DrawDebugHelpers.h"

#include "TargetSystem/Components/TGOR_InteractableComponent.h"


FTGOR_VolumeAllocation::FTGOR_VolumeAllocation()
:	Center(FVector::ZeroVector),
	Extend(FVector::ZeroVector)
{

}

// Sets default values
ATGOR_VolumeAllocationTest::ATGOR_VolumeAllocationTest(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Interactable = ObjectInitializer.CreateDefaultSubobject<UTGOR_InteractableComponent>(this, FName(TEXT("Interactable")));
}


bool ATGOR_VolumeAllocationTest::AddVolume(const FVector& Bounds)
{
	FVector Result;
	if(_spatialTree.Insert(Bounds.GetAbs(), Result))
	{
		FTGOR_VolumeAllocation Volume;
		Volume.Center = Result;
		Volume.Extend = Bounds / 2;
		Volumes.Add(Volume);

		return(true);
	}
	
	return(false);
}

bool ATGOR_VolumeAllocationTest::ATGOR_VolumeAllocationTest::RemoveVolume()
{
	const int32 Num = Volumes.Num();
	if (Num > 0)
	{
		// Get random index
		const int32 Index = FMath::RandRange(0, Num-1);

		// Remove from structure and array
		_spatialTree.Remove(Volumes[Index].Center);
		Volumes.RemoveAt(Index);
		return(true);
	}
	return(false);
}

void ATGOR_VolumeAllocationTest::ResetVolumes(const FVector& Extend, int32 Splices)
{
	_spatialTree = CTGOR_SpatialRoot(-Extend, Extend * 2, Splices);
}


void ATGOR_VolumeAllocationTest::DrawVolumes(FLinearColor BranchColor, FLinearColor LeafColor, FLinearColor VolumeColor, float Duration, float Thicc)
{
	// Initialse transform and colors
	const FTransform Transform = GetActorTransform();
	const FColor BColor = BranchColor.ToFColor(false);
	const FColor LColor = LeafColor.ToFColor(false);
	const FColor VColor = VolumeColor.ToFColor(false);

	// Draw a box for every cell in world space
	_spatialTree.ForEach([=](const FVector& Center, const FVector& Extend, bool Leaf)
	{
		const FVector WorldCenter = Transform.TransformPosition(Center);
		const FVector WorldExtend = Transform.TransformVector(Extend);

		DrawDebugBox(GetWorld(), WorldCenter, WorldExtend, Leaf ? LColor : BColor, false, Duration, 0x00, Thicc);
	});

	// Draw a box for every volume in world space
	for (const FTGOR_VolumeAllocation Volume : Volumes)
	{
		const FVector WorldCenter = Transform.TransformPosition(Volume.Center);
		const FVector WorldExtend = Transform.TransformVector(Volume.Extend);

		DrawDebugBox(GetWorld(), WorldCenter, WorldExtend, VColor, false, Duration, 0x00, Thicc);
	}
}