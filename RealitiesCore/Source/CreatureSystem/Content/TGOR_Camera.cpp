// The Gateway of Realities: Planes of Existence.


#include "TGOR_Camera.h"
#include "CreatureSystem/Components/TGOR_CameraComponent.h"


UTGOR_Camera::UTGOR_Camera()
	: Super(),
	DefaultBlendTime(2.0f)
{
}

FVector4 UTGOR_Camera::Initial(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm) const
{
	return FVector::ZeroVector;
}

FVector4 UTGOR_Camera::Filter(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm, const FVector4& Params) const
{
	return Params;
}

void UTGOR_Camera::Apply(UTGOR_CameraComponent* Camera, USpringArmComponent* SpringArm, const FVector4& Input)
{
}