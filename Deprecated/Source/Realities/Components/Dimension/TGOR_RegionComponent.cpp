// The Gateway of Realities: Planes of Existence.


#include "TGOR_RegionComponent.h"

#include "Realities/Actors/Dimension/TGOR_MainRegionActor.h"
#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Base/TGOR_GameState.h"

// Sets default values for this component's properties
UTGOR_RegionComponent::UTGOR_RegionComponent()
	: Super()
{
	_current = -1;
	_section = -1;

	PrimaryComponentTick.bCanEverTick = true;
	bActiveRegionComponent = false;

	Current = nullptr;
	Opposite = nullptr;
	Prev = nullptr;
	Next = nullptr;
	MainRegion = nullptr;
}

// Called when the game starts
void UTGOR_RegionComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UTGOR_RegionComponent::BeginDestroy()
{
	Super::BeginDestroy();

	if (Current != nullptr)
	{
		Current->UnregisterComponent(this);
	}
}

void UTGOR_RegionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UTGOR_RegionComponent::PostAssemble(UTGOR_DimensionData* Dimension)
{
	ITGOR_DimensionInterface::PostAssemble(Dimension);

	if (GetOwner()->HasAuthority())
	{
		DIMENSION_RETCHK(false)

		// Reset
		ClearRegion();
		MainRegion = Dimension->GetMainRegion();
		
		// Ensure construction (Only first region to call this will actually trigger the construction)
		MainRegion->Construct();

		// Initialise this region
		Update();

		// Assume actor was spawned as map-actor if called with assembly
		RegionStart(Dimension->IsActorsAssembled());
	}

	return(true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////


ATGOR_RegionActor* UTGOR_RegionComponent::GetCurrentRegion() const
{
	return Current;
}

ATGOR_MainRegionActor* UTGOR_RegionComponent::GetMainRegion() const
{
	return MainRegion;
}



void UTGOR_RegionComponent::RegionStart(bool Spawned)
{

}

void UTGOR_RegionComponent::RegionTick( float DeltaTime )
{

}


void UTGOR_RegionComponent::CallNearby(const FProximityDelegate& Delegate, float Radius, bool IgnoreSelf)
{
	// Call helper function
	CallNearby<UTGOR_RegionComponent>([this, Delegate, IgnoreSelf](UTGOR_RegionComponent* Component, float Ratio)
	{ 
		if (Component != this || !IgnoreSelf)
		{ 
			Delegate.ExecuteIfBound(Component, Ratio); 
		}
	}, Radius);
}


void UTGOR_RegionComponent::Update()
{
	SINGLETON_CHK
	DIMENSION_CHK
	
	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{ return; }

	// Compute Actor location
	FVector Location = Owner->GetActorLocation();

	// Construct search actor if not already constructed
	if (!_search.isValid())
	{
		if (!DimensionData->Construct(_search))
		{
			return;
		}
	}

	// Update search actor
	_search.update(Voronoi::Vector(Location.X, Location.Y));

	// Change region if changed
	ETGOR_FetchEnumeration State;
	int current = _search.getID();	
	if (current != _current)
	{
		_current = current;
		
		// Unregister from current region
		if (IsValid(Current))
		{ 
			Current->UnregisterComponent(this); 
		}
		
		// Register in new region and call delegate
		Current = MainRegion->GetRegion(_current, State);
		if (State == ETGOR_FetchEnumeration::Found)
		{
			// Reset section
			Opposite = Prev = Next = Current;

			Current->RegisterComponent(this);
			OnRegionEntered.Broadcast(Current);
		}
	}
	
	int section = _search.getSubID();
	if (section != _section)
	{
		_section = section;

		int32 OppositeID, PrevID, NextID;
		_search.getSurroundings(OppositeID, PrevID, NextID);

		// Ensure opposite region
		Opposite = MainRegion->GetRegion(OppositeID, State);
		if (!IsValid(Opposite))
		{
			ERROR("Couldn't find opposite.", Error)
		}

		// Ensure prev region
		Prev = MainRegion->GetRegion(PrevID, State);
		if (!IsValid(Prev))
		{
			Prev = Current;
		}

		// Ensure next region
		Next = MainRegion->GetRegion(NextID, State);
		if (!IsValid(Next))
		{
			Next = Current;
		}
		
		OnSectionEntered.Broadcast(Opposite, Prev, Next);
	}
}

void UTGOR_RegionComponent::ClearRegion()
{
	// Reset cache
	_search.reset();
	_current = -1;
	
	// Unregister from current region
	if (Current != nullptr)
	{
		Current->UnregisterComponent(this); 
	}

	// Reset regions
	Current = nullptr;
}
