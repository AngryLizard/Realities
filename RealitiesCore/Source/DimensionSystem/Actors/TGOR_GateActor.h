// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "DimensionSystem/Actors/TGOR_DimensionActor.h"
#include "TGOR_GateActor.generated.h"

///////////////////////////////////////////////// DECL ///////////////////////////////////////////////////

class UTGOR_DimensionGateComponent;



/**
* TGOR_GateActor
*/
UCLASS()
class DIMENSIONSYSTEM_API ATGOR_GateActor : public ATGOR_DimensionActor
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_GateActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual bool Assemble(UTGOR_DimensionData* Dimension) override;

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** Called after this gate has teleported actors (called both on master and slave). */
	UFUNCTION(BlueprintImplementableEvent, BlueprintAuthorityOnly, Category = "!TGOR Volume", Meta = (Keywords = "C++"))
		void OnGateExchange();

	/** Get free spawn transform near this gate */
	UFUNCTION(BlueprintImplementableEvent, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void GetFreeSpawnTransform(FVector& Location, FRotator& Rotation) const;

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	/** Dimension loader */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Actor, meta = (AllowPrivateAccess = "true"))
		UTGOR_DimensionGateComponent* GateComponent;

public:

	FORCEINLINE UTGOR_DimensionGateComponent* GetGateComponent() const { return GateComponent; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UFUNCTION()
		void RepNotifyLifeline();

	/** Mobility component in this gate. */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Dimension")
		TArray<UTGOR_PilotComponent*> Content;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Starts/Stops teleport depending on current state */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void CheckContentTeleport();

	/** Adds mobility to gate content */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void AddContent(UTGOR_PilotComponent* Component);

	/** Removes mobility from gate content */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void RemoveContent(UTGOR_PilotComponent* Component);

	/** Check whether there are any actors in teleport area. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		bool HasContent() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Exchange all actors in this gate with its link. Gets only executed on master node. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void ExchangeContent();

	/** Check whether there are any actors in this or link teleport area. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		bool LinksHaveContent() const;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

private:
	
};
