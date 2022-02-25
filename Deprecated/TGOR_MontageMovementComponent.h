// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_CreatureMovementComponent.h"
#include "TGOR_MontageMovementComponent.generated.h"

/**
 * TGOR_MontageMovementComponent adds montage functionliaty to the actor
 */
UCLASS()
class REALITIES_API UTGOR_MontageMovementComponent : public UTGOR_CreatureMovementComponent
{
	GENERATED_BODY()
	
public:
	UTGOR_MontageMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	void InitialiseMovementmode(EMovementMode CurrentMode, ETGOR_CustomMovementMode CustomMode) override;
	virtual bool CanCrouchInCurrentState() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** True if currently skiing */
	UFUNCTION(BlueprintPure, Category = "TGOR|MontageMovement", Meta = (Keywords = "C++"))
		bool IsInMontage() const;

	virtual void CalcCustomMontageVelocity(float DeltaTime) override;

	/** Location character lerps to in montage mode */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR|MontageMovement", Meta = (Keywords = "C++"))
		FVector MontageDelta;

	/** Character crouches in montage mode */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR|MontageMovement", Meta = (Keywords = "C++"))
		bool MontageCrouch;

	/** Character lerps in montage mode */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR|MontageMovement", Meta = (Keywords = "C++"))
		bool MontageLerp;

	/** Character lerp speed in montage mode */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR|MontageMovement", Meta = (Keywords = "C++"))
		float MontageSpeed;

	/** Time after followup mode gets enabled */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR|MontageMovement", Meta = (Keywords = "C++"))
		float MontageFollowupTimer;

	/** Movement mode after montage is done */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR|MontageMovement", Meta = (Keywords = "C++"))
		TEnumAsByte<EMovementMode> MontageFollowup;

	// Plays montage with given timer, crouched or uncrouched
	UFUNCTION(BlueprintCallable, Category = "TGOR|MontageMovement", Meta = (Keywords = "C++"))
		void PlayMontage(UAnimMontage* Montage, float Timer, bool Crouch);

protected:
	// Lerps given distance (Call after Playmontage to work)
	void LerpMontage(const FVector& Delta);


private:
	float _montageTimer;
};
