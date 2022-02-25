// The Gateway of Realities: Planes of Existence.


#include "TGOR_InfluenceComponent.h"

#include "Realities/Components/Dimension/TGOR_InfluencableComponent.h"


UTGOR_InfluenceComponent::UTGOR_InfluenceComponent()
{
	bDebug = false;
	InfluenceRadius = 100.0f;
}


void UTGOR_InfluenceComponent::RegionTick(float DeltaTime)
{
	Super::RegionTick(DeltaTime);
	
	// Get influencable nearby
	CallNearby<UTGOR_InfluencableComponent>([this](UTGOR_InfluencableComponent* Component, float Ratio)
	{
		if (bDebug) UTGOR_Error::DebugTrace(Component->GetWorld(), Component->GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation(), FColor(0xF00));
		
		// Accumulate influence
		Component->AccumulateAmbient(InfluenceState * Ratio * Ratio);

	}, InfluenceRadius);
}
