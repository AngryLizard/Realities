// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Instances/Creature/TGOR_MovementInstance.h"

#include "Realities/Interfaces/TGOR_RegisterInterface.h"
#include "Realities/Interfaces/TGOR_DimensionInterface.h"
#include "Realities/Components/Movement/TGOR_SymplecticComponent.h"
#include "TGOR_MovementComponent.generated.h"

class ATGOR_Pawn;
class ATGOR_Avatar;
class UTGOR_Movement;
class UTGOR_Creature;
class UTGOR_Animation;
class UTGOR_NamedSocket;

DECLARE_STATS_GROUP(TEXT("TGOR Movement System"), STATGROUP_TGOR_MOV, STATCAT_Advanced);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FKnockoutDelegate);

/**
 * 
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_MovementComponent : public UTGOR_SymplecticComponent, public ITGOR_DimensionInterface
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	UTGOR_MovementComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual int32 GetIndexFromName(const FName& Name) const override;
	virtual FName GetNameFromIndex(int32 Index) const override;

	virtual FTGOR_MovementDynamic GetIndexTransform(UTGOR_Socket* Socket, int32 Index) const override;
	virtual void AttachToMovement(USceneComponent* Component, UTGOR_Socket* Socket, int32 Index) override;

	virtual bool PreAssemble(UTGOR_DimensionData* Dimension) override;
	virtual bool UpdatesBase() const override;

	virtual TSet<UTGOR_Content*> GetActiveContent_Implementation() const override;

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////

	/** Assigned pawn */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "TGOR Pawn")
		ATGOR_Pawn* Pawn;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Fills movement queues from creature setup */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void BuildMovementQueues(UTGOR_Creature* OwningCreature);

	/** Update movement frame according to pawn */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void BuildMovementFrame(UTGOR_Creature* OwningCreature);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Calls simulation tick externally instead of component tick */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		bool PreemptSimulation(float Timestep);

	/** Calls simulation tick externally instead of movement tick (Overrides movement for one tick if defined) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void PreemptMovement(float Timestep, const FVector& Input, const FVector& View, UTGOR_Movement* Override = nullptr);

	/** Preemption wrapper for overriding Input only */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void PreemptMovementInput(float Timestep, const FVector& Input, const FVector& View);

	/** Preemption wrapper for overriding movement only */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void PreemptMovementOverride(float Timestep, TSubclassOf<UTGOR_Movement> Movement);

	/** Preemption wrapper for overriding movement and movement base */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void PreemptMovementBaseOverride(float Timestep, TSubclassOf<UTGOR_Movement> Movement, UTGOR_MobilityComponent* Component);

	/** Preemption wrapper for overriding input and movement only */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void PreemptMovementInputOverride(float Timestep, const FVector& Input, const FVector& View, TSubclassOf<UTGOR_Movement> Movement);

	/** Change scale of this component */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void SetScale(float Scale);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Peaceful movement modes */
	UPROPERTY(BlueprintReadOnly)
		TArray<UTGOR_Movement*> MovementQueue;
	
	/** Animation slot name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		FName AnimationSlotName;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Current movement state */
	UPROPERTY(ReplicatedUsing = RepNotifyState, EditAnywhere, Category = "TGOR Movement")
		FTGOR_MovementState State;

	/** Current movement content */
	UPROPERTY(ReplicatedUsing = RepNotifyContent, EditAnywhere, Category = "TGOR Movement")
		FTGOR_MovementContent Content;

	/** Current movement body */
	UPROPERTY(ReplicatedUsing = RepNotifyFrame, EditAnywhere, Category = "TGOR Movement")
		FTGOR_MovementFrame Frame;

	UFUNCTION()
		void RepNotifyState(const FTGOR_MovementState& Old);

	UFUNCTION()
		void RepNotifyContent(const FTGOR_MovementContent& Old);

	UFUNCTION()
		void RepNotifyFrame(const FTGOR_MovementFrame& Old);


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get current movement state */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		const FTGOR_MovementState& GetState() const;

	/** Get current movement content */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		const FTGOR_MovementContent& GetContent() const;

	/** Get current movement frame */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		const FTGOR_MovementFrame& GetFrame() const;

	/** Updates the current state to client */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void ForceAdjustCurrentToClient();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Compute transformed input vector from the currently active movement */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void GetMovementInput(FVector& Input, FVector& View) const;

	/** Sets local input and computes view input */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void SetInput(const FVector& Input, float Strength);


	/** Find whether we are knocked out. Both movement master and authority can knock the pawn out, only server can recover. */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		bool IsKnockedOut() const;
	
	/** Enables autonomy (should usually disable ragdoll), lerps towards 1.0 with given speed */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		void Knockin(float Speed = 1.0f);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Raw input */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Movement")
		FVector RawInput;

	/** Local input strength */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Movement")
		float InputStrength;

	/** Camera rotation in world space */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Movement")
		FQuat InputRotation;


	/** Threshold for which pawn counts as being knocked out */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float KnockoutThreshold;

	/** Called when getting knocked out */
	UPROPERTY(BlueprintAssignable, Category = "TGOR Movement")
		FKnockoutDelegate OnKnockout;

	/** Impact threshold when pawn gets knocked out from impact on the head */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float KnockoutHeadThreshold;

	/** Impact threshold when pawn gets knocked out from impact on the feet */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Movement")
		float KnockoutFootThreshold;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get raw input in input local space */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		FVector GetRawInput() const;

	/** Get input strength */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		float GetInputStrength() const;

	/** Get input rotation */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		FQuat GetInputRotation() const;


	/** Performs a sphere trace from origin in any direction, parameters given in visual local space */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void VisualTrace(const FVector& Offset, const FVector& Direction, float Distance, float Radius, FTGOR_VisualContact& Contact) const;

	/** Performs a sphere trace from origin in any direction, parameters given in world space */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		bool CenteredTrace(const FVector& Location, const FVector& Offset, const FVector& Direction, float Distance, float Radius, FHitResult& HitOut) const;

	/** Performs four traces for smoother ground detection, uses principal to offset samples. 
	The triangulated normal is given in HitOut.ImpactNormal, the averaged weight is given in HitOut.Normal.
	Parameters given in local space (state)*/
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		bool SampleTrace(const FVector& Location, const FVector& Direction, const FVector& Principal, float Distance, float Radius, FHitResult& HitOut) const;

	/** Performs a capsule overlap from origin */
	UFUNCTION(BlueprintCallable, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		bool Overlap(const FVector& Location, const FQuat& Rotation, float Radius, float HalfSize, FHitResult& HitOut) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Many objects want to orient themselves before starting to move. This function returns a smooth ratio that ramps up either when the forward ratio is near 1 (depending on LockTurn) or when already moving. */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		float ComputeDirectionRatio(float ForwardRatio, float SpeedRatio, float LockTurn) const;

	/** Many objects rotate slower the faster they go. This function returns a smooth ratio that ramps up the slower (depending on SpeedCutoff). */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		float ComputeTorqueSpeedRatio(float SpeedRatio, float TorqueSpeedCutoff) const;

	/** Compute torque to rotate Forward towards Direction along TorqueAxis. */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		FVector ComputeTorqueAlong(const FVector& TorqueAxis, const FVector& Forward, const FVector& Direction) const;

	/** Compute torque to rotate Forward towards Direction freely. */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		FVector ComputeTorqueTowards(const FVector& Forward, const FVector& Direction) const;

	/** Compute force along Forward direction counteracting damping in movement direction so that SpeedRatio goes towards 1.0 (aka, braking when going too fast or accelerating when too slow). */
	UFUNCTION(BlueprintPure, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		FVector ComputeForceTowards(const FVector& Forward, const FVector& Damping, float Strength, float SpeedRatio) const;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	virtual void TickReset(float Time) override;
	virtual void PreComputePhysics(const FTGOR_MovementTick& Tick, bool Replay) override;
	virtual void ComputePhysics(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External, bool Replay, FTGOR_MovementOutput& Output) override;

	virtual void MovementUpdate() override;
	virtual void OutOfLevel() override;
	virtual void Impact(const FTGOR_MovementDynamic& Dynamic, const FVector& Point, const FVector& Impact) override;
	virtual bool CanInflict() const override;
	virtual bool CanRotateOnImpact() const override;

	/** Update movement priority lists if not overriding */
	void UpdateMovement(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External);

	/** Update animation content */
	void UpdateAnimation(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, const FTGOR_MovementExternal& External);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(unreliable, server, WithValidation)
		void ServerInputLight(FTGOR_MovementState Update);
		void ServerInputLight_Implementation(FTGOR_MovementState Update);
		bool ServerInputLight_Validate(FTGOR_MovementState Update);

	UFUNCTION(reliable, Server, WithValidation)
		void ServerKnockout();
		void ServerKnockout_Implementation();
		bool ServerKnockout_Validate();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UFUNCTION(unreliable, Server, WithValidation)
		void ServerContentForceUpdate(FTGOR_MovementInputUpdate Update);
	void ServerContentForceUpdate_Implementation(FTGOR_MovementInputUpdate Update);
	bool ServerContentForceUpdate_Validate(FTGOR_MovementInputUpdate Update);

	UFUNCTION(unreliable, Client)
		void ClientContentForceUpdate(FTGOR_MovementContentUpdate Update);
	void ClientContentForceUpdate_Implementation(FTGOR_MovementContentUpdate Update);


	UFUNCTION(unreliable, Server, WithValidation)
		void ServerContentBufferAdjust(FTGOR_MovementInputUpdate Update);
	void ServerContentBufferAdjust_Implementation(FTGOR_MovementInputUpdate Update);
	bool ServerContentBufferAdjust_Validate(FTGOR_MovementInputUpdate Update);

	UFUNCTION(unreliable, Client)
		void ClientContentBufferAdjust(FTGOR_MovementContentUpdate Update);
	void ClientContentBufferAdjust_Implementation(FTGOR_MovementContentUpdate Update);

	UFUNCTION(unreliable, NetMulticast)
		void MulticastContentBufferAdjust(FTGOR_MovementContentUpdate Update);
	void MulticastContentBufferAdjust_Implementation(FTGOR_MovementContentUpdate Update);
	
	/////////////////////

	virtual bool BufferPeerAdjust(int32 PrevIndex, int32 NextIndex, const FTGOR_MovementThreshold& Threshold) override;
	virtual void BufferReplicate(int32 Index) override;

	/////////////////////

	virtual void LoadFromBufferExternal(int32 Index) override;
	virtual void LoadFromBufferWhole(int32 Index) override;
	virtual void LerpToBuffer(int32 PrevIndex, int32 NextIndex, float Alpha) override;
	virtual void StoreToBuffer(int32 Index) override;
	virtual void SetBufferSize(int32 Size) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Create movement update from index */
	UFUNCTION(BlueprintCallable, Category = "TGOR Network", Meta = (Keywords = "C++"))
		FTGOR_MovementInputUpdate CreateMovementInputUpdate(int32 Index) const;

	float ContentBufferCompare(const FTGOR_MovementContentUpdate& Update, int32 Index, const FTGOR_MovementThreshold& Threshold) const;
	void ContentBufferOverride(const FTGOR_MovementContentUpdate& Update, int32 Index);

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** State buffer */
	UPROPERTY()
		TArray<FTGOR_MovementState> StateBuffer;

	/** Content buffer */
	UPROPERTY()
		TArray<FTGOR_MovementContent> ContentBuffer;

	/** Frame buffer */
	UPROPERTY()
		TArray<FTGOR_MovementFrame> FrameBuffer;
};