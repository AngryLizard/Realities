// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"

#include "DimensionSystem/TGOR_MobilityInstance.h"

#include "CoreSystem/Components/TGOR_CapsuleComponent.h"
#include "DimensionSystem/Interfaces/TGOR_DimensionInterface.h"
#include "CoreSystem/Interfaces/TGOR_RegisterInterface.h"
#include "DimensionSystem/Interfaces/TGOR_BufferInterface.h"
#include "TGOR_MobilityComponent.generated.h"

class UTGOR_MobilityComponent;
class UTGOR_LinearPilotTask;

/**
* TGOR_MobilityComponent handles movement states
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class DIMENSIONSYSTEM_API UTGOR_MobilityComponent : public UTGOR_CapsuleComponent, public ITGOR_DimensionInterface, public ITGOR_RegisterInterface
{
	GENERATED_BODY()

	friend struct FTGOR_MovementRecord;
	friend class UTGOR_LinearPilotTask;

public:
	UTGOR_MobilityComponent();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Current movement body */
	UPROPERTY(ReplicatedUsing = RepNotifyBody)
		FTGOR_MovementBody Body;

	UFUNCTION()
		virtual void RepNotifyBody(const FTGOR_MovementBody& Old);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Sets body structure */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void SetBody(const FTGOR_MovementBody& NewBody);

	/** Get current shape of this movement component */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		const FTGOR_MovementBody& GetBody() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Attach a component to this movement */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		virtual void AttachToMovement(USceneComponent* Component, int32 Index);

	/** Detach a component from any movement */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		virtual void DetachFromMovement(USceneComponent* Component);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Attachments to this component */
	UPROPERTY()
		TSet<TWeakObjectPtr<UTGOR_LinearPilotTask>> LinearAttachments;

	/** Add a child to attachment list */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void RegisterAttach(UTGOR_LinearPilotTask* Attachee);

	/** Remove a child from attachment list */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void RegisterDetach(UTGOR_LinearPilotTask* Attachee);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	/** Get scale applied to all thresholds (already part of body) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		const float GetScale() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Inflicts an impulse to a given location and part index (does nothing by default) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		virtual void InflictPointImpact(const FVector& Point, const FVector& Impulse);

	/** Inflicts a force to a given location (does nothing by default) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		virtual void InflictPointForce(const FVector& Point, const FVector& Force, float DeltaTime);

	/** Computes inertial torque factor along a normal (used by collision response) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		virtual float ComputeInertial(const FVector& Point, const FVector& Normal) const;

	/** Whether this movement has authority */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		virtual bool UpdatesBase() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Gets index for a given part name (e.g. bone) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		virtual int32 GetIndexFromName(const FName& Name) const;

	/** Gets name for a given part index (e.g. bone) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		virtual FName GetNameFromIndex(int32 Index) const;

	/** Transform according to an indexed component (e.g. bone) in local space */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		virtual FTGOR_MovementDynamic GetIndexTransform(int32 Index) const;

	/** Whether given index is supported by this mobility */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		virtual bool IsValidIndex(int32 Index) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get this or associated component's position */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		FTGOR_MovementPosition GetComponentPosition() const;

	/** Moves this or associated component to a given position */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		void SetComponentPosition(const FTGOR_MovementPosition& Position);

	/** Called when this mobility changed position */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		virtual void OnPositionChange(const FTGOR_MovementPosition& Position);

	/** Called when this mobility changed parent, mainly for component to transform properties that are stored in local space. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		virtual void OnReparent(const FTGOR_MovementDynamic& Previous);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Check whether this mobility has a given component in its parent chain */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		virtual bool HasParent(const UTGOR_MobilityComponent* Component) const;

	/** Check whether whether this component can be attached to given parent (not already attached in chain) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		virtual bool CanParent(const UTGOR_MobilityComponent* Component) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Parents a component to this mobility using the Parent socket at a given world dynamic. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool ParentLinear(UTGOR_PilotComponent* Attachee, int32 Index, const FTGOR_MovementDynamic& Dynamic);

	/** Parents a component to this mobility using the Parent socket. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool CanParentLinear(UTGOR_PilotComponent* Attachee, int32 Index) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Compute current movement space */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		virtual const FTGOR_MovementSpace ComputeSpace() const;

	/** Compute current movement position */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		virtual const FTGOR_MovementPosition ComputePosition() const;


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Convert dynamic into position */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", meta = (DisplayName = "ToPosition (pos)", CompactNodeTitle = "->"))
		static FTGOR_MovementPosition Conv_DynamicToPosition(const FTGOR_MovementDynamic& Dynamic);

	/** Convert space into dynamic */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", meta = (DisplayName = "ToDynamic (dynamic)", CompactNodeTitle = "->"))
		static FTGOR_MovementDynamic Conv_SpaceToDynamic(const FTGOR_MovementSpace& Space);

	/** Create body from box */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", meta = (DisplayName = "FromBox (box)"))
		static FTGOR_MovementBody Conv_BodyFromBox(UBoxComponent* Box, const FVector& SurfaceSparsity, float Density);

	/** Create body from sphere */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", meta = (DisplayName = "FromSphere (sphere)"))
		static FTGOR_MovementBody Conv_BodyFromSphere(USphereComponent* Sphere, const FVector& SurfaceSparsity, float Density);

	/** Create body from capsule */
	UFUNCTION(BlueprintPure, Category = "!TGOR Math", meta = (DisplayName = "FromCapsule (capsule)"))
		static FTGOR_MovementBody Conv_BodyFromCapsule(UCapsuleComponent* Capsule, const FVector& SurfaceSparsity, float Density);
};
