// The Gateway of Realities: Planes of Existence. By Salireths.

#include "TGOR_SkeletalTarget.h"

#include "../Components/TGOR_InteractableComponent.h"
#include "Components/SkeletalMeshComponent.h"

UTGOR_SkeletalTarget::UTGOR_SkeletalTarget()
:	Super(),
	SnapCenterToBone(true),
	HitDistancePercentage(0.2f)
{
	Importance = 0.5f;
}

bool UTGOR_SkeletalTarget::OverlapTarget(UTGOR_InteractableComponent* Component, const FVector& Origin, float MaxDistance, FTGOR_AimPoint& Point) const
{
	PARAMS_CHK;

	AActor* Owner = Component->GetOwner();
	USkinnedMeshComponent* Skeletal = Cast<USkeletalMeshComponent>(Owner->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (IsValid(Skeletal))
	{
		// Only interested in master pose
		if (Skeletal->MasterPoseComponent.IsValid())
		{
			Skeletal = Skeletal->MasterPoseComponent.Get();
		}
		Point.Component = Skeletal;

		// TODO: Overlap doesn't output bones. Could sweep over super small distance instead
		const FCollisionShape Shape = FCollisionShape::MakeSphere(MaxDistance);
		if (Skeletal->OverlapComponent(Origin, FQuat::Identity, Shape))
		{
			Point.Center = Origin;
			Point.Distance = HitDistancePercentage;
			return true;
		}
	}
	Point.Distance = 1.0f;
	return false;
}

bool UTGOR_SkeletalTarget::SearchTarget(UTGOR_InteractableComponent* Component, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimPoint& Point) const
{
	PARAMS_CHK;

	AActor* Owner = Component->GetOwner();
	USkinnedMeshComponent* Skeletal = Cast<USkeletalMeshComponent>(Owner->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (IsValid(Skeletal))
	{
		// Only interested in master pose
		if (Skeletal->MasterPoseComponent.IsValid())
		{
			Skeletal = Skeletal->MasterPoseComponent.Get();
		}
		Point.Component = Skeletal;

		// Default to full distance 
		FHitResult Hit;
		FCollisionQueryParams Params;
		if (Skeletal->LineTraceComponent(Hit, Origin, Origin + Direction * MaxDistance, Params))
		{
			if (SnapCenterToBone)
			{
				// Get bone transform
				int32 Index = Skeletal->GetBoneIndex(Hit.BoneName);
				const FTransform Transform = Skeletal->GetBoneTransform(Index);
				Point.Center = Transform.GetLocation();
			}
			else
			{
				Point.Center = Hit.ImpactPoint;
			}

			Point.Distance = HitDistancePercentage;
			return true;
		}
	}
	Point.Distance = 1.0f;
	return false;
}

bool UTGOR_SkeletalTarget::ComputeTarget(const FTGOR_AimPoint& Point, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimInstance& Instance) const
{
	USkeletalMeshComponent* Skeletal = Cast<USkeletalMeshComponent>(Point.Component.Get());
	if (IsValid(Skeletal))
	{
		FHitResult Hit;
		FCollisionQueryParams Params;
		if (Skeletal->LineTraceComponent(Hit, Origin, Origin + Direction * MaxDistance, Params))
		{
			Instance.Component = Point.Component;

			// Get bone transform
			Instance.Index = Skeletal->GetBoneIndex(Hit.BoneName);
			const FTransform Transform = Skeletal->GetBoneTransform(Instance.Index);

			// Compute relative offset
			Instance.Offset = Transform.InverseTransformPosition(Hit.ImpactPoint);
			return true;
		}
	}
	
	return false;
}

FVector UTGOR_SkeletalTarget::QueryAimLocation(const FTGOR_AimInstance& Instance) const
{
	USkeletalMeshComponent* Skeletal = Cast<USkeletalMeshComponent>(Instance.Component.Get());
	if (IsValid(Skeletal))
	{
		// Compute world location
		const FTransform Transform = Skeletal->GetBoneTransform(Instance.Index);
		return(Transform.TransformPosition(Instance.Offset));
	}
	else
	{
		ERRET("Skeletal component invalid", Error, FVector::ZeroVector)
	}
	return(FVector::ZeroVector);
}

FVector UTGOR_SkeletalTarget::QueryStickyLocation(const FTGOR_AimInstance& Instance) const
{
	return(QueryAimLocation(Instance));
}