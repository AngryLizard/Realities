// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Realities/Actors/Pawns/TGOR_Pawn.h"
#include "Realities/Utility/Datastructures/TGOR_Time.h"
#include "Realities/Base/Instances/System/TGOR_IndexInstance.h"
#include "TGOR_Avatar.generated.h"

class UTGOR_HitVolumeComponent;
class UTGOR_InventoryComponent;
class UTGOR_EquipmentComponent;
class UTGOR_AbilityComponent;
class UTGOR_AvatarTracker;

UCLASS()
class REALITIES_API ATGOR_Avatar : public ATGOR_Pawn
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_Avatar(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual bool Assemble(UTGOR_DimensionData* Dimension) override;

	virtual FVector GetUpVector() const override;
	virtual TSet<UTGOR_Content*> GetContext() const override;
	virtual UTGOR_ItemStorage* PutStorage(UTGOR_ItemStorage* Storage) override;
	virtual UTGOR_AnimInstance* GetAnimation() const override;
	virtual FVector GetBoundingDimensions() const override;
	virtual void AnimationChange(UTGOR_Animation* Old, UTGOR_Animation* New) override;
	virtual USkeletalMeshComponent* GetSkeletalMesh() const override;

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;

	//////////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////

	/** Called when a character setup was applied. */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		void OnSetupChange(UTGOR_Creature* Creature);

	////////////////////////////////////////////////// COMPONENTS //////////////////////////////////////
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = "true"))
		UTGOR_SimulationComponent* SimulationComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = "true"))
		UTGOR_SceneComponent* VisualsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = "true"))
		UTGOR_ModularSkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = "true"))
		UTGOR_MovementComponent* MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* BoundingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UPhysicalAnimationComponent* PhysicsAnimationComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = "true"))
		UTGOR_EquipmentComponent* EquipmentComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = "true"))
		UTGOR_AbilityComponent* AbilityComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = "true"))
		UTGOR_InventoryComponent* InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UTGOR_HitVolumeComponent* CombatComponent;

public:

	FORCEINLINE UTGOR_InventoryComponent* GetInventory() const { return InventoryComponent; }
	FORCEINLINE UTGOR_EquipmentComponent* GetEquipment() const { return EquipmentComponent; }
	FORCEINLINE UTGOR_AbilityComponent* GetAbility() const { return AbilityComponent; }
	
	FORCEINLINE UTGOR_MovementComponent* GetMovement() const { return MovementComponent; }
	FORCEINLINE UTGOR_HitVolumeComponent* GetCombat() const { return CombatComponent; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get skeletal mesh */
	UFUNCTION(BlueprintPure, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		UTGOR_ModularSkeletalMeshComponent* GetMesh() const;


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Bone to use as profile root */
	UPROPERTY(EditAnywhere)
		FName PhysicalRootBone;

	/** Current strength of physical animation */
	UPROPERTY(EditAnywhere)
		float PhysicalAnimationStrength;

	/** Whether pyhsical animation strength is currently lerping */
	UPROPERTY(EditAnywhere)
		bool HasPhysicalAnimationLerp;

	/** Bones that are *not* lerped */
	UPROPERTY(EditAnywhere)
		TArray<FName> PhysicalAnimationUnchangedBones;

	/** Serverside character setup information */
	UPROPERTY(ReplicatedUsing = OnReplicateSetup, BlueprintReadOnly, Category = "TGOR Pawn")
		FTGOR_CreatureSetupInstance CurrentSetup;

	/** Changes physical animation to the current animation content */
	UFUNCTION(BlueprintCallable, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		void ApplyCurrentPhysicalAnimation();

	/** Changes physical animation to the current animation strength */
	UFUNCTION(BlueprintCallable, Category = "TGOR Pawn", Meta = (Keywords = "C++"))
		void ApplyCurrentPhysicalBlending();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Set and apply overall scale */
	UFUNCTION(BlueprintCallable, Category = "TGOR Creature", Meta = (Keywords = "C++"))
		void SetScale(float Scale = 1.0f);

	/** Set and apply setup to all components */
	UFUNCTION(BlueprintCallable, Category = "TGOR Creature", Meta = (Keywords = "C++"))
		void ApplySetup(const FTGOR_CreatureSetupInstance& Setup);

	/** Set and apply appearance specifically, updates setup struct */
	UFUNCTION(BlueprintCallable, Category = "TGOR Appearance", Meta = (Keywords = "C++"))
		void ApplyAppearance(const FTGOR_CreatureAppearanceInstance& Appearance);


	/** Called on each client on replication of current appearance structure. */
	UFUNCTION()
		void OnReplicateSetup();

	/** Get currently assigned creature */
	UFUNCTION(BlueprintPure, Category = "TGOR Creature", Meta = (Keywords = "C++"))
		UTGOR_Creature* GetCreature() const;

	/** Requests an appearance change (only allowed in the entry level) */
	UFUNCTION(BlueprintCallable, Reliable, Server, WithValidation, Category = "TGOR Creature", Meta = (Keywords = "C++"))
		void RequestAppearance(FTGOR_CreatureAppearanceInstance Appearance);
	void RequestAppearance_Implementation(FTGOR_CreatureAppearanceInstance Appearance);
	bool RequestAppearance_Validate(FTGOR_CreatureAppearanceInstance Appearance);


	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////
	
protected:

private:

};
