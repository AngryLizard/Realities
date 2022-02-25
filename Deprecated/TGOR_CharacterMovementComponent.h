// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#define TWOPI (6.28318530718f)

#include "Kismet/KismetSystemLibrary.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "TGOR_CharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	public:
		UTGOR_CharacterMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
		
		
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|CustomMovement", meta = (ClampMin = "0.1", ClampMax = "1.0"))
			float MinYawControl;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|CustomMovement", meta = (ClampMin = "1.0", ClampMax = "10.0"))
			float UTurnRate;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|CustomMovement", meta = (ClampMin = "0.0", ClampMax = "4.0"))
			float UTurnAngle;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|CustomMovement", meta = (ClampMin = "0.0", ClampMax = "90.0"))
			float MaxPitchAngle;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|CustomMovement", meta = (ClampMin = "0.0", ClampMax = "1.0"))
			float Pitchfactor;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|CustomMovement")
			float NormalPitch;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|CustomMovement")
			bool bAdaptToGround;
		
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|CustomMovement")
			TEnumAsByte<ETraceTypeQuery> GroundDetection;

		/** Direction character last turned towards */
		UPROPERTY(BlueprintReadWrite, Category = "TGOR|CustomMovement")
			float YawDirection;

		/** Current angle speed */
		UPROPERTY(BlueprintReadWrite, Category = "TGOR|CustomMovement")
			float YawVelocity;


	protected:

		virtual void InitializeComponent() override;
		FVector ConstrainInputAcceleration(const FVector& InputAcceleration) const override;
		void PhysicsRotation(float DeltaTime) override;

		virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

		void AdaptToGround();

	private:
		// Current movement values
		FVector Orientation;
		float LastYaw;
		float Angle;
		float Ratio;
};
