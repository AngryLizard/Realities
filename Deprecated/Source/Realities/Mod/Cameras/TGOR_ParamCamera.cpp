// The Gateway of Realities: Planes of Existence.


#include "TGOR_ParamCamera.h"
#include "Realities/Components/Camera/TGOR_CameraComponent.h"


UTGOR_ParamCamera::UTGOR_ParamCamera()
	: Super(),
	DefaultParam(FVector::ZeroVector)
{
}

FVector4 UTGOR_ParamCamera::Initial(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm) const
{
	return DefaultParam;
}

void UTGOR_ParamCamera::Apply(UTGOR_CameraComponent* Camera, USpringArmComponent* SpringArm, const FVector4& Input)
{
	ApplyParameter(Input);
}
