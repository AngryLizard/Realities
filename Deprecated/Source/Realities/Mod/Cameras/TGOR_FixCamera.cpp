// The Gateway of Realities: Planes of Existence.


#include "TGOR_FixCamera.h"
#include "Realities/Components/Camera/TGOR_CameraComponent.h"

UTGOR_FixCamera::UTGOR_FixCamera()
	: Super()
{
	Abstract = false;
}

FVector4 UTGOR_FixCamera::Initial(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm) const
{
	return FVector4(Camera->GetDesiredCameraLocation(), 0.0f);
}

void UTGOR_FixCamera::Apply(UTGOR_CameraComponent* Camera, USpringArmComponent* SpringArm, const FVector4& Input)
{
	Camera->SetWorldLocation(Input);
}
