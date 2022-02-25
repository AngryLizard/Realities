// The Gateway of Realities: Planes of Existence.


#include "TGOR_ProjectileMovementComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"


UTGOR_ProjectileMovementComponent::UTGOR_ProjectileMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	State.Rotator = FRotator(0.0f, 0.0f, 0.0f);
	State.Location = FVector(0.0f, 0.0f, 0.0f);
	State.Velocity = FVector(0.0f, 0.0f, 0.0f);

	VelocityThreshold = 10.0f;
	ReplicationRate = 0.1f;
	_timer = 0.0f;
}

void UTGOR_ProjectileMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTGOR_ProjectileMovementComponent, State);
}

void UTGOR_ProjectileMovementComponent::BeginPlay()
{

}

void UTGOR_ProjectileMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	_timer -= DeltaTime;
	if (_timer <= 0.0f)
	{
		_timer = ReplicationRate;
		AActor* Actor = GetOwner();
		if (Actor->HasAuthority())
		{
			float Squared = VelocityThreshold * VelocityThreshold;
			if (FVector::DistSquared(State.Velocity, Velocity) > VelocityThreshold)
			{
				State.Rotator = Actor->GetActorRotation();
				State.Location = Actor->GetActorLocation();
				State.Velocity = Velocity;
			}
		}
	}
}

void UTGOR_ProjectileMovementComponent::OnReplicateState()
{
	AActor* Actor = GetOwner();

	Actor->SetActorRotation(State.Rotator);
	Actor->SetActorLocation(State.Location);
	Velocity = State.Velocity;
}