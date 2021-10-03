// The Gateway of Realities: Planes of Existence.

#include "TGOR_ArmatureComponent.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "DimensionSystem/Components/TGOR_MobilityComponent.h"
#include "DimensionSystem/Volumes/TGOR_LevelVolume.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "DimensionSystem/Content/TGOR_Spawner.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ArmatureComponent::UTGOR_ArmatureComponent()
:	Super()
{
}

void UTGOR_ArmatureComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	/*
	// Testing attachment points
	FTGOR_MovementSpace Space = GetBase().ComputeParentSpace();
	DrawDebugPoint(GetWorld(), Space.Linear, 25.0f, FColor::Blue, false, -1.0f, 0);
	*/

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTGOR_ArmatureComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

FTGOR_MovementDynamic UTGOR_ArmatureComponent::GetIndexTransform(int32 Index) const
{
	if (Index != INDEX_NONE)
	{
		// Get bone transform
		USkeletalMeshComponent* Mesh = GetSkeletalMesh();
		if (IsValid(Mesh) && Index < Mesh->GetNumBones())
		{
			const FTransform ParentTransform = GetComponentTransform();

			const FTransform MeshTransform = Mesh->GetComponentTransform();
			const FTransform& BoneTransform = Mesh->GetBoneTransform(Index, MeshTransform);

			FTGOR_MovementDynamic Dynamic;
			Dynamic.Linear = ParentTransform.InverseTransformPositionNoScale(BoneTransform.GetLocation());
			Dynamic.Angular = ParentTransform.InverseTransformRotation(BoneTransform.GetRotation());
			return Dynamic;
		}
	}
	return Super::GetIndexTransform(Index);
}

int32 UTGOR_ArmatureComponent::GetIndexFromName(const FName& Name) const
{
	if (!Name.IsNone())
	{
		USkeletalMeshComponent* Mesh = GetSkeletalMesh();
		if (IsValid(Mesh))
		{
			const int32 Index = Mesh->GetBoneIndex(Name);
			if (Index != INDEX_NONE)
			{
				return Index;
			}
		}
	}
	return Super::GetIndexFromName(Name);
}

FName UTGOR_ArmatureComponent::GetNameFromIndex(int32 Index) const
{
	if (Index >= 0)
	{
		USkeletalMeshComponent* Mesh = GetSkeletalMesh();
		if (IsValid(Mesh))
		{
			const int32 Num = Mesh->GetNumBones();
			if (Index < Num)
			{
				return Mesh->GetBoneName(Index);
			}
		}
	}
	return Super::GetNameFromIndex(Index);
}

bool UTGOR_ArmatureComponent::IsValidIndex(int32 Index) const
{
	USkeletalMeshComponent* Mesh = GetSkeletalMesh();
	if (IsValid(Mesh) && 0 <= Index && Index < Mesh->GetNumBones())
	{
		return true;
	}
	return Super::IsValidIndex(Index);
}


void UTGOR_ArmatureComponent::AttachToMovement(USceneComponent* Component, int32 Index)
{
	if (Index == INDEX_NONE)
	{
		Super::AttachToMovement(Component, Index);
		return;
	}

	const FName Name = GetNameFromIndex(Index);
	USkeletalMeshComponent* Mesh = GetSkeletalMesh();

	check(!Mesh->IsAttachedTo(Component) && "Attachment loop detected");
	Component->AttachToComponent(Mesh, FAttachmentTransformRules::KeepWorldTransform, Name);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

USkeletalMeshComponent* UTGOR_ArmatureComponent::GetSkeletalMesh() const
{
	return GetOwnerComponent<USkeletalMeshComponent>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
