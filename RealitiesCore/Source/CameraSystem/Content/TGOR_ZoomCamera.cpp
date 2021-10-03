// The Gateway of Realities: Planes of Existence.


#include "TGOR_ZoomCamera.h"
#include "CameraSystem/Components/TGOR_CameraComponent.h"


UTGOR_ZoomCamera::UTGOR_ZoomCamera()
	: Super(),
	DefaultZoom(500.0f)
{
	Abstract = false;
}

FVector4 UTGOR_ZoomCamera::Initial(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm) const
{
	return FVector4(1.0f, 0.0f, 0.0f, 0.0f);
}

void UTGOR_ZoomCamera::Apply(UTGOR_CameraComponent* Camera, USpringArmComponent* SpringArm, const FVector4& Input)
{
	const float Scale = Camera->GetComponentScale().GetMax();
	SpringArm->TargetArmLength = (Input.X * DefaultZoom + Input.Y) * Scale;
}
