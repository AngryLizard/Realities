// The Gateway of Realities: Planes of Existence.

#include "TGOR_HitVolumeComponent.h"

#include "Realities/Actors/Dimension/Volumes/TGOR_PhysicsVolume.h"
#include "Realities/Actors/Dimension/Volumes/TGOR_LevelVolume.h"
#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Actors/Combat/HitVolumes/TGOR_HitVolume.h"
#include "Realities/Base/TGOR_Singleton.h"

FTGOR_HitVolumeHandle::FTGOR_HitVolumeHandle()
	: HitVolume(nullptr),
	Autonomous(false)
{
}

UTGOR_HitVolumeComponent::UTGOR_HitVolumeComponent()
	: Super()
{
}

void UTGOR_HitVolumeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Poll for inactive handles to be removed
	for (auto It = HitVolumeHandles.CreateIterator(); It; ++It)
	{
		if (!IsValid(It->Value.HitVolume))
		{
			It.RemoveCurrent();
		}
		else if(!It->Value.Autonomous)
		{
			const TSet<UTGOR_Content*> ContentContext = It->Value.Register->Execute_GetActiveContent(It->Value.Register.GetObject());
			if (!ContentContext.Contains(It->Key))
			{
				It->Value.HitVolume->Kill();
				It.RemoveCurrent();
			}
		}
	}
}


void UTGOR_HitVolumeComponent::ShootHitVolume(TScriptInterface<ITGOR_RegisterInterface> Register, UTGOR_Content* Content, TSubclassOf<ATGOR_HitVolume> Type, const FTransform& Transform, const FTGOR_ForceInstance& Forces, bool Autonomous)
{
	ATGOR_HitVolume* HitVolume;

	FTGOR_HitVolumeHandle* Ptr = HitVolumeHandles.Find(Content);
	if (Ptr && IsValid(Ptr->HitVolume) && Ptr->HitVolume->IsA(Type))
	{
		HitVolume = Ptr->HitVolume;
		Ptr->HitVolume->SetActorTransform(Transform);
	}
	else
	{
		// If a wrong type already exists, kill it first
		KillHitVolume(Content, false);

		// Spawn HitVolume
		UWorld* World = GetWorld();
		FActorSpawnParameters Params = FActorSpawnParameters();
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		HitVolume = World->SpawnActor<ATGOR_HitVolume>(Type, Transform, Params);
		if (!IsValid(HitVolume))
		{
			ERROR("Spawning HitVolume failed", Error);
		}

		// Add to handles
		FTGOR_HitVolumeHandle Handle;
		Handle.HitVolume = HitVolume;
		Handle.Register = Register;
		Handle.Autonomous = Autonomous;
		HitVolumeHandles.Add(Content, Handle);
	}

	// Assign level volume
	ETGOR_FetchEnumeration State;
	UTGOR_DimensionData* Dimension = GetDimension(State);
	if (State == ETGOR_FetchEnumeration::Found)
	{
		HitVolume->AssignOwnerVolume(Dimension->GetLevelVolume());
	}

	HitVolume->AssignOwnerInstigator(GetOwner());
	HitVolume->Shoot(AimInstance, Forces);
}


void UTGOR_HitVolumeComponent::KillHitVolume(UTGOR_Content* Content, bool Autonomous)
{
	FTGOR_HitVolumeHandle Handle;
	HitVolumeHandles.RemoveAndCopyValue(Content, Handle);
	if (IsValid(Handle.HitVolume) && (Autonomous || !Handle.Autonomous))
	{
		Handle.HitVolume->Kill();
	}
}
