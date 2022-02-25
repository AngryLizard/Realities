// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Components/Dimension/TGOR_InfluencableComponent.h"
#include "TGOR_ItemDropSimulationComponent.generated.h"

class ATGOR_ItemDropActor;
class UTGOR_MetamorphComponent;

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_ItemDropSimulationComponent : public UTGOR_InfluencableComponent
{
	GENERATED_BODY()
	
public:
	UTGOR_ItemDropSimulationComponent();
	
	virtual void BeginPlay() override;
	virtual void RegionTick(float DeltaTime) override;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual void DirectInfluenced(const FTGOR_ElementInstance& State, AActor* Instigator) override;

	/** Get item and ensure it is synced with itemdrop */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Simulation", Meta = (Keywords = "C++"))
		UTGOR_Item* EnsureItem();

	/** Get progress per element for nearest metamorph (Values betwen -1.0 and 1.0) */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Simulation", Meta = (Keywords = "C++"))
		float GetMetamorphProgress(FTGOR_ElementInstance& ElementInstance);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Parent itemdrop */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR|Simulation")
		ATGOR_ItemDropActor* ItemDrop;

	/** Parent' MetamorphComponent if exists */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TGOR|Simulation")
		UTGOR_MetamorphComponent* MetamorphComponent;

private:
	UTGOR_Item* _item;

};
