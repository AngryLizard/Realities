// The Gateway of Realities: Planes of Existence.

#include "TGOR_MovementTask.h"
#include "AnimationSystem/Components/TGOR_HandleComponent.h"
#include "AnimationSystem/Components/TGOR_AttachComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"
#include "MovementSystem/Content/TGOR_Movement.h"
#include "AttributeSystem/Content/TGOR_Attribute.h"
#include "DimensionSystem/Content/TGOR_Primitive.h"

UTGOR_MovementTask::UTGOR_MovementTask()
: Super()
{
}

void UTGOR_MovementTask::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_MovementTask, Identifier, COND_InitialOnly);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_Movement* UTGOR_MovementTask::GetMovement() const
{
	return Identifier.Content;
}


UTGOR_MovementComponent* UTGOR_MovementTask::GetOwnerComponent() const
{
	return Identifier.Component;
}

int32 UTGOR_MovementTask::GetSlotIndex() const
{
	return Identifier.Slot;
}

float UTGOR_MovementTask::GetSpeedRatio() const
{
	const FTGOR_MovementFrame& Frame = Identifier.Component->GetFrame();
	return Frame.Agility * GetAttribute(Identifier.Content->SpeedAttribute, 1.0f);
}


float UTGOR_MovementTask::GetAttribute(TSubclassOf<UTGOR_Attribute> Type, float Default) const
{
	UTGOR_Attribute* Attribute = Identifier.Content->Instanced_AttributeInsertions.GetOfType(Type);//GetIFromType<UTGOR_Attribute>(Type);
	return Identifier.Component->Execute_GetAttribute(Identifier.Component, Attribute, Default);
}

void UTGOR_MovementTask::GetHandleComponents(TArray<UTGOR_HandleComponent*>& Handles, const TSet<TSubclassOf<UTGOR_Primitive>>& Types, bool EnforceCone) const
{
	TArray<UTGOR_HandleComponent*> Components = Identifier.Component->GetOwnerComponents<UTGOR_HandleComponent>();
	for (UTGOR_HandleComponent* Component : Components)
	{
		if (!EnforceCone || IsValid(Component->MovementCone))
		{
			for (TSubclassOf<UTGOR_Primitive> Type : Types)
			{
				if (Component->SpawnPrimitive->IsChildOf(Type))
				{
					Handles.Emplace(Component);
					break;
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_MovementTask::Initialise()
{
	Identifier.Content->TaskInitialise(this);
	OnInitialise();
}

bool UTGOR_MovementTask::Invariant(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External) const
{
	// Default implementation disallows movement if we're unconscious
	const FTGOR_MovementFrame& Frame = Identifier.Component->GetFrame();
	return Frame.Strength > SMALL_NUMBER;
}

void UTGOR_MovementTask::Reset(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External)
{
	
}

void UTGOR_MovementTask::QueryInput(FVector& OutInput, FVector& OutView) const
{
	// Default implementation straightly outputs input direction as given
	const FQuat InputDirection = Identifier.Component->GetInputRotation();
	const float InputStrength = Identifier.Component->GetInputStrength();
	const FVector RawInput = Identifier.Component->GetRawInput();
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

void UTGOR_MovementTask::Update(FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Out)
{

}

void UTGOR_MovementTask::Animate(const FTGOR_MovementSpace& Space, float DeltaTime)
{
	OnAnimate(Space, DeltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
