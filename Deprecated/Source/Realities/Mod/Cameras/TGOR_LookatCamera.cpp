// The Gateway of Realities: Planes of Existence.


#include "TGOR_LookatCamera.h"
#include "Realities/Components/Camera/TGOR_CameraComponent.h"


UTGOR_LookatCamera::UTGOR_LookatCamera()
	: Super(),
	MinControlDistance(100.0f),
	MaxControlDistance(200.0f)
{
	Abstract = false;
}

FVector4 UTGOR_LookatCamera::Initial(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm) const
{
	const FVector Origin = Camera->GetComponentLocation();
	const FQuat ViewRotation = Camera->GetViewRotation();

	// Get point in front of camera as default
	return FVector4(Origin + ViewRotation.Vector() * 512.0f, 0.0f);
}

FVector4 UTGOR_LookatCamera::Filter(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm, const FVector4& Params) const
{
	return Params + FVector4(FVector::ZeroVector, 1.0f);
}

void UTGOR_LookatCamera::Apply(UTGOR_CameraComponent* Camera, USpringArmComponent* SpringArm, const FVector4& Input)
{
	const FVector Origin = Camera->GetComponentLocation();
	const FQuat ViewRotation = Camera->GetViewRotation();

	// Rotate camera to face SpringArm origin
	const FVector Delta = FVector(Input) - Origin;
	if (Delta.SizeSquared() > SMALL_NUMBER)
	{
		const FQuat Quat = FRotationMatrix::MakeFromXZ(Delta, ViewRotation.GetAxisZ()).ToQuat();
		Camera->SetWorldRotation(Quat.GetNormalized());
		//Camera->SetWorldRotation(FQuat::FastLerp(ViewRotation, Quat, Input.W).GetNormalized());
	}

	Camera->SetControlThrottle(FMath::Min(Input.W * 4, 1.0f));
}
