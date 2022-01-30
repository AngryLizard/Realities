// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"

#include "../TGOR_PilotInstance.h"

#include "DimensionSystem/Interfaces/TGOR_SpawnerInterface.h"
#include "TGOR_ColliderComponent.h"
#include "TGOR_PilotComponent.generated.h"

class UTGOR_Primitive;
class UTGOR_PilotTask;
class UTGOR_Pilot;

/**
*
*/
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_PrimitiveSlot
{
	GENERATED_USTRUCT_BODY();

	/** Pilot task primitive is attached to */
	UPROPERTY()
		int32 Index = -1;

	/** Primitive radius */
	UPROPERTY()
		float Radius = 0.0f;

	/** Primitive local offset */
	UPROPERTY()
		FVector Offset = FVector::ZeroVector;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPilotUpdateDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FBaseChangeDelegate, const FTGOR_MovementDynamic&, From, const FTGOR_MovementDynamic&, To, ATGOR_PhysicsVolume*, Volume);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOutOfLevelDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChangedVolumeDelegate, ATGOR_PhysicsVolume*, Previous);

/**
 * TGOR_PilotComponent Moves owner's root component. Actors should only have one of these.
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class DIMENSIONSYSTEM_API UTGOR_PilotComponent : public UTGOR_ColliderComponent, public ITGOR_SpawnerInterface
{
	GENERATED_BODY()
	
public:
	UTGOR_PilotComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void DestroyComponent(bool bPromoteChildren) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& Objs) override;
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	virtual TSet<UTGOR_CoreContent*> GetActiveContent_Implementation() const override;

	virtual const FTGOR_MovementSpace ComputeSpace() const override;
	virtual const FTGOR_MovementPosition ComputePosition() const override;
	virtual bool HasParent(const UTGOR_MobilityComponent* Component) const override;
	virtual void OnPositionChange(const FTGOR_MovementPosition& Position) override;

	virtual void UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies) override;
	virtual TMap<int32, UTGOR_SpawnModule*> GetModuleType_Implementation() const override;

	virtual bool PreAssemble(UTGOR_DimensionData* Dimension) override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Movement")
		FPilotUpdateDelegate OnPilotChanged;

	/** Called when we left the level on surroundings update */
	UPROPERTY(BlueprintAssignable, Category = "!TGOR Movement")
		FOutOfLevelDelegate OnOutOfLevel;

	/** Called when we left a volume (not called when entering a subvolume) */
	UPROPERTY(BlueprintAssignable, Category = "!TGOR Movement")
		FChangedVolumeDelegate OnVolumeChanged;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Movement")
		float SpawnWeight = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Movement")
		FVector SpawnSurfaceArea = FVector(0.5f, 0.5f, 0.5f);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Primitive this pilot represents */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		TSubclassOf<UTGOR_Primitive> SpawnPrimitive;

	/** Pilots in this Pilot */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		TArray<TSubclassOf<UTGOR_Pilot>> SpawnPilots;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Delta for movement corrections so visuals can update smoothly */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Movement")
		FTGOR_MovementPosition NetSmoothDelta;

	/** Sets NetSmoothDelta to a given delta transform (mostly used for testing) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		void SetNetSmooth(FVector Delta, FRotator Rotation);

	/** NetSmoothDelta is the visual movement offset introduced by replication to be used for smooth visual updates */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void NetSmooth(float Timestep, float Speed);

	/** Transforms a component to the current smoothdelta depending on the initial transform (transform when there is no delta) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void SetComponentFromSmoothDelta(USceneComponent* Component, const FTransform& InitialWorld);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Movement")
		FBaseChangeDelegate OnTeleportedMovement;

	/** Teleports this component to a position (Slow and reparents to any matching volume. Reparent properly if possible.) */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Movement", Meta = (Keywords = "C++", DeprecatedFunction, DeprecationMessage = "There is no reason to do this manually, use proper reparenting instead."))
		virtual bool Teleport(const FTGOR_MovementDynamic& Dynamic);

	/** Look for a volume thet encompasses given location */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		ATGOR_PhysicsVolume* QueryVolume(const FVector& Location, ATGOR_PhysicsVolume* ReferenceVolume = nullptr) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Compute current movement base */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		FTGOR_MovementDynamic ComputeBase() const;

	/** Get attached pilot task */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		UTGOR_PilotTask* GetPilotTask() const;

	/** Get parent component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		UTGOR_MobilityComponent* GetParent() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Unparent to euclidean at current location to the same parent. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool Detach();

	/** Check whether a given pilot is attached */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool IsAttachedWith(int32 Identifier) const;

	/** Set which pilot we're attaching to */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void AttachWith(int32 Identifier);

	/** Get attached pilot task */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		UTGOR_PilotTask* GetCurrentPilotOfType(TSubclassOf<UTGOR_PilotTask> Type) const;

	template<typename T> T* GetCurrentPOfType() const
	{
		return Cast<T>(GetCurrentPilotOfType(T::StaticClass()));
	}

	/** Get all pilot tasks of given type */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		TArray<UTGOR_PilotTask*> GetPilotListOfType(TSubclassOf<UTGOR_PilotTask> Type) const;

	template<typename T> TArray<T*> GetPListOfType() const
	{
		TArray<T*> Output;
		TArray<UTGOR_PilotTask*> Pilots = GetPilotListOfType(T::StaticClass());
		for (UTGOR_PilotTask* Pilot : Pilots)
		{
			Output.Emplace(Cast<T>(Pilot));
		}
		return Output;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Currently assigned containers. */
	UPROPERTY(Replicated)
		TArray<UTGOR_PilotTask*> PilotSlots;

	/** Currently scheduled pilot */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = RepNotifyPilotState)
		FTGOR_PilotState PilotState;

	/** Fast forward pilot on replication */
	UFUNCTION()
		void RepNotifyPilotState(const FTGOR_PilotState& Old);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Current physics volume */
	UPROPERTY(BlueprintReadOnly)
		TWeakObjectPtr<ATGOR_PhysicsVolume> SurroundingVolume;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Threshold when comparing current state on master to a received update from slave, decides whether to send an adjust to the slave. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Internal")
		FTGOR_MovementThreshold AdjustThreshold;

	/** Finds an actor's pilot if available */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		static UTGOR_PilotComponent* FindOwningPilot(USceneComponent* Component);

	/** Finds an actor's pilot if available */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		static UTGOR_PilotComponent* FindRootPilot(AActor* Actor);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Tries to find reparent to a given actor (defaults to current volume if null or invalid) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		FTGOR_MovementParent FindReparentToActor(AActor* Actor, const FName& Name) const;

	/** Tries to find reparent to a given component (defaults to current volume if null or invalid) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		FTGOR_MovementParent FindReparentToComponent(UActorComponent* Component, const FName& Name) const;
};
