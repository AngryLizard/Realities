// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/Inventory/TGOR_SocketInstance.h"
#include "Realities/Base/Instances/Inventory/TGOR_StorageInstance.h"

#include "Realities/Components/Movement/TGOR_MobilityComponent.h"
#include "Realities/Base/Instances/Combat/TGOR_AimInstance.h"

#include "Realities/Mod/Targets/TGOR_Target.h"
#include "TGOR_Socket.generated.h"

class UTGOR_SocketStorage;
class UTGOR_DimensionComponent;

/**
 * 
 */
UCLASS(Blueprintable)
class REALITIES_API UTGOR_Socket : public UTGOR_Target
{
	GENERATED_BODY()

public:
	UTGOR_Socket();

	virtual bool OverlapTarget(UTGOR_InteractableComponent* Component, const FVector& Origin, float MaxDistance, FTGOR_AimPoint& Point) const override;
	virtual bool SearchTarget(UTGOR_InteractableComponent* Component, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimPoint& Point) const override;
	virtual bool ComputeTarget(const FTGOR_AimPoint& Point, const FVector& Origin, const FVector& Direction, float MaxDistance, FTGOR_AimInstance& Instance) const override;
	virtual FVector QueryAimLocation(const FTGOR_AimInstance& Instance) const override;
	virtual FVector QueryStickyLocation(const FTGOR_AimInstance& Instance) const override;


	/** Distance threshold on camera plane for a valid target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Target", Meta = (Keywords = "C++"))
		float DistanceThreshold;

	/** Offset applied for target calculation (so one can target stuff attached to the socket and vice versa) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Target", Meta = (Keywords = "C++"))
		FVector TargetOffset;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get spatial information about this socket */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual UTGOR_MobilityComponent* QuerySocket(UTGOR_InteractableComponent* Component, FTGOR_MovementPosition& Position) const;

	/** Get spatial information about this socket for a given target */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual UTGOR_MobilityComponent* QuerySocketWith(USceneComponent* Component, FTGOR_MovementPosition& Position) const;

	/** Attach component to a parent movement */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual bool AttachToSocket(UTGOR_MobilityComponent* Parent, UTGOR_PilotComponent* Component);

	/** Whether we can attach component to a parent movement */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual bool CanAttachToSocket(UTGOR_MobilityComponent* Parent, UTGOR_PilotComponent* Component) const;

	/** Whether we can fit a body to a parent movement */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		virtual bool CanFitToSocket(UTGOR_MobilityComponent* Parent, const FTGOR_MovementShape& Shape) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/**
	* Create an empty storage instance for this socket.
	* @see TGOR_Storage
	*/
	UFUNCTION(BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		virtual UTGOR_SocketStorage* CreateStorage();

	/** Build Storage modules and set their default values */
	UFUNCTION(BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		virtual void BuildStorage(UTGOR_SocketStorage* Storage);

	/** Change values and module values of a created storage object */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		void InitialiseStorage(UTGOR_SocketStorage* Storage);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Tick this science with given storage and owner, return action to be performed by container after tick. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Inventory", Meta = (Keywords = "C++"))
		virtual void Tick(UTGOR_DimensionComponent* Owner, UTGOR_SocketStorage* Storage, float DeltaSeconds);

	/** Attaches given component to this socket. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		virtual void Attach(UTGOR_SocketStorage* Storage, USceneComponent* Attachee);

	/** Whether this storage is parented to the given movement. */
	UFUNCTION(BlueprintPure, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		virtual UTGOR_MobilityComponent* GetParent(UTGOR_SocketStorage* Storage) const;

	/** Get current transform of this socket. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		virtual FTGOR_MovementDynamic GetTransform(UTGOR_MobilityComponent* Owner, UTGOR_SocketStorage* Storage) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Modules that are part of this science storage */
	UPROPERTY(EditAnywhere, Category = "TGOR Inventory")
		FTGOR_StorageDeclaration Modules;
};