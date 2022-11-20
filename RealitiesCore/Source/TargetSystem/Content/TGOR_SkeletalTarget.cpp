// The Gateway of Realities: Planes of Existence. By Salireths.

#include "TGOR_SkeletalTarget.h"

#include "../Components/TGOR_AimTargetComponent.h"
#include "Components/SkeletalMeshComponent.h"

UTGOR_SkeletalTarget::UTGOR_SkeletalTarget()
:	Super(),
	SnapCenterToBone(true),
	HitDistancePercentage(0.2f)
{
	Importance = 0.5f;
}

bool UTGOR_SkeletalTarget::OverlapTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, FTGOR_AimPoint& Point) const
{
	PARAMS_CHK;

	AActor* Owner = Component->GetOwner();
	USkinnedMeshComponent* Skeletal = Cast<USkeletalMeshComponent>(Owner->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (IsValid(Skeletal))
	{
		// Only interested in master pose
		if (Skeletal->LeaderPoseComponent.IsValid())
		{
			Skeletal = Skeletal->LeaderPoseComponent.Get();
		}
		Point.Instance.Component = Skeletal;

		// TODO: Overlap doesn't output bones. Could sweep over super small distance instead
		const FCollisionShape Shape = FCollisionShape::MakeSphere(Component->TargetRadius);
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

bool UTGOR_SkeletalTarget::ComputeTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, const FVector& Direction, FTGOR_AimPoint& Point) const
{
	PARAMS_CHK;

	AActor* Owner = Component->GetOwner();
	USkinnedMeshComponent* Skeletal = Cast<USkeletalMeshComponent>(Owner->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (IsValid(Skeletal))
	{
		// Only interested in master pose
		if (Skeletal->LeaderPoseComponent.IsValid())
		{
			Skeletal = Skeletal->LeaderPoseComponent.Get();
		}

		// Default to full distance 
		FHitResult Hit;
		FCollisionQueryParams Params;
		if (Skeletal->LineTraceComponent(Hit, Origin, Origin + Direction * Component->TargetRadius, Params))
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

			Point.Instance.Component = Skeletal;

			// Get bone transform
			Point.Instance.Index = Skeletal->GetBoneIndex(Hit.BoneName);
			const FTransform Transform = Skeletal->GetBoneTransform(Point.Instance.Index);

			// Compute relative offset
			Point.Instance.Offset = Transform.InverseTransformPosition(Hit.ImpactPoint);

			return true;
		}
	}
	Point.Distance = 1.0f;
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