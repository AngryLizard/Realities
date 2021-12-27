// TGOR (C) // CHECKED //
#pragma once

#include "DimensionSystem/TGOR_PilotInstance.h"

#include "../Interfaces/TGOR_DimensionInterface.h"
#include "CoreSystem/Actors/TGOR_Actor.h"
#include "TGOR_PhysicsVolume.generated.h"

class UTGOR_PilotComponent;
class UTGOR_IdentityComponent;
class UTGOR_VolumeVisualiserComponent;
class UTGOR_ReplicationGraphNode_VolumeCell;

///////////////////////////////////////////////// DECL ///////////////////////////////////////////////////

UENUM(BlueprintType)
enum class ETGOR_BoundaryTest : uint8
{
	Disjunct,
	Engulfing,
	Overlapping,
	Inside
};

/**
* 
*/

UCLASS()
class DIMENSIONSYSTEM_API ATGOR_PhysicsVolume : public ATGOR_Actor, public ITGOR_DimensionInterface
{
	GENERATED_BODY()

		friend class UTGOR_ReplicationGraphNode_VolumeSpatialization;
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_PhysicsVolume(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;


	virtual bool PreAssemble(UTGOR_DimensionData* Dimension) override;

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** Point test */
	UFUNCTION(BlueprintPure, Category = "!TGOR Volume", Meta = (Keywords = "C++"))
		virtual bool IsInside(const FVector& Location) const;

	/** Checks a boundary against this volume */
	UFUNCTION(BlueprintPure, Category = "!TGOR Volume|Internal", Meta = (Keywords = "C++"))
		virtual ETGOR_BoundaryTest TestBoundary(const FBox& Box) const;

	/** Volume to order volumes by */
	UFUNCTION(BlueprintPure, Category = "!TGOR Volume|Internal", Meta = (Keywords = "C++"))
		virtual float ComputeVolume() const;

	/** Bounding box */
	UFUNCTION(BlueprintPure, Category = "!TGOR Volume|Internal", Meta = (Keywords = "C++"))
		virtual FBox ComputeBoundingBox() const;

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UTGOR_PilotComponent* MobilityComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UTGOR_IdentityComponent* IdentityComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UTGOR_VolumeVisualiserComponent* Visualization;

public:

	FORCEINLINE UTGOR_PilotComponent* GetMovement() const { return MobilityComponent; }
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get Volume inside this volume depending on best overlap */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Volume", Meta = (Keywords = "C++"))
		ATGOR_PhysicsVolume* Query(const FVector& Location);

	/** Get network volumes inside this volume depending on best overlap */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Volume", Meta = (Keywords = "C++"))
		ATGOR_PhysicsVolume* QueryNetwork(const FVector& Location);

	/** Insert a physics volume into this tree */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Volume|Internal", Meta = (Keywords = "C++"))
		void Insert(ATGOR_PhysicsVolume* Other);

	/** Remove a physics volume from this tree */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Volume|Internal", Meta = (Keywords = "C++"))
		void Remove(ATGOR_PhysicsVolume* Other);

	/** Print tree in text format */
	UFUNCTION(BlueprintPure, Category = "!TGOR Volume", Meta = (Keywords = "C++"))
		virtual FString Print(const FString& Prefix) const;

	/** Retreive parent volume if available */
	UFUNCTION(BlueprintPure, Category = "!TGOR Volume", Meta = (Keywords = "C++"))
		ATGOR_PhysicsVolume* GetParentVolume() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Parent volume */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Volume")
		TWeakObjectPtr<ATGOR_PhysicsVolume> Parent;

	/** Child volumes */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Volume")
		TArray<ATGOR_PhysicsVolume*> Volumes;

	/** Whether parent surroundings are added on top */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Volume")
		bool IsAdditive;


	/** Volume gravity direction in local space */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "!TGOR Volume")
		FVector_NetQuantizeNormal UpVector;

	/** Volume gravity */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "!TGOR Volume")
		float Gravity;


	/** Material density */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Volume")
		float Density;

	/** Material drag */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Volume")
		float DragCoeff;

	/** Whether this volume is used for network culling */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Volume")
		bool IsNetworkVolume;

	/** Network cell associated with this volume */
	UPROPERTY()
		UTGOR_ReplicationGraphNode_VolumeCell* NetworkCell;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Compute surroundings */
	UFUNCTION(BlueprintPure, Category = "!TGOR Volume", Meta = (Keywords = "C++"))
		virtual FTGOR_PhysicsProperties ComputeSurroundings(const FVector& Location) const;

	/** Adds parent surroundings to own if additive */
	UFUNCTION(BlueprintPure, Category = "!TGOR Volume", Meta = (Keywords = "C++"))
		virtual FTGOR_PhysicsProperties ComputeAllSurroundings(const FVector& Location) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	void UpdateVisuals();
#endif // WITH_EDITOR
	//~ End UObject Interface
	// 
	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:
	
};
