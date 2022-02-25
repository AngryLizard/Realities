// TGOR (C) // CHECKED //
#pragma once

#include "Realities/Base/Instances/Creature/TGOR_CreatureInstance.h"
#include "Realities/Base/Instances/Creature/TGOR_TrackerInstance.h"
#include "Realities/Base/Instances/Creature/TGOR_MovementInstance.h"
#include "Realities/Base/Instances/Player/TGOR_UserInstance.h"
#include "Realities/Components/Player/TGOR_PlayerComponent.h"
#include "Realities/Components/Camera/TGOR_CameraComponent.h"
#include "Realities/Interfaces/TGOR_SingletonInterface.h"
#include "Realities/Interfaces/TGOR_DimensionInterface.h"
#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Realities/Base/Instances/Creature/TGOR_TrackerInstance.h"

#include "Components/BoxComponent.h"
#include "Engine/Classes/GameFramework/SpringArmComponent.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"

#include "Realities/Interfaces/TGOR_SaveInterface.h"
#include "GameFramework/Character.h"
#include "TGOR_Pawn.generated.h"

///////////////////////////////////////////////// DECL ///////////////////////////////////////////////////

class UTGOR_AimComponent;
class UTGOR_ModularSkeletalMeshComponent;
class UTGOR_DimensionPortalComponent;
class UTGOR_SimulationComponent;
class UTGOR_MovementComponent;
class UTGOR_GuidedComponent;
class UTGOR_CameraComponent;
class UTGOR_ActionComponent;
class UTGOR_PlayerComponent;
class UTGOR_IdentityComponent;
class UTGOR_SceneComponent;
class UTGOR_AnimInstance;
class UTGOR_PawnTracker;
class ATGOR_GateActor;
class UTGOR_ItemStorage;

/**
* TGOR_Pawn is the base class for player controlled characters. It provides
* access to basic components like a camera and access to the data components
*  of the game through the singleton class.
*/
UCLASS()
class REALITIES_API ATGOR_Pawn : public APawn, public ITGOR_SingletonInterface, public ITGOR_DimensionInterface, public ITGOR_SaveInterface
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
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		void OnAnimationChange(UTGOR_Animation* Old, UTGOR_Animation* New);

	/** Returns true if cursor should be displayed */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "TGOR Interface", Meta = (Keywords = "C++"))
		bool HasCursor();
		virtual bool HasCursor_Implementation();

	/** Returns true if Pawn should get despawned on player disconnect */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		bool DespawnsOnLogout();
		bool DespawnsOnLogout_Implementation();

	// Blueprint interface called after Assemble
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Game")
		void DimensionEntered();

	// Called if an item is given to this pawn directly
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Game")
		void OnItemGiven(UTGOR_ItemStorage* Storage);

	/** Called when Pawn left their current dimension volume */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "TGOR Volume", Meta = (Keywords = "C++"))
		void OnOutOfLevel();

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = "true"))
		UCapsuleComponent* CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UTGOR_PlayerComponent* PlayerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UTGOR_IdentityComponent* IdentityComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* SelfieStick;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UTGOR_CameraComponent* CameraComponent;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	FORCEINLINE UTGOR_IdentityComponent* GetIdentity() const { return IdentityComponent; }
	FORCEINLINE UCapsuleComponent* GetCapsule() const { return CapsuleComponent; }
	FORCEINLINE USpringArmComponent* GetSelfie() const { return SelfieStick; }
	FORCEINLINE UTGOR_CameraComponent* GetCamera() const { return CameraComponent; }

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Get skeletal mesh component */
	UFUNCTION(BlueprintPure, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		virtual USkeletalMeshComponent* GetSkeletalMesh() const;

	/** Computes closest portal of a given type */
	UFUNCTION(BlueprintPure, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		UTGOR_DimensionPortalComponent* GetClosestPortal(TSubclassOf<UTGOR_DimensionPortalComponent> Type) const;

	/** Get current dimension content */
	UFUNCTION(BlueprintPure, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		UTGOR_Dimension* GetDimensionContent() const;

	/** Get this pawn's spawner content */
	UFUNCTION(BlueprintPure, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		UTGOR_Spawner* GetSpawnerContent() const;

	/** Moves to another dimension if not in it already */
	UFUNCTION(BlueprintCallable, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		void SwapDimension(UTGOR_DimensionData* Other);

	/** Tries to give this pawn an item, returns leftovers */
	UFUNCTION(BlueprintCallable, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		virtual UTGOR_ItemStorage* PutStorage(UTGOR_ItemStorage* Storage);

	/** Gives access to this pawn's animation instance if there is one */
	UFUNCTION(BlueprintPure, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		virtual UTGOR_AnimInstance* GetAnimation() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Horizontal camera rotation around current up vector */
	UFUNCTION(BlueprintCallable, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		void RotateCameraHorizontally(float Amount);

	/** Vertical camera rotation around control rotation right axis */
	UFUNCTION(BlueprintCallable, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		void RotateCameraVertically(float Amount);

	/** Get camera rotation in global space */
	UFUNCTION(BlueprintCallable, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		FQuat GetCameraRotation() const;

	/** Get camera rotation in pawn local space */
	UFUNCTION(BlueprintCallable, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		FQuat ToLocalCameraRotation(const FTGOR_MovementPosition& Position) const;

	/** Set camera rotation from pawn local space */
	UFUNCTION(BlueprintCallable, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		void FromLocalCameraRotation(const FTGOR_MovementPosition& Position, const FQuat& Quat);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Returns weight this pawn is additionally carrying */
	UFUNCTION(BlueprintPure, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		virtual float GetCarryWeight() const;

	/** Get dimensions of bounding box (width, height, depth) */
	UFUNCTION(BlueprintPure, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		virtual FVector GetBoundingDimensions() const;

	/** Computes camera height */
	UFUNCTION(BlueprintPure, Category = "TGOR Camera", Meta = (Keywords = "C++"))
		virtual float GetCameraHeight(float Ratio) const;

	/** Get leg length in centimeters. For now leg length is stored in the Pawn but should preferably move e.g. to creature class */
	UFUNCTION(BlueprintPure, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		virtual float GetLegLength() const;

	/** Computes bottom of capsule */
	UFUNCTION(BlueprintPure, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		FVector GetFeetLocation() const;

	/** Called when pawn is out of level bounds */
	UFUNCTION(BlueprintCallable, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		void OutOfLevel();

	/** Current up vector */
	UFUNCTION(BlueprintPure, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		virtual FVector GetUpVector() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Called when movement changed animation */
	UFUNCTION(BlueprintCallable, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		virtual void AnimationChange(UTGOR_Animation* Old, UTGOR_Animation* New);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Clears and adds content classes to context. */
	UFUNCTION(BlueprintPure, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		virtual TSet<UTGOR_Content*> GetContext() const;
	
	/** Registers tracker information to this pawn with given context appendum. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		void PutTracker(TSubclassOf<UTGOR_Tracker> Type, const TSet<UTGOR_Content*>& Appendum, float Delta = 1.0f);


	/** Flushes tracker cache into another tracker database */
	UFUNCTION(BlueprintCallable, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		void FlushTracker(UPARAM(ref) FTGOR_TrackerData& Target);


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Registered tracker values */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Pawn", meta = (Keywords = "C++"))
		FTGOR_TrackerData Trackers;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:
private:

};