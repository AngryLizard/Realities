// The Gateway of Realities: Planes of Existence. By Salireths.

#include "TGOR_BoneTarget.h"

#include "TargetSystem/Components/TGOR_AimTargetComponent.h"
#include "Components/SkeletalMeshComponent.h"

UTGOR_BoneTarget::UTGOR_BoneTarget()
	: Super(),
	BoneName(""),
	Offset(FVector::ZeroVector),
	CheckBoneMatch(false),
	DistanceThreshold(150.0f)
{
	Importance = 0.5f;
}


bool UTGOR_BoneTarget::OverlapTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, FTGOR_AimPoint& Point) const
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

		// Get bone transform (locally offset)
		const int32 Index = Skeletal->GetBoneIndex(BoneName);
		if (Index != INDEX_NONE)
		{
			const FTransform Transform = Skeletal->GetBoneTransform(Index);
			Point.Distance = Component->ComputeRelativeDistance(Transform.TransformPosition(Offset), Origin);
			return true;
		}
	}
	return false;
}

bool UTGOR_BoneTarget::ComputeTarget(UTGOR_AimTargetComponent* Component, const FVector& Origin, const FVector& Direction, FTGOR_AimPoint& Point) const
{
	PARAMS_CHK;

	AActor* Owner = Component->GetOwner();
	USkinnedMeshComponent* Skeletal = Cast<USkeletalMeshComponent>(Owner->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (!IsValid(Skeletal))
	{
		return false;
	}

	const float ScaledThreshold = DistanceThreshold * Component->GetComponentScale().GetMax();

	// Only interested in master pose
	if (Skeletal->LeaderPoseComponent.IsValid())
	{
		Skeletal = Skeletal->LeaderPoseComponent.Get();
	}

	Point.Instance.Component = Skeletal;

	// Get bone transform (locally offset)
	const int32 Index = Skeletal->GetBoneIndex(BoneName);
	if (Index == INDEX_NONE)
	{
		return false;
	}

	const FTransform Transform = Skeletal->GetBoneTransform(Index);
	Point.Center = Transform.TransformPosition(Offset);

	// Check whether target is close enough
	if ((Point.Center - Origin).SizeSquared() >= (Component->TargetRadius * Component->TargetRadius))
	{
		return false;
	}

	// Ignore local offset for creation

	const FVector Projection = Origin + (Point.Center - Origin).ProjectOnToNormal(Direction);
	Point.Instance.Offset = Transform.InverseTransformPosition(Projection);

	// Check whether skeleton occludes
	FHitResult Hit;
	FCollisionQueryParams Params;
	if (Skeletal->LineTraceComponent(Hit, Origin, Origin + Direction * Component->TargetRadius, Params))
	{
		// Make sure we hit the right bone if check enabled
		if (CheckBoneMatch && Hit.BoneName != BoneName)
		{
			return false;
		}

		// Check impact point for distance
		Point.Distance = (Hit.ImpactPoint - Point.Center).Size() / ScaledThreshold;
		return true;
	}
	
	Point.Distance = Point.Instance.Offset.Size() / ScaledThreshold;
	return true;
}

FVector UTGOR_BoneTarget::QueryAimLocation(const FTGOR_AimInstance& Instance) const
{
	if (Instance.Component.IsValid())
	{
		USkeletalMeshComponent* Skeletal = Cast<USkeletalMeshComponent>(Instance.Component.Get());
		if (IsValid(Skeletal))
		{
			// Get bone transform (locally offset)
			const int32 Index = Skeletal->GetBoneIndex(BoneName);
			if (Index != INDEX_NONE)
			{
				const FTransform Transform = Skeletal->GetBoneTransform(Index);

				// Project onto bone screen plane
				return Transform.TransformPosition(Instance.Offset);
			}
		}
		else
		{
			ERRET("Skeletal component invalid", Error, FVector::ZeroVector)
		}
	}
	return FVector::ZeroVector;
}

FVector UTGOR_BoneTarget::QueryStickyLocation(const FTGOR_AimInstance& Instance) const
{
	if (Instance.Component.IsValid())
	{
		USkeletalMeshComponent* Skeletal = Cast<USkeletalMeshComponent>(Instance.Component.Get());
		if (IsValid(Skeletal))
		{
			// Get bone transform (locally offset)
			const int32 Index = Skeletal->GetBoneIndex(BoneName);
			if (Index != INDEX_NONE)
			{
				const FTransform Transform = Skeletal->GetBoneTransform(Index);

				// Project onto bone screen plane
				return Transform.TransformPosition(Offset);
			}
		}
		else
		{
			ERRET("Skeletal component invalid", Error, FVector::ZeroVector)
		}
	}
	return FVector::ZeroVector;
}