// TGOR (C) // CHECKED //
#pragma once

#include "Realities/Base/Instances/Dimension/TGOR_MainRegionInstance.h"
#include "Realities/Base/Instances/Dimension/TGOR_RegionInstance.h"
#include "Realities/Base/Instances/Creature/TGOR_MovementInstance.h"
#include "Realities/Actors/Voronoi/VoronoiDiagram.h"

#include "Realities/Utility/Error/TGOR_Error.h"

#include "Realities/Actors/Dimension/TGOR_RegionActor.h"
#include "TGOR_MainRegionActor.generated.h"



/**
* TGOR_MainRegionActor is a dimension holding multiple regions
* TGOR_RegionComponents can orient themselves inside.
*/

UCLASS()
class REALITIES_API ATGOR_MainRegionActor : public ATGOR_RegionActor
{
	GENERATED_BODY()

		friend class UTGOR_DimensionData;
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_MainRegionActor();
	
	virtual void BeginPlay() override;

	virtual bool PreAssemble(UTGOR_DimensionData* Dimension) override;
	virtual bool PostAssemble(UTGOR_DimensionData* Dimension) override;


	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////


	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	/** */
	

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Add region to this controller */
	UFUNCTION(Category = "TGOR Region", Meta = (Keywords = "C++"))
		void AddRegion(ATGOR_RegionActor* Region);

	/** Get region from this controller */
	UFUNCTION(BlueprintCallable, Category = "TGOR Region", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		ATGOR_RegionActor* GetRegion(int32 Index, ETGOR_FetchEnumeration& Branches);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Counts number of active region components in this dimension */
	UFUNCTION(BlueprintCallable, Category = "TGOR Region", Meta = (Keywords = "C++"))
		int32 CountActive();

	/** Returns true if active */
	UFUNCTION(BlueprintCallable, Category = "TGOR Region", Meta = (Keywords = "C++"))
		bool IsActive();
		
	/** Prints the voronoi diagram to 3D space at a certain height */
	UFUNCTION(BlueprintCallable, Category = "TGOR Region", Meta = (Keywords = "C++"))
		void DebugDraw(float Z);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Create corner regions */
	UFUNCTION(Category = "TGOR Region", Meta = (Keywords = "C++"))
		void Construct();

	/** Triangulate */
	UFUNCTION(Category = "TGOR Region", Meta = (Keywords = "C++"))
		void Triangulate();

	/** Holds state whether this controller has already been constructed */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Region", Meta = (Keywords = "C++"))
		bool IsConstructed;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Draws debuglines to display regions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Region", Meta = (Keywords = "C++"))
		bool DrawDebugRegions;

	/** Region storage (stores regions) */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Region", Meta = (Keywords = "C++"))
		FTGOR_MainRegionInstance MainRegionInstance;
	
	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////
public:

	void DebugDraw(Voronoi::Cell* c, float Z);
	void DebugDraw(Voronoi::Edge* e, float Z);
	void DebugDraw(Voronoi::Triangle* t, float Z);
	void DebugDraw(FVector Start, FVector End, FColor Colour, float Size, float Z);

#if WITH_EDITOR
	virtual void DebugIndependentDraw() override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditMove(bool bFinished) override;
#endif

protected:
private:
};
