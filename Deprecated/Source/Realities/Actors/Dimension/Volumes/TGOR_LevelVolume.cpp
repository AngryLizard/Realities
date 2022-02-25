// The Gateway of Realities: Planes of Existence.

#include "TGOR_LevelVolume.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Mod/Targets/TGOR_WorldTarget.h"
#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Components/Dimension/Interactable/TGOR_WorldInteractableComponent.h"

// Sets default values
ATGOR_LevelVolume::ATGOR_LevelVolume(const FObjectInitializer& ObjectInitializer)
:	Super(ObjectInitializer)
{
	IsNetworkVolume = true;

	InteractableComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_WorldInteractableComponent>(this, FName(TEXT("InteractableComponent")));
	InteractableComponent->SetupAttachment(GetRootComponent());
	InteractableComponent->TargetClasses.Emplace(UTGOR_WorldTarget::StaticClass());
}

bool ATGOR_LevelVolume::PreAssemble(UTGOR_DimensionData* Dimension)
{
	Dimension->LevelVolume = this;
	
	return(ATGOR_BoxPhysicsVolume::PreAssemble(Dimension));
}

////////////////////////////////////////////////////////////////////////////////////////////////////


void ATGOR_LevelVolume::SetExternal(const FVector& Force)
{
	Gravity = Force.Size();
	if (Gravity >= SMALL_NUMBER)
	{
		UpVector = Force / -Gravity;
	}
}
