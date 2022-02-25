// The Gateway of Realities: Planes of Existence.


#include "TGOR_RegionActor.h"

#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Actors/Dimension/TGOR_MainRegionActor.h"
#include "Realities/Components/Dimension/TGOR_RegionComponent.h"
#include "Realities/Components/Dimension/TGOR_IdentityComponent.h"
#include "EngineUtils.h"

RegionCell::RegionCell()
	: Voronoi::Cell(Voronoi::Vector(), -1)
{
}

void RegionCell::set(FVector v, int id)
{
	center.p.x = floor(v.X);
	center.p.y = floor(v.Y);

	_id = id;
}


ATGOR_RegionActor::ATGOR_RegionActor()
	: _tickBuffer(0.0f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RegionUpdateRate = 0.2f;
	ActiveComponents = 0;
}


void ATGOR_RegionActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATGOR_RegionActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	
	if (ActiveComponents == 0) return;
	
	// Increase counter
	_tickBuffer += DeltaTime;
	if (_tickBuffer > RegionUpdateRate)
	{
		// Tick every component
		int32 Num = RegionInstance.Register.Num();
		for (int i = Num-1; i >= 0; i--)
		{
			FTGOR_ActorState& Instance = RegionInstance.Register[i];
			UTGOR_RegionComponent* Component = Instance.Component;
			if (IsValid(Component))
			{
				// Update location
				AActor* Actor = Component->GetOwner();
				if (IsValid(Actor))
				{
					Instance.Cache = FVector2D(Actor->GetActorLocation());
				}

				// Tick component
				Component->RegionTick(_tickBuffer);

				// Update component
				Component->Update();
			}
		}

		// Reset ticker
		_tickBuffer = 0.0f;
	}
}


bool ATGOR_RegionActor::Assemble(UTGOR_DimensionData* Dimension)
{
	Super::Assemble(Dimension);

	ATGOR_MainRegionActor* MainRegion = Dimension->GetMainRegion();
	if (IsValid(MainRegion))
	{
		MainRegion->AddRegion(this);
	}

	return(true);
}

int32 ATGOR_RegionActor::FindComponent(UTGOR_RegionComponent* Component)
{
	for (int32 i = 0; i < RegionInstance.Register.Num(); i++)
	{
		FTGOR_ActorState& Instance = RegionInstance.Register[i];
		if (Component == Instance.Component)
		{
			return(i);
		}
	}
	return(-1);
}

void ATGOR_RegionActor::RegisterComponent(UTGOR_RegionComponent* Component)
{
	// Find index of component and make sure it's not already in it
	int32 Index = FindComponent(Component);
	if (Index != -1) return;

	// Remove component
	RegionInstance.Register.Add(FTGOR_ActorState(Component));

	// If component is active, add to active list
	if (Component->bActiveRegionComponent)
	{ 
		ActiveComponents++;
		for (ATGOR_RegionActor* Region : Neighbours)
		{
			Region->ActiveComponents++;
		}
	}
}

void ATGOR_RegionActor::UnregisterComponent(UTGOR_RegionComponent* Component)
{
	// Find index of component and make sure it's in it
	int32 Index = FindComponent(Component);
	if (Index == -1) return;

	// If component is active, remove from active list
	if (Component->bActiveRegionComponent)
	{ 
		ActiveComponents--;
		for (ATGOR_RegionActor* Region : Neighbours)
		{
			Region->ActiveComponents--;
		}
	}

	// Remove registered component
	RegionInstance.Register.RemoveAt(Index);
}

bool ATGOR_RegionActor::HasActiveComponents() const
{
	return(ActiveComponents > 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const FTGOR_ElementInstance& ATGOR_RegionActor::GetTop() const
{
	return Top;
}

const FTGOR_ElementInstance& ATGOR_RegionActor::GetBottom() const
{
	return Bottom;
}

float ATGOR_RegionActor::GetCeiling() const
{
	return Ceiling;
}

float ATGOR_RegionActor::GetFloor() const
{
	return Floor;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

RegionCell& ATGOR_RegionActor::Construct(int ID)
{
	// Get dimensions
	FVector Origin = GetActorLocation();
	float Height = ComputeLocationAndHeight(Origin);

	// Get floor and ceiling
	Floor = Origin.Z;
	Ceiling = Origin.Z + Height;
	
	// reset, populate and return cell
	_cell.set(Origin, ID);
	return(_cell);
}


RegionCell& ATGOR_RegionActor::GetCell()
{
	return(_cell);
}

#if WITH_EDITOR
void ATGOR_RegionActor::DebugIndependentDraw()
{
	if (!IsValid(GetWorld())) return;

	// Find all dimensions
	for (TActorIterator<AActor> a(GetWorld()); a; ++a)
	{
		if (a->IsA(ATGOR_MainRegionActor::StaticClass()))
		{
			ATGOR_MainRegionActor* MainRegion = Cast<ATGOR_MainRegionActor>(*a);
			if (MainRegion != nullptr)
			{
				MainRegion->DebugIndependentDraw();
			}
		}
	}
}


void ATGOR_RegionActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	DebugIndependentDraw();
}

void ATGOR_RegionActor::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);

	DebugIndependentDraw();
}
#endif