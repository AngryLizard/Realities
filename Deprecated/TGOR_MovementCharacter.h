// TGOR (C) // CHECKED //
#pragma once

#include "Utility/TGOR_CustomEnumerations.h"

#include "TGOR_Battleable.h"
#include "TGOR_MovementCharacter.generated.h"

///////////////////////////////////////////////// DECL ///////////////////////////////////////////////////

class UTGOR_ReplicatedMovementComponent;
class UTGOR_MovementModeComponent;
class UTGOR_SimulationComponent;
class UTGOR_StaminaComponent;
class UTGOR_WeaponActor;

///////////////////////////////////////////////// ENUM ///////////////////////////////////////////////////

UENUM(BlueprintType)
enum class ETGOR_InputBaseEnumeration : uint8
{
	LocalSpace,
	WorldSpace,
	PlaneSpace,
	WallSpace
};

/**
* TGOR_MovementCharacter adds character functionality for advanced movement such as climbing, swimming, etc.
* It also adds a bit of logic to control movement mode changes related to combat.
*/
UCLASS()
class REALITIES_API ATGOR_MovementCharacter : public ATGOR_Battleable
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_MovementCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	bool CanJumpInternal_Implementation() const override;

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** Movement has been updated */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR|Creature", Meta = (Keywords = "C++"))
		void MovementUpdate(EMovementMode Mode, ETGOR_CustomMovementMode Custom);

	/** Movement mode updated */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR|Creature", Meta = (Keywords = "C++"))
		void MovementModeChanged(EMovementMode Mode, ETGOR_CustomMovementMode Custom);

	/** Defines if health regenerates */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR|Creature", Meta = (Keywords = "C++"))
		bool CanRegenHealth(EMovementMode Mode, ETGOR_CustomMovementMode Custom);

	/** Defines if energy regenerates */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR|Creature", Meta = (Keywords = "C++"))
		bool CanRegenEnergy(EMovementMode Mode, ETGOR_CustomMovementMode Custom);

	/** Defines if stamina regenerates */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR|Creature", Meta = (Keywords = "C++"))
		bool CanRegenStamina(EMovementMode Mode, ETGOR_CustomMovementMode Custom);

	/** Desync detected (Client only) */
	UFUNCTION(BlueprintNativeEvent, Category = "TGOR|Creature", Meta = (Keywords = "C++"))
		void DesyncDetected();
		void DesyncDetected_Implementation();

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
		
	/** Cached cast of movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TGOR|Creature", Meta = (Keywords = "C++"))
		UTGOR_ReplicatedMovementComponent* CreatureMovementComponent;

	/** Simulation component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TGOR|Creature", meta = (AllowPrivateAccess = "true"))
		UTGOR_SimulationComponent* SimulationComponent;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Get base input gets applied to */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Creature", Meta = (Keywords = "C++"))
		void GetInputBase(FVector& Forward, FVector& Right, FVector& Up);

	/** Plays a montage (not replicated) */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Creature", Meta = (Keywords = "C++"))
		void PlayMontage(UAnimMontage* Montage, float Timer, bool Crouch);

	/** Apply movement to character relative to Camera (true) or relative to Character (false) */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Creature", Meta = (Keywords = "C++"))
		void ApplyMovement();

	/** Detect desync */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Creature", Meta = (Keywords = "C++"))
		void DetectDesync();
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Replicates Acceleration */
	UPROPERTY(Replicated)
		FVector ReplicatedInput;

	/** Input strength */
	UPROPERTY(BlueprintReadWrite, Category = "TGOR|Creature")
		float InputStrength;

	/** Input to the top */
	UPROPERTY(BlueprintReadWrite, Category = "TGOR|Creature")
		float InputUpward;

	/** Input to the front */
	UPROPERTY(BlueprintReadWrite, Category = "TGOR|Creature")
		float InputForward;

	/** Input to the right */
	UPROPERTY(BlueprintReadWrite, Category = "TGOR|Creature")
		float InputRight;
	
	/** Currently active input base */
	UPROPERTY(BlueprintReadWrite, Category = "TGOR|Creature", Meta = (Keywords = "C++"))
		ETGOR_InputBaseEnumeration InputBase;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

	virtual float GetCameraHeight(float Ratio) override;
	virtual bool CanEnterAggressive() override;
	virtual void ChangedAggressive(bool Aggressive) override;

protected:

private:

	int32 _desyncsThisTick;
};