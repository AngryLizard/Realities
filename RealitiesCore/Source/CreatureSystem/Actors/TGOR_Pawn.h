// TGOR (C) // CHECKED //
#pragma once

#include "Components/BoxComponent.h"
#include "Engine/Classes/GameFramework/SpringArmComponent.h"

#include "MovementSystem/TGOR_MovementInstance.h"
#include "KnowledgeSystem/TGOR_TrackerInstance.h"

#include "CreatureSystem/Components/TGOR_CameraComponent.h"

#include "CoreSystem/Storage/TGOR_SaveInterface.h"
#include "DimensionSystem/Interfaces/TGOR_DimensionInterface.h"
#include "GameFramework/Character.h"
#include "TGOR_Pawn.generated.h"

///////////////////////////////////////////////// DECL ///////////////////////////////////////////////////

class UTGOR_AimComponent;
class UTGOR_ModularSkeletalMeshComponent;
class UTGOR_DimensionReceiverComponent;
class UTGOR_SimulationComponent;
class UTGOR_ArmatureComponent;
class UTGOR_MovementComponent;
class UTGOR_GuidedComponent;
class UTGOR_CameraComponent;
class UTGOR_ActionComponent;
class UTGOR_TrackedComponent;
class UTGOR_AttributeComponent;
class UTGOR_StatComponent;
class UTGOR_SceneComponent;
class UTGOR_AnimInstance;
class UTGOR_PawnTracker;
class ATGOR_GateActor;
class UTGOR_ItemStorage;
class UTGOR_Character;

/**
* TGOR_Pawn is the base class for player controlled characters. It provides
* access to basic components like a camera and access to the data components
*  of the game through the singleton class.
*/
UCLASS()
class CREATURESYSTEM_API ATGOR_Pawn : public APawn, public ITGOR_SingletonInterface, public ITGOR_DimensionInterface, public ITGOR_SaveInterface
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_Pawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	virtual bool Assemble(UTGOR_DimensionData* Dimension) override;
	
	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** Called when some animation changed. To be used e.g. for ragdoll system. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		void OnAnimationChange(UTGOR_Animation* Old, UTGOR_Animation* New);

	/** Returns true if cursor should be displayed */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "!TGOR Interface", Meta = (Keywords = "C++"))
		bool HasCursor();
		virtual bool HasCursor_Implementation();

	/** Returns true if Pawn should get despawned on player disconnect */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		bool DespawnsOnLogout();
		bool DespawnsOnLogout_Implementation();

	// Blueprint interface called after Assemble
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Game")
		void DimensionEntered();

	// Called if an item is given to this pawn directly
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Game")
		void OnItemGiven(UTGOR_ItemStorage* Storage);

	/** Called when Pawn left their current dimension volume */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "!TGOR Volume", Meta = (Keywords = "C++"))
		void OnOutOfLevel();

	/** Called when a controlling player has just switched bodies. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		void OnUnposession();

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UTGOR_TrackedComponent* IdentityComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = "true"))
		UTGOR_ArmatureComponent* ArmatureComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = "true"))
		UTGOR_MovementComponent* MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* SelfieStick;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UTGOR_CameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UTGOR_StatComponent* StatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = "true"))
		UTGOR_AttributeComponent* AttributeComponent;


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	FORCEINLINE UTGOR_TrackedComponent* GetIdentity() const { return IdentityComponent; }

	FORCEINLINE UTGOR_ArmatureComponent* GetArmature() const { return ArmatureComponent; }
	FORCEINLINE UTGOR_MovementComponent* GetMovement() const { return MovementComponent; }
	FORCEINLINE USpringArmComponent* GetSelfie() const { return SelfieStick; }
	FORCEINLINE UTGOR_CameraComponent* GetCamera() const { return CameraComponent; }
	FORCEINLINE UTGOR_StatComponent* GetStats() const { return StatComponent; }
	FORCEINLINE UTGOR_AttributeComponent* GetAttributes() const { return AttributeComponent; }


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get skeletal mesh component */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		virtual USkeletalMeshComponent* GetSkeletalMesh() const;

	/** Computes closest portal of a given type */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		UTGOR_DimensionReceiverComponent* GetClosestPortal(TSubclassOf<UTGOR_DimensionReceiverComponent> Type) const;

	/** Get current dimension content */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		UTGOR_Dimension* GetDimensionContent() const;

	/** Get this pawn's spawner content */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		UTGOR_Creature* GetSpawnerContent() const;

	/** Moves to another dimension if not in it already */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Pawn|Internal", Meta = (Keywords = "C++"))
		void SwapDimension(UTGOR_DimensionData* Other);

	/** Tries to give this pawn an item, returns leftovers */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		virtual UTGOR_ItemStorage* PutStorage(UTGOR_ItemStorage* Storage);

	/** Gives access to this pawn's animation instance if there is one */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		virtual UTGOR_AnimInstance* GetAnimation() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Horizontal camera rotation around current up vector */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		void RotateCameraHorizontally(float Amount);

	/** Vertical camera rotation around control rotation right axis */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		void RotateCameraVertically(float Amount);

	/** Get camera rotation in global space */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		FQuat GetCameraRotation() const;

	/** Get camera rotation in pawn local space */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		FQuat ToLocalCameraRotation(const FTGOR_MovementPosition& Position) const;

	/** Set camera rotation from pawn local space */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Camera", Meta = (Keywords = "C++"))
		void FromLocalCameraRotation(const FTGOR_MovementPosition& Position, const FQuat& Quat);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Returns weight this pawn is additionally carrying */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pawn|Internal", Meta = (Keywords = "C++"))
		virtual float GetCarryWeight() const;

	/** Computes camera height */
	UFUNCTION(BlueprintPure, Category = "!TGOR Camera|Internal", Meta = (Keywords = "C++"))
		virtual float GetCameraHeight(float Ratio) const;

	/** Get leg length in centimeters. For now leg length is stored in the Pawn but should preferably move e.g. to creature class */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		virtual float GetLegLength() const;

	/** Computes bottom of capsule */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		FVector GetFeetLocation() const;

	/** Called when pawn is out of level bounds */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Pawn|Internal", Meta = (Keywords = "C++"))
		void OutOfLevel();

	/** Current up vector */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		virtual FVector GetUpVector() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Called when movement changed animation */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Pawn|Internal", Meta = (Keywords = "C++"))
		virtual void AnimationChange(UTGOR_Animation* Old, UTGOR_Animation* New);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Clears and adds content classes to context. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		virtual TSet<UTGOR_CoreContent*> GetContext() const;
	
	/** Registers tracker information to this pawn with given context appendum. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		void PutTracker(TSubclassOf<UTGOR_Tracker> Type, const TSet<UTGOR_CoreContent*>& Appendum, float Delta = 1.0f);


	/** Flushes tracker cache into another tracker database */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Pawn|Internal", Meta = (Keywords = "C++"))
		void FlushTracker(UPARAM(ref) FTGOR_TrackerData& Target);


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Registered tracker values */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Pawn", meta = (Keywords = "C++"))
		FTGOR_TrackerData Trackers;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:
private:

};