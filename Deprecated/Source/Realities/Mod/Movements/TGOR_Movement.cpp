// The Gateway of Realities: Planes of Existence.


#include "TGOR_Movement.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Components/Movement/TGOR_MovementComponent.h"
#include "Realities/Components/Creature/TGOR_ModularSkeletalMeshComponent.h"
#include "Realities/Mod/Spawner/Creatures/TGOR_Creature.h"
#include "Realities/Mod/Animations/TGOR_Animation.h"

UTGOR_Movement::UTGOR_Movement()
	: Super(),
	CanRotateOnCollision(false),
	Mode(ETGOR_MovementEnumeration::Queued)
{
}

void UTGOR_Movement::PostBuildResource()
{
	Super::PostBuildResource();

	// Override inserted
	TArray<UTGOR_Movement*> Movements = GetIListFromType<UTGOR_Movement>();
	for (UTGOR_Movement* Movement : Movements)
	{
		Movement->OverrideMovements(this);
	}
}

void UTGOR_Movement::OverrideMovements(UTGOR_Movement* Override)
{
	// Check for cycles
	if (Override == this)
	{
		ERROR(FString("Movement override cycle found in ") + GetDefaultName(), Error);
	}

	// Don't need to add if already there (Should never happen with no cycles, but ye know)
	if (!OverriddenBy.Contains(Override))
	{
		// Add to list
		OverriddenBy.Emplace(Override);

		// Add to children
		TArray<UTGOR_Movement*> Movements = GetIListFromType<UTGOR_Movement>();
		for (UTGOR_Movement* Movement : Movements)
		{
			Movement->OverrideMovements(Override);
		}
	}
}


void UTGOR_Movement::QueryInputVector(const UTGOR_MovementComponent* Component, FVector& OutInput, FVector& OutView) const
{
	const FQuat InputDirection = Component->GetInputRotation();
	const float InputStrength = Component->GetInputStrength();
	const FVector RawInput = Component->GetRawInput();
	const float Size = RawInput.Size();
	if (Size >= SMALL_NUMBER)
	{
		const FVector Normal = RawInput / Size;
		OutInput = Normal * FMath::Min(Size, 1.0f) * InputStrength;
		OutView = InputDirection.GetAxisX();
		return;
	}

	OutInput = FVector::ZeroVector;
	OutView = FVector::ZeroVector;
}

void UTGOR_Movement::Tick(UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, bool Replay, FTGOR_MovementOutput& Out)
{

}

bool UTGOR_Movement::Invariant(const UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	const FTGOR_MovementFrame& Frame = Component->GetFrame();
	return Frame.Strength > SMALL_NUMBER;
}

UTGOR_Animation* UTGOR_Movement::QueryAnimation(const UTGOR_MovementComponent* Component, const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	const FTGOR_MovementContent& Content = Component->GetContent();
	UTGOR_Animation* OldAnimation = Content.Animation;
	if (IsValid(Component->Pawn))
	{
		UTGOR_Spawner* Spawner = Component->Pawn->GetSpawnerContent();
		if (IsValid(Spawner))
		{
			// Don't need to update if already valid
			if (OldAnimation && OldAnimation->IsA(MainAnimation))
			{
				return OldAnimation;
			}

			// Get matching animation content
			return Spawner->GetIFromType<UTGOR_Animation>(MainAnimation);
		}
	}
	return nullptr;
}

bool UTGOR_Movement::IsOverriddenBy(UTGOR_Movement* Movement) const
{
	return OverriddenBy.Contains(Movement);
}

void UTGOR_Movement::GetAnimation(const UTGOR_MovementComponent* Component, UTGOR_Animation*& Animation, USkeletalMeshComponent*& Mesh) const
{
	Mesh = Component->Pawn->GetSkeletalMesh();
	Animation = Component->GetContent().Animation;
}