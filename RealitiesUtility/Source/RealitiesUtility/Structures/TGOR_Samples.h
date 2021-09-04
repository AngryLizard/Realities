// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "TGOR_Samples.generated.h"


USTRUCT(BlueprintType)
struct REALITIESUTILITY_API FTGOR_Samples
{
	GENERATED_USTRUCT_BODY()

	FTGOR_Samples();
	FTGOR_Samples(const TArray<FVector>& Data);

	// Returns mean
	FVector Mean() const;

	// Returns largest distance to center
	float Radius(const FVector& Center) const;

	// Returns largest distance to center along a given normal
	float RadiusAlong(const FVector& Normal) const;

	// Centers data
	FTGOR_Samples CenterData(const FVector& Center) const;

	// Projects data onto a plane around origin and returns max distance
	FTGOR_Samples ProjectData(const FVector& Normal) const;

	// Compute Pca on this dataset to get an oriented bounding box
	void Pca(int Iterations, FQuat& Quat, FVector& Extend, FVector& Center) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector> Data;
};
