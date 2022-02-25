// The Gateway of Realities: Planes of Existence.


#include "TGOR_InteractionOverlay.h"

#include "Realities/UI/Queries/System/TGOR_InteractionQuery.h"
#include "Realities/Components/Combat/TGOR_AimComponent.h"
#include "Realities/Actors/Pawns/TGOR_Pawn.h"
#include "Realities/Mod/Actions/TGOR_Action.h"

#define LOCTEXT_NAMESPACE "InteractionOverlay"

UTGOR_InteractionOverlay::UTGOR_InteractionOverlay(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InteractionQuery = ObjectInitializer.CreateDefaultSubobject<UTGOR_InteractionQuery>(this, TEXT("InteractionQuery"));
	
}

void UTGOR_InteractionOverlay::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UTGOR_InteractionOverlay::OnWidgetRebuilt()
{
	Super::OnWidgetRebuilt();
}


#undef LOCTEXT_NAMESPACE