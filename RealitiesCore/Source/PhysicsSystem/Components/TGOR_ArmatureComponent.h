// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "PhysicsSystem/TGOR_PhysicsInstance.h"

#include "PhysicsSystem/Components/TGOR_RigidComponent.h"
#include "TGOR_ArmatureComponent.generated.h"

class UTGOR_NamedSocket;

/**
 * 
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class PHYSICSSYSTEM_API UTGOR_ArmatureComponent : public UTGOR_RigidComponent
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_ArmatureComponent();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool UpdatesBase() const override;

	virtual int32 GetIndexFromName(const FName& Name) const override;
	virtual FName GetNameFromIndex(int32 Index) const override;
	virtual FTGOR_MovementDynamic GetIndexTransform(int32 Index) const override;
	virtual bool IsValidIndex(int32 Index) const override;

	virtual void AttachToMovement(USceneComponent* Component, int32 Index) override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get skeletal mesh active with this component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		USkeletalMeshComponent* GetSkeletalMesh() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Performs a sphere trace from origin in any direction, parameters given in visual local space */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void VisualTrace(const FVector& Offset, const FVector& Direction, float Distance, float Radius, FTGOR_VisualContact& Contact) const;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	virtual void PreComputePhysics(const FTGOR_MovementTick& Tick) override;
	virtual void ComputePhysics(FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output) override;
	virtual void PostComputePhysics(const FTGOR_MovementSpace& Space, float Energy, float DeltaTime) override;

	virtual float Simulate(float Time) override; // Send input data to server after all simulation ticks are done

	virtual void OutOfLevel() override;
	virtual void Impact(const FTGOR_MovementDynamic& Dynamic, const FVector& Point, const FVector& Impact) override;
	virtual bool CanInflict() const override;
	virtual bool CanRotateOnImpact() const override;

};