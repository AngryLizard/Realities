// The Gateway of Realities: Planes of Existence.

#pragma once

#include <functional>
#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "../Voronoi/SearchActor.h"

#include "../TGOR_RegionActor.h"
#include "CoreSystem/Interfaces/TGOR_SingletonInterface.h"
#include "../../Interfaces/TGOR_DimensionInterface.h"
#include "Components/SceneComponent.h"
#include "TGOR_RegionComponent.generated.h"

#define UPDATE_EPS 1.0f

class ATGOR_MainRegionActor;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FProximityDelegate, UTGOR_RegionComponent*, RegionComponent, float, Ratio);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRegionDelegate, ATGOR_RegionActor*, Region);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSectionDelegate, ATGOR_RegionActor*, Opposite, ATGOR_RegionActor*, Prev, ATGOR_RegionActor*, Next);

/**
* TGOR_RegionComponent allows navigation inside regions
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DIMENSIONSYSTEM_API UTGOR_RegionComponent : public USceneComponent, public ITGOR_DimensionInterface, public ITGOR_SingletonInterface
{
	GENERATED_BODY()

		friend class ATGOR_RegionActor;

public:	
	// Sets default values for this component's properties
	UTGOR_RegionComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual bool PostAssemble(UTGOR_DimensionData* Dimension) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Gets called when a new region got entered */
	UPROPERTY(BlueprintAssignable, Category = "!TGOR Region")
		FRegionDelegate OnRegionEntered;

	/** Gets called when new a new section entered */
	UPROPERTY(BlueprintAssignable, Category = "!TGOR Region")
		FSectionDelegate OnSectionEntered;

	/** Actively ticked region component 
		( Passively ticked components only get ticked 
		if there is an active component in the region )
		Should only be changed if not in an active region at the moment. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Region", Meta = (Keywords = "C++"))
		bool bActiveRegionComponent;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Current region */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Region", Meta = (Keywords = "C++"))
		ATGOR_RegionActor* Current;

	/** Opposite region */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Region", Meta = (Keywords = "C++"))
		ATGOR_RegionActor* Opposite;

	/** Region to the right */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Region", Meta = (Keywords = "C++"))
		ATGOR_RegionActor* Prev;

	/** Region to the left */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Region", Meta = (Keywords = "C++"))
		ATGOR_RegionActor* Next;

	/** Associated controller */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Region", Meta = (Keywords = "C++"))
		ATGOR_MainRegionActor* MainRegion;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get current region */
	UFUNCTION(BlueprintPure, Category = "!TGOR Region", Meta = (Keywords = "C++"))
		ATGOR_RegionActor* GetCurrentRegion() const;

	/** Get current main region */
	UFUNCTION(BlueprintPure, Category = "!TGOR Region", Meta = (Keywords = "C++"))
		ATGOR_MainRegionActor* GetMainRegion() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Gets called on spawn (true) or level load (false) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Region", Meta = (Keywords = "C++"))
		virtual void RegionStart(bool Spawned);

	/** Gets ticked by the region */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Region", Meta = (Keywords = "C++"))
		virtual void RegionTick(float DeltaTime);

	/** Updates this component's region state */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Region", Meta = (Keywords = "C++"))
		void Update();

	/** Clears this region */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Region", Meta = (Keywords = "C++"))
		void ClearRegion();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Call delegate all actors in proximity */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Region", Meta = (Keywords = "C++"))
		void CallNearby(const FProximityDelegate& Delegate, float Radius, bool IgnoreSelf = true);

		template<typename T>
		void CallNearby(std::function<void(T*, float)> func, float Radius, bool IgnoreSelf = true)
		{
			AActor* Owner = GetOwner();
			FVector Location = Owner->GetActorLocation();
			if (!IgnoreSelf)
			{
				Owner = nullptr;
			}
			
			// Call on myself and all neighbours
			if (IsValid(Current))
			{
				Current->CallInside<T>(func, Owner, Location, Radius);
				Opposite->CallInside<T>(func, Owner, Location, Radius);
				Prev->CallInside<T>(func, Owner, Location, Radius);
				Next->CallInside<T>(func, Owner, Location, Radius);
			}
		}

		template<typename T>
		T* GetMaxNearby(std::function<float(T*, float)> cmp, float Radius, bool IgnoreSelf = true)
		{
			T* t = nullptr;
			float Max = 0.0f;

			CallNearby<T>([&t, &Max, &cmp](T* Component, float Ratio)
			{
				float Value = cmp(Component, Ratio);
				if (Value > Max)
				{
					Max = Value;
					t = Component;
				}
			}, Radius, IgnoreSelf);

			return(t);
		}

protected:
	Voronoi::SearchActor _search;
	int _current;
	int _section;
};
