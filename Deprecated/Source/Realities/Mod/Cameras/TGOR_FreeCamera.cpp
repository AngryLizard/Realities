// The Gateway of Realities: Planes of Existence.


#include "TGOR_FreeCamera.h"
#include "Realities/Components/Camera/TGOR_CameraComponent.h"

UTGOR_FreeCamera::UTGOR_FreeCamera()
	: Super()
{
	Abstract = false;
}

FVector4 UTGOR_FreeCamera::Initial(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm) const
{
	return FVector4(1.0f, 0.0f, 0.0f, 0.0f);
}

void UTGOR_FreeCamera::Apply(UTGOR_CameraComponent* Camera, USpringArmComponent* SpringArm, const FVector4& Input)
{
	Camera->SetCollisionStrength(FVector(Input) | FVector::OneVector);
}
