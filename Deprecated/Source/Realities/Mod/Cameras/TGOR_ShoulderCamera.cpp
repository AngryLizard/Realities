// The Gateway of Realities: Planes of Existence.


#include "TGOR_ShoulderCamera.h"
#include "Realities/Components/Camera/TGOR_CameraComponent.h"


UTGOR_ShoulderCamera::UTGOR_ShoulderCamera()
	: Super(),
	DefaultShoulder(0.0f)
{
	Abstract = false;
}

FVector4 UTGOR_ShoulderCamera::Initial(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm) const
{
	return FVector4(0.0f, DefaultShoulder, 0.0f, 0.0f);
}

void UTGOR_ShoulderCamera::Apply(UTGOR_CameraComponent* Camera, USpringArmComponent* SpringArm, const FVector4& Input)
{
	const float Scale = Camera->GetComponentScale().GetMax();
	SpringArm->SocketOffset.Y = (FVector(Input) | FVector::OneVector) * Scale;
}
