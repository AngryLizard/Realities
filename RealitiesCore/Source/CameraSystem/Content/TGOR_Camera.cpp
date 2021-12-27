// The Gateway of Realities: Planes of Existence.


#include "TGOR_Camera.h"
#include "CameraSystem/Components/TGOR_CameraComponent.h"


UTGOR_Camera::UTGOR_Camera()
	: Super()
{
}

FVector4 UTGOR_Camera::Initial(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm) const
{
	return FVector4(0,0,0,0);
}

FVector4 UTGOR_Camera::Filter(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm, const FVector& Params) const
{
	return FVector4(Params, 1);
}

void UTGOR_Camera::Apply(UTGOR_CameraComponent* Camera, USpringArmComponent* SpringArm, const FVector4& Input)
{
}