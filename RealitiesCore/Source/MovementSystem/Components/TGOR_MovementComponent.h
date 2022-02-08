// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSystem/TGOR_AttributeInstance.h"
#include "MovementSystem/TGOR_MovementInstance.h"
#include "PhysicsSystem/TGOR_PhysicsInstance.h"

#include "AnimationSystem/Interfaces/TGOR_AnimationInterface.h"
#include "DimensionSystem/Interfaces/TGOR_SpawnerInterface.h"
#include "CoreSystem/Interfaces/TGOR_RegisterInterface.h"
#include "AttributeSystem/Interfaces/TGOR_AttributeInterface.h"
#include "PhysicsSystem/Components/TGOR_RigidPawnComponent.h"
#include "TGOR_MovementComponent.generated.h"

class UTGOR_Mobile;
class UTGOR_Movement;
class UTGOR_Animation;
class UTGOR_Performance;
class UTGOR_NamedSocket;

DECLARE_STATS_GROUP(TEXT("TGOR Movement System"), STATGROUP_TGOR_MOV, STATCAT_Advanced);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FKnockoutDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMovementUpdateDelegate);

/**
 * 
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class MOVEMENTSYSTEM_API UTGOR_MovementComponent : public UTGOR_RigidPawnComponent, public ITGOR_AnimationInterface, public ITGOR_SpawnerInterface, public ITGOR_RegisterInterface, public ITGOR_AttributeInterface
{
	GENERATED_BODY()

	friend class UTGOR_Movement;

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_MovementComponent();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual TSet<UTGOR_CoreContent*> GetActiveContent_Implementation() const override;
	
	virtual void UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies) override;
	virtual TMap<int32, UTGOR_SpawnModule*> GetModuleType_Implementation() const override;

	virtual TSubclassOf<UTGOR_Performance> GetPerformanceType() const override;
	virtual UTGOR_AnimationComponent* GetAnimationComponent() const override;

	virtual float GetMaxSpeed() const override;
	virtual void RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed) override;
	virtual void RequestPathMove(const FVector& MoveInput) override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Movement")
		FMovementUpdateDelegate OnMovementChanged;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Movements to this component */
	UPROPERTY()
		TArray<UTGOR_Movement*> Movements;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Movement types this movement spawns with. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		TArray<TSubclassOf<UTGOR_Movement>> SpawnMovements;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Update movement frame according to pawn */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void BuildMovementFrame();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Animation performance type */
	UPROPERTY(EditAnywhere, Category = "!TGOR Movement")
		TSubclassOf<UTGOR_Performance> PerformanceType;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Current movement state */
	UPROPERTY(ReplicatedUsing = RepNotifyState)
		FTGOR_MovementInput MovementInput;

	/** Current movement body */
	UPROPERTY(ReplicatedUsing = RepNotifyFrame)
		FTGOR_MovementFrame MovementFrame;

	UFUNCTION()
		void RepNotifyState(const FTGOR_MovementInput& Old);

	UFUNCTION()
		void RepNotifyFrame(const FTGOR_MovementFrame& Old);


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get current movement state */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		const FTGOR_MovementInput& GetState() const;

	/** Get current movement frame */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		const FTGOR_MovementFrame& GetFrame() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Sets local input and computes view input */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void AddLocalInputVector(const FVector& Input, float Strength);

	/** Find whether we are knocked out. Both movement master and authority can knock the pawn out, only server can recover. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool IsKnockedOut() const;

	/** Enables autonomy (should usually disable ragdoll), lerps towards 1.0 with given speed */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		void Knockin(float Speed = 1.0f);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Raw input */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Movement")
		FVector RawInput;

	/** Local input strength */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Movement")
		float InputStrength;

	/** Threshold for which pawn counts as being knocked out */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Internal")
		float KnockoutThreshold;

	/** Called when getting knocked out */
	UPROPERTY(BlueprintAssignable, Category = "!TGOR Movement")
		FKnockoutDelegate OnKnockout;

	/** Impact threshold when pawn gets knocked out from impact on the head */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Internal")
		float KnockoutHeadThreshold;

	/** Impact threshold when pawn gets knocked out from impact on the feet */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement|Internal")
		float KnockoutFootThreshold;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get attached movement task */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		UTGOR_MovementTask* GetMovementTask() const;

	/** Check whether a given pilot is attached */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		bool IsMovingWith(int32 Identifier) const;

	/** Set which pilot we're attaching to */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		void MoveWith(int32 Identifier);

	/** Get attached movement task */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		UTGOR_MovementTask* GetMovementOfType(TSubclassOf<UTGOR_MovementTask> Type) const;

	template<typename T> T* GetMOfType() const
	{
		return Cast<T>(GetMovementOfType(T::StaticClass()));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Currently assigned containers. */
	UPROPERTY(Replicated)
		TArray<UTGOR_MovementTask*> MovementSlots;

	/** Currently scheduled movement */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = RepNotifyMovementTaskState)
		FTGOR_MovementState MovementTaskState;

	/** Update movement on replication */
	UFUNCTION()
		void RepNotifyMovementTaskState(const FTGOR_MovementState& Old);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get raw input in input local space */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		FVector GetRawInput() const;

	/** Get input strength */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		float GetInputStrength() const;

	/** Get input rotation */
	UFUNCTION(BlueprintPure, Category = "!TGOR Movement|Internal", Meta = (Keywords = "C++"))
		FQuat GetInputRotation() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		float ComputeDirectionRatio(float ForwardRatio, float SpeedRatio, float LockTurn) const;

	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		float ComputeTorqueSpeedRatio(float SpeedRatio, float TorqueSpeedCutoff) const;

	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		FVector ComputeTorqueAlong(const FVector& TorqueAxis, const FVector& Forward, const FVector& Direction) const;

	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		FVector ComputeTorqueTowards(const FVector& Forward, const FVector& Direction) const;

	UFUNCTION(BlueprintPure, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		FVector ComputeForceTowards(const FVector& Forward, const FVector& Damping, float Strength, float SpeedRatio) const;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Update movement priority lists if not overriding, returns true if movement changed */
	bool UpdateMovement(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(unreliable, server, WithValidation)
		void ServerInputLight(FTGOR_MovementInput Update);
	void ServerInputLight_Implementation(FTGOR_MovementInput Update);
	bool ServerInputLight_Validate(FTGOR_MovementInput Update);

	UFUNCTION(reliable, Server, WithValidation)
		void ServerKnockout();
	void ServerKnockout_Implementation();
	bool ServerKnockout_Validate();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	virtual void PreComputePhysics(const FTGOR_MovementTick& Tick) override;
	virtual void ComputePhysics(FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, const FTGOR_MovementTick& Tick, FTGOR_MovementOutput& Output) override;
	virtual void PostComputePhysics(const FTGOR_MovementSpace& Space, float Energy, float DeltaTime) override;

	//virtual void Impact(const FTGOR_MovementDynamic& Dynamic, const FVector& Point, const FVector& Impact) override;
	//virtual bool CanInflict() const override;
	//virtual bool CanRotateOnImpact() const override;
};