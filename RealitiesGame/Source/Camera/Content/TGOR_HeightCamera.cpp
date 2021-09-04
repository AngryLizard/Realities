// The Gateway of Realities: Planes of Existence.


#include "TGOR_HeightCamera.h"
#include "CreatureSystem/Components/TGOR_CameraComponent.h"


UTGOR_HeightCamera::UTGOR_HeightCamera()
	: Super(),
	DefaultHeight(100.0f)
{
	Abstract = false;
}

FVector4 UTGOR_HeightCamera::Initial(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm) const
{
	return FVector4(0.0f, 0.0f, DefaultHeight, 0.0f);
}

void UTGOR_HeightCamera::Apply(UTGOR_CameraComponent* Camera, USpringArmComponent* SpringArm, const FVector4& Input)
{
	const float Scale = Camera->GetComponentScale().GetMax();
	SpringArm->SocketOffset.Z = (FVector(Input) | FVector::OneVector) * Scale;
}
