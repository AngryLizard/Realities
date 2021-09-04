// The Gateway of Realities: Planes of Existence.


#include "TGOR_PivotCamera.h"
#include "CreatureSystem/Components/TGOR_CameraComponent.h"


UTGOR_PivotCamera::UTGOR_PivotCamera()
	: Super()
{
	Abstract = false;
}

FVector4 UTGOR_PivotCamera::Initial(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm) const
{
	return FVector4(0.0f, 0.0f, 0.0f, 0.0f);
}

void UTGOR_PivotCamera::Apply(UTGOR_CameraComponent* Camera, USpringArmComponent* SpringArm, const FVector4& Input)
{
	SpringArm->SetRelativeLocation(FVector(Input));
}
