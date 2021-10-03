// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "DimensionSystem/TGOR_MobilityInstance.h"
#include "TargetSystem/Content/TGOR_Target.h"

#include "DimensionSystem/Content/TGOR_SpawnModule.h"
#include "TGOR_Socket.generated.h"

class UTGOR_Target;
class UTGOR_Attribute;
class UTGOR_DimensionComponent;

/**
 * 
 */
UCLASS(Blueprintable)
class SOCKETSYSTEM_API UTGOR_Socket : public UTGOR_SpawnModule
{
	GENERATED_BODY()

public:
	UTGOR_Socket();

	/*
	virtual bool OverlapTarget(UTGOR_InteractableComponent* Component, const FVector& Origin, float MaxDistance, FTGOR_AimPoint& Point) const override;
	virtual bool SearchTarget(UTGOR_InteractableComponent* Component, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimPoint& Point) const override;
	virtual bool ComputeTarget(const FTGOR_AimPoint& Point, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimInstance& Instance) const override;
	virtual FVector QueryAimLocation(const FTGOR_AimInstance& Instance) const override;
	virtual FVector QueryStickyLocation(const FTGOR_AimInstance& Instance) const override;
	*/

	/** Distance threshold on camera plane for a valid target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Target", Meta = (Keywords = "C++"))
		float DistanceThreshold;

	/** Offset applied for target calculation (so one can target stuff attached to the socket and vice versa) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Target", Meta = (Keywords = "C++"))
		FVector TargetOffset;

	/** Whether children are locked into place (can reparent themselves if false) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Target", Meta = (Keywords = "C++"))
		bool AllowsReparent;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get spatial information about this socket */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		virtual UTGOR_MobilityComponent* QuerySocket(UTGOR_InteractableComponent* Component, FTGOR_MovementPosition& Position) const;

	/** Get spatial information about this socket for a given target */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		virtual UTGOR_MobilityComponent* QuerySocketWith(UActorComponent* Component, FTGOR_MovementPosition& Position) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Targets this socket aligns to */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Target>> TargetInsertions;
	DECL_INSERTION(TargetInsertions);

	/** Attributes in this spawner */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Attribute>> AttributeInsertions;
	DECL_INSERTION(AttributeInsertions);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;

};