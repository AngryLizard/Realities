// The Gateway of Realities: Planes of Existence.


#include "TGOR_PhysicsVolume.h"
#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Components/Movement/TGOR_PilotComponent.h"
#include "Realities/Components/Dimension/TGOR_IdentityComponent.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/System/Data/TGOR_WorldData.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATGOR_PhysicsVolume::ATGOR_PhysicsVolume(const FObjectInitializer& ObjectInitializer)
:	Super(ObjectInitializer),
	Parent(nullptr),
	IsAdditive(false),
	UpVector(FVector::UpVector),
	Gravity(1962.0f),
	Density(1.0f),
	DragCoeff(0.001f),
	IsNetworkVolume(false),
	NetworkCell(nullptr)
{
	bReplicates = true;

	MobilityComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_PilotComponent>(this, FName(TEXT("PilotComponent")));
	IdentityComponent = ObjectInitializer.CreateDefaultSubobject<UTGOR_IdentityComponent>(this, FName(TEXT("IdentityComponent")));
}

// Called when the game starts or when spawned
void ATGOR_PhysicsVolume::BeginPlay()
{
	Super::BeginPlay();
	
	SINGLETON_CHK

	UTGOR_WorldData* WorldData = Singleton->GetWorldData();
	ATGOR_PhysicsVolume* MainVolume = WorldData->GetMainVolume();
	if (IsValid(MainVolume))
	{
		MainVolume->Insert(this);
	}
}

void ATGOR_PhysicsVolume::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(Singleton))
	{
		UTGOR_WorldData* WorldData = Singleton->GetWorldData();
		ATGOR_PhysicsVolume* MainVolume = WorldData->GetMainVolume();
		if (IsValid(MainVolume))
		{
			MainVolume->Remove(this);
		}
	}
	Super::EndPlay(EndPlayReason);
}

void ATGOR_PhysicsVolume::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate attachment status to everyone
	DOREPLIFETIME_CONDITION(ATGOR_PhysicsVolume, UpVector, COND_None);
	DOREPLIFETIME_CONDITION(ATGOR_PhysicsVolume, Gravity, COND_None);
}

bool ATGOR_PhysicsVolume::PreAssemble(UTGOR_DimensionData* Dimension)
{
	return ITGOR_DimensionInterface::PreAssemble(Dimension);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool ATGOR_PhysicsVolume::IsInside(const FVector& Location) const
{
	return false;
}

ETGOR_BoundaryTest ATGOR_PhysicsVolume::TestBoundary(const FBox& Box) const
{
	const FBox BoundingBox = ComputeBoundingBox();
	if (BoundingBox.IsInside(Box))
	{
		return ETGOR_BoundaryTest::Inside;
	}
	else if(Box.IsInside(BoundingBox))
	{
		return ETGOR_BoundaryTest::Engulfing;
	}
	else if(BoundingBox.Intersect(Box))
	{
		return ETGOR_BoundaryTest::Overlapping;
	}
	return ETGOR_BoundaryTest::Disjunct;
}


float ATGOR_PhysicsVolume::ComputeVolume() const
{
	return 0.0f;
}

FBox ATGOR_PhysicsVolume::ComputeBoundingBox() const
{
	return FBox();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ATGOR_PhysicsVolume* ATGOR_PhysicsVolume::Query(const FVector& Location)
{
	for (ATGOR_PhysicsVolume* Child : Volumes)
	{
		if (!IsValid(Child))
		{
			ERRET("Invalid volume", Error, nullptr);
		}

		if (Child->IsInside(Location))
		{
			return(Child->Query(Location));
		}
	}
	return this;
}

ATGOR_PhysicsVolume* ATGOR_PhysicsVolume::QueryNetwork(const FVector& Location)
{
	for (ATGOR_PhysicsVolume* Child : Volumes)
	{
		if (Child->IsNetworkVolume && Child->IsInside(Location))
		{
			return(Child->Query(Location));
		}
	}
	return this;
}

void ATGOR_PhysicsVolume::Insert(ATGOR_PhysicsVolume* Other)
{
	if (Volumes.Contains(Other) || Other == this)
	{
		return;
	}
	
	bool IsAllocated = false;

	// Test all children
	const FBox Boundary = Other->ComputeBoundingBox();
	auto Iter = Volumes.CreateIterator();
	for (; Iter; Iter++)
	{
		ATGOR_PhysicsVolume* Child = *Iter;
		const ETGOR_BoundaryTest Test = Child->TestBoundary(Boundary);
		if (Test == ETGOR_BoundaryTest::Inside)
		{
			Child->Insert(Other);
			IsAllocated = true;
		}
		else if (Test == ETGOR_BoundaryTest::Engulfing)
		{
			// Replace child with new volume and put volume inside of it
			Other->Insert(Child);
			Iter.RemoveCurrent();
		}
		else if (Test == ETGOR_BoundaryTest::Overlapping)
		{
			// Add to multiple children if overlapping
			if (Child->ComputeVolume() > Other->ComputeVolume())
			{
				Child->Insert(Other);
			}
			else
			{
				Other->Insert(Child);
			}
		}
	}

	// If not fully inside of a child, add to adjunt list
	if (!IsAllocated)
	{
		Volumes.Emplace(Other);
	}
	Other->Parent = this;
}

void ATGOR_PhysicsVolume::Remove(ATGOR_PhysicsVolume* Other)
{
	TArray<ATGOR_PhysicsVolume*> Spill;
	auto Iter = Volumes.CreateIterator();
	for (; Iter; Iter++)
	{
		ATGOR_PhysicsVolume* Child = *Iter;
		Child->Remove(Other);

		// Remove child and spill its content on match
		if (Child == Other)
		{
			Iter.RemoveCurrent();
			Spill.Append(Child->Volumes);
		}
	}

	Volumes.Append(Spill);
	for (ATGOR_PhysicsVolume* Volume : Spill)
	{
		Volume->Parent = this;
	}
}


FString ATGOR_PhysicsVolume::Print(const FString& Prefix) const
{
	ETGOR_FetchEnumeration State;
	const FName DimensionIdentifier = GetDimensionIdentifier(State);

	FString Out = Prefix + GetName() + " - " + DimensionIdentifier.ToString();
	if (DimensionData.IsValid())
	{
		FName Identifier = DimensionData->GetIdentifier();
		Out = Out + "|" + Identifier.ToString();
	}
	Out = Out + "\n ";

	const FString Indent = Prefix + "--";
	for (const ATGOR_PhysicsVolume* Child : Volumes)
	{
		if (IsValid(Child))
		{
			Out += Prefix + Child->Print(Indent);
		}
	}
	return Out;
}

ATGOR_PhysicsVolume* ATGOR_PhysicsVolume::GetParentVolume() const
{
	return Parent.Get();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_PhysicsProperties ATGOR_PhysicsVolume::ComputeSurroundings(const FVector& Location) const
{
	const FTransform Transform = GetActorTransform();
	const FVector Local = Transform.InverseTransformPosition(Location);
	const FVector Direction = Transform.TransformVectorNoScale(UpVector);


	FTGOR_PhysicsProperties Properties;
	Properties.Depth = 0.0f;
	Properties.Density = Density;
	Properties.DragCoeff = DragCoeff;
	Properties.Gravity = Direction * -Gravity;

	if (IsValid(MobilityComponent))
	{
		const FTGOR_MovementSpace Space = MobilityComponent->ComputeSpace();
		Properties.Velocity = Space.PointVelocity(Location);
		Properties.Vorticity = Space.AngularVelocity;
	}

	return Properties;
}

FTGOR_PhysicsProperties ATGOR_PhysicsVolume::ComputeAllSurroundings(const FVector& Location) const
{
	FTGOR_PhysicsProperties Properties = ComputeSurroundings(Location);
	if (IsAdditive && Parent.IsValid())
	{
		const FTGOR_PhysicsProperties ParentProperties = Parent->ComputeAllSurroundings(Location);
		Properties.Density += ParentProperties.Density;
		Properties.DragCoeff += ParentProperties.DragCoeff;
		Properties.Gravity += ParentProperties.Gravity;
	}
	return Properties;
}
