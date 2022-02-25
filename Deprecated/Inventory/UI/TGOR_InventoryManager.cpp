// The Gateway of Realities: Planes of Existence.

#include "TGOR_InventoryManager.h"
#include "Realities.h"


FVector UTGOR_InventoryManager::ScreenPositionToWorldLocation(APawn* Pawn, FVector2D ScreenPosition, ETraceTypeQuery Query, float Height, float Distance, ETGOR_ComputeEnumeration& Branches)
{
	Branches = ETGOR_ComputeEnumeration::Failed;

	ATGOR_PlayerController* Controller = Cast<ATGOR_PlayerController>(Pawn->GetController());
	if (Controller == nullptr) return(FVector());

	FVector HitLocation = Controller->HitTraceFrom(ScreenPosition, Query);

	FHitResult Hit;
	TArray<AActor*> Ignore;
	FVector Direction = FVector(0.0f, 0.0f, 1.0f);
	FVector Location = HitLocation + Direction;
	FVector End = Location + Direction * Height * 2;
	if (UKismetSystemLibrary::LineTraceSingle(Pawn, Location, End, Query, false, Ignore, EDrawDebugTrace::Type::None, Hit, true)) return(FVector());

	Location = Location + Direction * Height;

	FVector PawnLocation = Pawn->GetActorLocation();
	FVector PawnDifference = Location - PawnLocation;
	float PawnDistance = PawnDifference.Size();

	Branches = ETGOR_ComputeEnumeration::Success;
	if (PawnDistance < Distance) return(Location);

	return(PawnLocation);
}