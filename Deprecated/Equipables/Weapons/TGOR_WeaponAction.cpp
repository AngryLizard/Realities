// The Gateway of Realities: Planes of Existence.


#include "TGOR_WeaponAction.h"
#include "Realities/Components/System/TGOR_ActionComponent.h"
#include "Realities/Components/Creature/TGOR_ModularSkeletalMeshComponent.h"
#include "Realities/Components/Combat/TGOR_HitVolumeComponent.h"

#include "Realities/Components/Combat/TGOR_AimComponent.h"
#include "Realities/Base/TGOR_Singleton.h"

UTGOR_WeaponAction::UTGOR_WeaponAction()
	: Super()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FVector UTGOR_WeaponAction::GetCurrentAimLocation(UTGOR_ActionComponent* Component) const
{
	if (IsValid(Component))
	{
		UTGOR_AimComponent* AimComponent = Component->GetOwnerComponent<UTGOR_AimComponent>();
		if (IsValid(AimComponent))
		{
			return AimComponent->ComputeCurrentAimLocation(false);
		}
	}
	return FVector::ZeroVector;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_WeaponAction::ShootHitVolume(UTGOR_ActionComponent* Component, TSubclassOf<ATGOR_HitVolume> Type, FVector Location, FRotator Rotation, const FTGOR_ForceInstance& Forces)
{
	if (IsValid(Component))
	{
		UTGOR_HitVolumeComponent* HitVolumeComponent = Component->GetOwnerComponent<UTGOR_HitVolumeComponent>();
		if (IsValid(HitVolumeComponent))
		{
			HitVolumeComponent->ShootHitVolume(Component, this, Type, FTransform(Rotation, Location), Forces);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_WeaponAction::RegisterBodypart(UTGOR_ActionComponent* Component, TSubclassOf<UTGOR_Bodypart> Type)
{
	if (IsValid(Component))
	{
		UTGOR_ModularSkeletalMeshComponent* SkeletalMesh = Component->GetOwnerComponent<UTGOR_ModularSkeletalMeshComponent>();
		if (IsValid(SkeletalMesh))
		{
			SkeletalMesh->RegisterHandle(Component, this, Type);
		}
	}
}

void UTGOR_WeaponAction::UnregisterBodypart(UTGOR_ActionComponent* Component)
{
	if (IsValid(Component))
	{
		UTGOR_ModularSkeletalMeshComponent* SkeletalMesh = Component->GetOwnerComponent<UTGOR_ModularSkeletalMeshComponent>();
		if (IsValid(SkeletalMesh))
		{
			SkeletalMesh->UnregisterHandle(this);
		}
	}
}

