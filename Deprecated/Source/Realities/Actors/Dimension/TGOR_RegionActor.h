// TGOR (C) // CHECKED //
#pragma once

#include "Realities/Base/Instances/Dimension/TGOR_RegionInstance.h"
#include "Realities/Base/Instances/Dimension/TGOR_ElementInstance.h"

#include "Realities/Utility/Datastructures/TGOR_ContentTable.h"
#include "Realities/Actors/Voronoi/VoronoiDiagram.h"

#include "Realities/Actors/TGOR_DimensionActor.h"
#include "TGOR_RegionActor.generated.h"

///////////////////////////////////////////////// DECL ///////////////////////////////////////////////////

class UTGOR_RegionComponent;

////////////////////////////////////////////// EXTERNAL //////////////////////////////////////////////////////

class RegionCell : public Voronoi::Cell
{
public:
	RegionCell();
	
	void set(FVector v, int id);

private:
};

/**
* TGOR_RegionActor defines a region within a dimension that holds RegionComponents and
* allows them to communicate with each other and their surroundings.
*/
UCLASS()
class REALITIES_API ATGOR_RegionActor : public ATGOR_DimensionActor
{
	GENERATED_BODY()

	friend class ATGOR_MainRegionActor;
	friend struct FTGOR_MainRegionInstance;
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_RegionActor();
	
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	virtual bool Assemble(UTGOR_DimensionData* Dimension) override;

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** Computes region location and height */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Region", Meta = (Keywords = "C++"))
		float ComputeLocationAndHeight(FVector& Location);

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	/** */

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Finds index of component inside region */
	UFUNCTION()
		int32 FindComponent(UTGOR_RegionComponent* Component);

	/** Adds Component to this regions register */
	UFUNCTION()
		void RegisterComponent(UTGOR_RegionComponent* Component);

	/** Removes Component from this regions register */
	UFUNCTION()
		void UnregisterComponent(UTGOR_RegionComponent* Component);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Check whether there are any active components */
	UFUNCTION(BlueprintPure, Category = "TGOR Region", Meta = (Keywords = "C++"))
		bool HasActiveComponents() const;


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Name of this region */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Region", Meta = (Keywords = "C++"))
		FText Name;

	/** Top state */
	UPROPERTY(EditAnywhere, Category = "TGOR Region", Meta = (Keywords = "C++"))
		FTGOR_ElementInstance Top;

	/** Bottom state */
	UPROPERTY(EditAnywhere, Category = "TGOR Region", Meta = (Keywords = "C++"))
		FTGOR_ElementInstance Bottom;

	/** Top most Z value of this region */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Region", Meta = (Keywords = "C++"))
		float Ceiling;

	/** Bottom most Z value of this region */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Region", Meta = (Keywords = "C++"))
		float Floor;

	/** Neighbouring regions */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Region", Meta = (Keywords = "C++"))
		TArray<ATGOR_RegionActor*> Neighbours;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get top element */
	UFUNCTION(BlueprintPure, Category = "TGOR Region", Meta = (Keywords = "C++"))
		const FTGOR_ElementInstance& GetTop() const;

	/** Get bottom element */
	UFUNCTION(BlueprintPure, Category = "TGOR Region", Meta = (Keywords = "C++"))
		const FTGOR_ElementInstance& GetBottom() const;

	/** Get ceiling height */
	UFUNCTION(BlueprintPure, Category = "TGOR Region", Meta = (Keywords = "C++"))
		float GetCeiling() const;

	/** Get floot height */
	UFUNCTION(BlueprintPure, Category = "TGOR Region", Meta = (Keywords = "C++"))
		float GetFloor() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Number of active regionComponents in this region */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Region", Meta = (Keywords = "C++"))
		int32 ActiveComponents;

	/** Region updates per second */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Region", meta = (ClampMin = "0.0", ClampMax = "30.0"))
		float RegionUpdateRate;

	/** This region's data storage */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Region", Meta = (Keywords = "C++"))
		FTGOR_RegionInstance RegionInstance;



	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////
public:

	template<typename T>
	void CallInside(std::function<void(T*, float)> func, AActor* Ignore, const FVector& Location, float Radius)
	{
		float Squared = Radius * Radius;
		FVector2D Plane = FVector2D(Location);
		for (int i = 0; i < RegionInstance.Register.Num(); i++)
		{
			// Check if actor is in range
			FTGOR_ActorState& Instance = RegionInstance.Register[i];
			float Dist = FVector2D::DistSquared(Instance.Cache, Plane);
			if (Dist >= Squared) { continue; }

			// Get target location
			T* Component = Cast<T>(Instance.Component);
			if (!IsValid(Component)) continue;

			// Ignore specified
			AActor* Actor = Component->GetOwner();
			if (Actor == Ignore) continue;

			// Get distance ratio
			float Ratio = Dist / Squared;

			// call function if in range
			if (Ratio <= 1.0f)
			{
				func(Component, __min(1.0f - Ratio, 1.0f));
			}
		}
	}

	RegionCell& Construct(int ID);
	RegionCell& GetCell();

#if WITH_EDITOR
	virtual void DebugIndependentDraw();

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditMove(bool bFinished) override;
#endif

protected:

private:

	RegionCell _cell;
	float _tickBuffer;
};
