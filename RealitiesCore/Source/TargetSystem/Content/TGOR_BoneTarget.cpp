// The Gateway of Realities: Planes of Existence. By Salireths.

#include "TGOR_BoneTarget.h"

#include "TargetSystem/Components/TGOR_InteractableComponent.h"
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


bool UTGOR_BoneTarget::OverlapTarget(UTGOR_InteractableComponent* Component, const FVector& Origin, float MaxDistance, FTGOR_AimPoint& Point) const
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

		// Get bone transform (locally offset)
		const int32 Index = Skeletal->GetBoneIndex(BoneName);
		if (Index != INDEX_NONE)
		{
			const FTransform Transform = Skeletal->GetBoneTransform(Index);
			return ComputeDistance(Transform.TransformPosition(Offset), Origin, MaxDistance, Point);
		}
	}
	return false;
}

bool UTGOR_BoneTarget::SearchTarget(UTGOR_InteractableComponent* Component, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimPoint& Point) const
{
	PARAMS_CHK;

	AActor* Owner = Component->GetOwner();
	USkinnedMeshComponent* Skeletal = Cast<USkeletalMeshComponent>(Owner->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (IsValid(Skeletal))
	{
		const float ScaledThreshold = DistanceThreshold * Component->GetComponentScale().GetMax();

		// Only interested in master pose
		if (Skeletal->MasterPoseComponent.IsValid())
		{
			Skeletal = Skeletal->MasterPoseComponent.Get();
		}
		Point.Component = Skeletal;

		// Get bone transform (locally offset)
		const int32 Index = Skeletal->GetBoneIndex(BoneName);
		if (Index != INDEX_NONE)
		{
			const FTransform Transform = Skeletal->GetBoneTransform(Index);
			Point.Center = Transform.TransformPosition(Offset);

			// Check whether target is close enough
			if ((Point.Center - Origin).SizeSquared() < (MaxDistance * MaxDistance))
			{
				// Check whether skeleton occludes
				FHitResult Hit;
				FCollisionQueryParams Params;
				if (Skeletal->LineTraceComponent(Hit, Origin, Origin + Direction * MaxDistance, Params))
				{
					// Make sure we hit the right bone if check enabled
					if (!CheckBoneMatch || Hit.BoneName == BoneName)
					{
						// Check impact point for distance
						Point.Distance = (Hit.ImpactPoint - Point.Center).Size() / ScaledThreshold;
						return Point.Distance < 1.0f;
					}
				}
				else
				{
					// Project onto bone screen plane if not hit
					const FVector Relative = ComputeProject(Point.Center, Origin, Direction);
					Point.Distance = Relative.Size() / ScaledThreshold;
					return Point.Distance < 1.0f;
				}
			}
		}
	}
	return false;
}

bool UTGOR_BoneTarget::ComputeTarget(const FTGOR_AimPoint& Point, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimInstance& Instance) const
{
	if (Point.Component.IsValid())
	{
		USkeletalMeshComponent* Skeletal = Cast<USkeletalMeshComponent>(Point.Component.Get());
		if (IsValid(Skeletal))
		{
			Instance.Component = Skeletal;

			// Get bone transform (locally offset)
			Instance.Index = Skeletal->GetBoneIndex(BoneName);
			if (Instance.Index != INDEX_NONE)
			{
				const FTransform Transform = Skeletal->GetBoneTransform(Instance.Index);

				// Ignore local offset for creation
				Instance.Offset = ComputeOffset(Transform, Origin, Direction);
				return true;
			}
		}
	}
	return false;
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