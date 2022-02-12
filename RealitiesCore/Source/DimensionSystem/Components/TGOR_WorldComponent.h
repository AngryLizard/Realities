// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreSystem/Components/TGOR_Component.h"
#include "DimensionSystem/TGOR_DimensionInstance.h"
#include "TGOR_WorldComponent.generated.h"

class UTGOR_DimensionData;

/**
*	TGOR_WorldComponent manages a controller's dimension loading behaviour
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DIMENSIONSYSTEM_API UTGOR_WorldComponent : public UTGOR_Component
{
	GENERATED_BODY()

public:	
	UTGOR_WorldComponent();
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;


	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** This controller's dimension list */
	UPROPERTY(ReplicatedUsing = RepNotifyDimension)
		FTGOR_DimensionRequestList DimensionRequestList;

	/** Whether this controller has a deletion pending */
	UPROPERTY(Replicated)
		bool HasPendingDeletion;

	/** Currently pending deletion with timestamp (server only) */
	UPROPERTY()
		TMap<FName, FTGOR_Time> PendingDeletions;

	UFUNCTION()
		void RepNotifyDimension(const FTGOR_DimensionRequestList& Old);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Adds an already existing dimension to the update list */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void AddExistingDimensionRequest(const FName& Identifier);

	/** Adds a new dimension to the update list */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void AddDimensionRequest(const FName& Identifier, UTGOR_Dimension* Dimension);

	/** Removes a dimension from the update list */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void RemoveDimensionRequest(const FName& Identifier);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Whether a given update is already queued */
	UFUNCTION(BlueprintPure, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		bool HasDimensionRequest(const FName& Identifier) const;

	/** Get dimensions that ought to currently be open from a pawn */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void UpdateDimensionRequestsFromPawn(APawn* ControlledPawn);

	/** Get dimensions that ought to currently be open from all currently active player controllers */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void UpdateDimensionRequestsFromPlayers();

	/** Get dimensions that ought to currently be open for a given actor */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		TSet<FName> GetDimensionRequestsForActor(AActor* Actor);

	/** Get dimensions that ought to currently be open for a given pilot */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		TSet<FName> GetDimensionRequestsForPilot(UTGOR_PilotComponent* Pilot);

	/** Sets the owning dimension */
	//UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension|Internal", Meta = (Keywords = "C++"))
	//	void SetDimensionOwner(const FName& Identifier);

	/** Copy dimension update list from host */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void CopyDimensionRequestListFromHost();

	/** Loads and unloads dimensions */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension|Internal", Meta = (Keywords = "C++"))
		void UpdateDimensionList();

	/** Whether any dimensions need to be loaded or unloaded */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension|Internal", Meta = (Keywords = "C++"))
		bool IsDimensionListUpToDate() const;

	/** Switches dimensions to owner */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension|Internal", Meta = (Keywords = "C++"))
		void UpdateDimensionOwner();

	/** Whether the dimension needs to be switched */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension|Internal", Meta = (Keywords = "C++"))
		bool IsDimensionOwnerUpToDate() const;

	/** Notify server about a dimension having finished unloading */
	UFUNCTION(Reliable, Server, WithValidation)
		void NotifyDimensionUnload(FName Identifier);
	virtual void NotifyDimensionUnload_Implementation(FName Identifier);
	virtual bool NotifyDimensionUnload_Validate(FName Identifier);
};
