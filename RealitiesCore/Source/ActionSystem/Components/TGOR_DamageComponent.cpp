// The Gateway of Realities: Planes of Existence.

#include "TGOR_DamageComponent.h"

#include "TargetSystem/Content/TGOR_Target.h"

#include "TargetSystem/Components/TGOR_InteractableComponent.h"


UTGOR_DamageComponent::UTGOR_DamageComponent()
	: Super()
{
}

void UTGOR_DamageComponent::HitAround(const FVector& Origin, float Radius, const FTGOR_ForceInstance& Forces)
{
	UWorld* World = GetWorld();

	// Generate Trace params
	TArray<AActor*> Ignore;
	FCollisionQueryParams TraceParams(FName(TEXT("ProjectileTrace")), true, GetOwner());
	TraceParams.bTraceComplex = false;
	TraceParams.bReturnFaceIndex = false;
	TraceParams.bReturnPhysicalMaterial = false;

	// Generate shape info
	const FCollisionShape Shape = FCollisionShape::MakeSphere(Radius);

	// Find interactables
	TArray<FOverlapResult> Overlaps;
	if (World->OverlapMultiByChannel(Overlaps, Origin, FQuat::Identity, HitChannel, Shape, TraceParams))
	{
		for (const FOverlapResult& Overlap : Overlaps)
		{
			UTGOR_InteractableComponent* Interactable = Cast<UTGOR_InteractableComponent>(Overlap.GetComponent());
			if (IsValid(Interactable))
			{
				for (UTGOR_Target* Target : Interactable->Targets)
				{
					FTGOR_AimPoint Point;
					if (Target->OverlapTarget(Interactable, Origin, Radius, Point))
					{
						FTGOR_InfluenceInstance Influence;
						Influence.Target = Point.Target;
						Influence.Forces = Forces * (1.0f - Point.Distance);
						Influence.Instigator = GetOwner();
						Influence.Location = Origin;

						Interactable->Influence(Influence);
					}
				}
			}
		}
	}
}

void UTGOR_DamageComponent::HitAroundSelf(const FTGOR_ForceInstance& Forces)
{
	const FVector Origin = GetComponentLocation();
	const float Radius = GetScaledSphereRadius();
	HitAround(Origin, Radius, Forces);
}

