// The Gateway of Realities: Planes of Existence.


#include "TGOR_ActorUtility.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PhysicsVolume.h"
#include "Materials/MaterialInstance.h"
#include "Components/MeshComponent.h"

FVector2D UTGOR_ActorUtility::CharacterIKFeetRotation(FVector Vector)
{
	float Dx = Vector.X * Vector.X;
	float Dy = Vector.Y * Vector.Y;
	float Dz = Vector.Z * Vector.Z;

	FVector2D Result;
	Result.X = FMath::Acos(Dz / (Dy + Dz)) * FMath::Sign(Vector.Y);
	Result.Y = FMath::Acos(Dz / (Dx + Dz)) * FMath::Sign(Vector.X) * -1;
	return(Result);
}


bool UTGOR_ActorUtility::FindTeleportSpot(AActor* Actor, FVector Location, FRotator Rotator, FVector& TargetLocation)
{
	if (Actor->GetWorld()->FindTeleportSpot(Actor, Location, Rotator))
	{
		TargetLocation = Location;
		return(true);
	}
	return(false);
}


float UTGOR_ActorUtility::GetWaterLevel(ACharacter* Character)
{
	// Check if inside water volume
	UPawnMovementComponent* Component = Character->GetMovementComponent();
	APhysicsVolume* Volume = Component->GetPhysicsVolume();
	if (Volume->bWaterVolume)
	{
		// Get water volume bounding box height
		FVector Origin, Extend;
		Volume->GetActorBounds(false, Origin, Extend);

		return(Origin.Z + Extend.Z);
	}
	return(0.0f);
}

FVector UTGOR_ActorUtility::GetLocalSpace(AActor* Actor, const FVector& Vector)
{
	if (!IsValid(Actor)) return(Vector);

	FVector Location = Actor->GetActorLocation();
	FVector Diff = Vector - Location;

	const FVector& Forward = Actor->GetActorForwardVector();
	const FVector& Right = Actor->GetActorRightVector();
	const FVector& Up = Actor->GetActorUpVector();

	return(FVector(FVector::DotProduct(Diff, Forward), FVector::DotProduct(Diff, Right), FVector::DotProduct(Diff, Up)));
}

FVector UTGOR_ActorUtility::GetGlobalSpace(AActor* Actor, const FVector& Vector)
{
	if (!IsValid(Actor)) return(Vector);

	const FVector& Forward = Actor->GetActorForwardVector();
	const FVector& Right = Actor->GetActorRightVector();
	const FVector& Up = Actor->GetActorUpVector();

	FVector Location = Actor->GetActorLocation();
	FVector Diff = Vector.X * Forward + Vector.Y * Right + Vector.Z * Up;

	return(Location + Diff);
}

void UTGOR_ActorUtility::OverrideMesh(UMeshComponent* Component, const TMap<uint8, UMaterialInstance*>& MaterialOverride)
{
	// Override all materials
	for (auto Pair : MaterialOverride)
	{
		if (IsValid(Pair.Value))
		{
			const int32 Num = Component->GetNumMaterials();
			if (0 <= Pair.Key && Pair.Key < Num)
			{
				Component->SetMaterial(Pair.Key, Pair.Value);
			}
		}
	}
}