// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "TGOR_Pawn.h"
#include "TGOR_Avatar.generated.h"

class UTGOR_MatterComponent;
class UTGOR_HitVolumeComponent;
class UTGOR_InventoryComponent;
class UTGOR_EquipmentComponent;
class UTGOR_AttributeComponent;
class UTGOR_AbilityComponent;
class UTGOR_EffectComponent;
class UTGOR_AvatarTracker;

UCLASS()
class CREATURESYSTEM_API ATGOR_Avatar : public ATGOR_Pawn
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

	virtual TSet<UTGOR_CoreContent*> GetContext() const override;
	virtual UTGOR_ItemStorage* PutStorage(UTGOR_ItemStorage* Storage) override;
	virtual UTGOR_AnimInstance* GetAnimation() const override;
	virtual void AnimationChange(UTGOR_Animation* Old, UTGOR_Animation* New) override;
	virtual USkeletalMeshComponent* GetSkeletalMesh() const override;

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;

	//////////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////

	////////////////////////////////////////////////// COMPONENTS //////////////////////////////////////
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = "true"))
		UTGOR_SimulationComponent* SimulationComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = "true"))
		UTGOR_SceneComponent* VisualsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = "true"))
		UTGOR_ModularSkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UPhysicalAnimationComponent* PhysicsAnimationComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = "true"))
		UTGOR_ActionComponent* ActionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, Meta = (AllowPrivateAccess = "true"))
		UTGOR_InventoryComponent* InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UTGOR_HitVolumeComponent* CombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UTGOR_MatterComponent* MatterComponent;

public:

	FORCEINLINE UTGOR_InventoryComponent* GetInventory() const { return InventoryComponent; }
	FORCEINLINE UTGOR_ActionComponent* GetAction() const { return ActionComponent; }
	
	FORCEINLINE UTGOR_HitVolumeComponent* GetCombat() const { return CombatComponent; }
	FORCEINLINE UTGOR_MatterComponent* GetMatter() const { return MatterComponent; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get skeletal mesh */
	UFUNCTION(BlueprintPure, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
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

	/** Changes physical animation to the current animation content */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Pawn|Internal", Meta = (Keywords = "C++"))
		void ApplyCurrentPhysicalAnimation();

	/** Changes physical animation to the current animation strength */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Pawn|Internal", Meta = (Keywords = "C++"))
		void ApplyCurrentPhysicalBlending();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Set and apply overall scale */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Creature", Meta = (Keywords = "C++"))
		void SetScale(float Scale = 1.0f);


	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////
	
protected:

private:

};
