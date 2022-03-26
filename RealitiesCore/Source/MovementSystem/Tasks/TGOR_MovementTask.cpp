// The Gateway of Realities: Planes of Existence.

#include "TGOR_MovementTask.h"
#include "AnimationSystem/Components/TGOR_HandleComponent.h"
#include "AnimationSystem/Components/TGOR_AttachComponent.h"
#include "MovementSystem/Components/TGOR_MovementComponent.h"
#include "AttributeSystem/Components/TGOR_AttributeComponent.h"
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

TScriptInterface<ITGOR_AnimationInterface> UTGOR_MovementTask::GetAnimationOwner() const
{
	return GetOwnerComponent();
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
	return UTGOR_AttributeComponent::GetAttributeValue(Identifier.Component->GetOwner(), Type, Default);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_MovementTask::GetMaxSpeed() const
{
	return 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_MovementTask::Initialise()
{
	Identifier.Content->TaskInitialise(this);
	OnInitialise();
}

void UTGOR_MovementTask::PrepareStart()
{
	PlayAnimation();
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

void UTGOR_MovementTask::Process(float DeltaTime)
{
	ConsumeRootMotion(DeltaTime);
}

void UTGOR_MovementTask::Context(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick)
{
}

void UTGOR_MovementTask::Update(const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Out)
{
}

void UTGOR_MovementTask::Interrupt()
{
	ResetAnimation();
}

void UTGOR_MovementTask::Animate(const FTGOR_MovementSpace& Space, float DeltaTime)
{
	OnAnimate(Space, DeltaTime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
