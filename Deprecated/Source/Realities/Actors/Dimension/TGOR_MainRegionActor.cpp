// The Gateway of Realities: Planes of Existence.


#include "TGOR_MainRegionActor.h"
#include "Realities/Base/TGOR_GameInstance.h"

#include "Engine/World.h"
#include "Engine/Level.h"
#include "Engine/LevelBounds.h"

#include "Realities/Base/System/Data/TGOR_DimensionData.h"
#include "Realities/Actors/Dimension/TGOR_RegionActor.h"
#include "Realities/Mod/Dimensions/TGOR_Dimension.h"
#include "Realities/Base/TGOR_WorldSettings.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"



ATGOR_MainRegionActor::ATGOR_MainRegionActor()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;

	IsConstructed = false;
	DrawDebugRegions = false;
}

void ATGOR_MainRegionActor::BeginPlay()
{
	Super::BeginPlay();

}

bool ATGOR_MainRegionActor::PreAssemble(UTGOR_DimensionData* Dimension)
{
	Super::PreAssemble(Dimension);

	// Register myself to dimension
	Dimension->MainRegion = this;

	// Make sure this is the first region
	AddRegion(this);

	return(true);
}

bool ATGOR_MainRegionActor::PostAssemble(UTGOR_DimensionData* Dimension)
{
	Super::PostAssemble(Dimension);

	// Construct voronoi diagram if not happened yet
	Construct();

	return(true);
}

void ATGOR_MainRegionActor::AddRegion(ATGOR_RegionActor* Region)
{
	// Don't add the same region twice
	if (!MainRegionInstance.Regions.Contains(Region))
	{
		// Add region to the end of the list
		MainRegionInstance.Regions.Add(Region);
	}
}

ATGOR_RegionActor* ATGOR_MainRegionActor::GetRegion(int32 Index, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;

	if (Index < 0) return(nullptr);
	if (Index >= MainRegionInstance.Regions.Num()) return(nullptr);

	Branches = ETGOR_FetchEnumeration::Found;
	return(MainRegionInstance.Regions[Index]);
}


////////////////////////////////////////////////////////////////////////////////////////////////////


void ATGOR_MainRegionActor::DebugDraw(float Z)
{
	FlushPersistentDebugLines(GetWorld());

	for (ATGOR_RegionActor* Region : MainRegionInstance.Regions)
	{
		DebugDraw(&Region->GetCell(), Z);
	}
}

void ATGOR_MainRegionActor::Construct()
{
	// Make sure this is constructed only once
	if (IsConstructed) return;

	// Triangulate diagram
	Triangulate();
	
	// Assign all regions their neighbours
	for (ATGOR_RegionActor* Region : MainRegionInstance.Regions)
	{
		Region->Neighbours.Reset();
		Region->GetCell().forEachNeighbour([this, Region](Voronoi::Cell* Cell) -> bool
		{
			ETGOR_FetchEnumeration State;
			ATGOR_RegionActor* Neighbour = GetRegion(Cell->getID(), State);
			if (IsValid(Neighbour))
			{
				Region->Neighbours.Add(Neighbour);
			}
			return(true);
		});
	}

	// Draw debug lines if active
	if (DrawDebugRegions)
	{
		DebugDraw(Ceiling);
	}

	IsConstructed = true;
}


void ATGOR_MainRegionActor::Triangulate()
{
	// Create array of adequate size
	int32 Size = MainRegionInstance.Regions.Num();
	Voronoi::Array<Voronoi::Cell*> Cells = Voronoi::Array<Voronoi::Cell*>(Size);
	
	// Find all regions
	for (int i = 0; i < Size; i++)
	{
		// Construct region cell
		RegionCell& Cell = MainRegionInstance.Regions[i]->Construct(i);
		Cells.push(&Cell);
	}

	// Create voronoi diagram
	Voronoi::VoronoiDiagram* voronoi = new Voronoi::VoronoiDiagram(&Cells);

	// Create and triangulate
	voronoi->create();
	voronoi->triangulate();

	// Deallocate
	delete(voronoi);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int32 ATGOR_MainRegionActor::CountActive()
{
	int32 Counter = 0;
	
	for (ATGOR_RegionActor* Region : MainRegionInstance.Regions)
	{
		if (Region->HasActiveComponents())
		{
			Counter++;
		}
	}
	return(Counter);
}

bool ATGOR_MainRegionActor::IsActive()
{
	return(CountActive() > 0);
}



void ATGOR_MainRegionActor::DebugDraw(Voronoi::Cell* c, float Z)
{
	// Draw every edge
	c->center.forEachEdge([this, Z](Voronoi::Edge* e)->bool
	{
		DebugDraw(e, Z);
		DebugDraw(e->back, Z);
		return(true);
	});

	// Draw center
	FVector From = FVector(c->center.p.x, c->center.p.y, 0.0f);
	FVector To = FVector(c->center.p.x, c->center.p.y, Z);
	DrawDebugLine(GetWorld(), From, To, FColor(0x2000FF00), true, -1.0f, 255, 64.0f);

	c->center.forEachJoint([this, Z](Voronoi::Triangle* t) -> bool
	{
		DebugDraw(t, Z);
		return(true);
	});
}

void ATGOR_MainRegionActor::DebugDraw(Voronoi::Edge* e, float Z)
{
	// Check if edge is valid
	if (e == nullptr)
	{
		return;
	}

	// Get start position
	Voronoi::Vector Start = e->getStart()->p;
	FVector From = FVector(Start.x, Start.y, Z);

	// Get end position
	Voronoi::EdgePoint* Point = e->getEnd();
	Voronoi::Vector End = e->getRay(32'768.0f);

	// Get end point if not infinite
	FVector To = FVector(End.x, End.y, Z);
	if (Point != nullptr)
	{
		To = FVector(Point->p.x, Point->p.y, Z);
	}
	
	DebugDraw(From, To, FColor(0x20FF0000), 64.0f, Z);
}

void ATGOR_MainRegionActor::DebugDraw(Voronoi::Triangle* t, float Z)
{
	// Check if edge is valid
	if (t == nullptr)
	{
		return;
	}
	
	t->forEachEdge([this, Z](Voronoi::Vector From, Voronoi::Vector To)
	{
		FVector Start = FVector(From.x, From.y, 0.0f);
		FVector End = FVector(To.x, To.y, 0.0f);
		DebugDraw(Start, End, FColor(0x2000FF00), 8.0f, Z / 3);
	});
}

void ATGOR_MainRegionActor::DebugDraw(FVector Start, FVector End, FColor Colour, float Size, float Z)
{
	TArray<FVector> Vertices;
	Vertices.Add(FVector(Start.X, Start.Y, -Z));
	Vertices.Add(FVector(Start.X, Start.Y, Z));
	Vertices.Add(FVector(End.X, End.Y, -Z));
	Vertices.Add(FVector(End.X, End.Y, Z));
	TArray<int32> Indices;
	Indices.Add(0);
	Indices.Add(1);
	Indices.Add(3);
	Indices.Add(0);
	Indices.Add(3);
	Indices.Add(2);
	DrawDebugMesh(GetWorld(), Vertices, Indices, Colour, true, -1.0f, 255);

	DrawDebugLine(GetWorld(), Vertices[1], Vertices[3], Colour, true, -1.0f, 255, Size);
}


#if WITH_EDITOR

void ATGOR_MainRegionActor::DebugIndependentDraw()
{
	FlushPersistentDebugLines(GetWorld());

	if (!DrawDebugRegions) return;
	if (!IsValid(GetWorld())) return;

	Voronoi::Array<Voronoi::Cell*> Temp = Voronoi::Array<Voronoi::Cell*>();
	
	// Create corners
	int Index = 0;

	// Find all regions
	for (TActorIterator<AActor> a(GetWorld()); a; ++a)
	{
		if (a->IsA(ATGOR_RegionActor::StaticClass()))
		{
			ATGOR_RegionActor* Region = Cast<ATGOR_RegionActor>(*a);
			FVector Location = Region->GetActorLocation();
			Voronoi::Vector Vector = Voronoi::Vector(floor(Location.X), floor(Location.Y));
			Voronoi::Cell* Cell = new Voronoi::Cell(Vector, Index++);
			Temp.push(Cell);
		}
	}

	// Create voronoi diagram
	Voronoi::VoronoiDiagram* voronoi = new Voronoi::VoronoiDiagram(&Temp);

	// Create and triangulate
	voronoi->create();
	voronoi->triangulate();

	// Deallocate
	delete(voronoi);

	// Draw all edges
	FVector Origin = GetActorLocation();
	FVector Scale = GetActorScale();
	const float Z = Origin.Z + 1000.0f * Scale.Z;
	for (int i = 0; i < Temp.getLength(); i++)
	{
		DebugDraw(Temp[i], Z);
	}

	// Deallocate
	for (int i = 0; i < Temp.getLength(); i++)
	{
		delete(Temp[i]);
	}
}

void ATGOR_MainRegionActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	DebugIndependentDraw();
}

void ATGOR_MainRegionActor::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);

	DebugIndependentDraw();
}

#endif