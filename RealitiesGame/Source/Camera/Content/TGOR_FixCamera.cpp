// The Gateway of Realities: Planes of Existence.


#include "TGOR_FixCamera.h"
#include "CreatureSystem/Components/TGOR_CameraComponent.h"

UTGOR_FixCamera::UTGOR_FixCamera()
	: Super()
{
	Abstract = false;
}

FVector4 UTGOR_FixCamera::Initial(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm) const
{
	const FVector Desired = Camera->GetDesiredCameraLocation();
	return FVector4(Desired, 0.0f);
}

void UTGOR_FixCamera::Apply(UTGOR_CameraComponent* Camera, USpringArmComponent* SpringArm, const FVector4& Input)
{
	Camera->SetWorldLocation(Input);
}
