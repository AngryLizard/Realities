// The Gateway of Realities: Planes of Existence.


#include "TGOR_DollyCamera.h"
#include "CameraSystem/Components/TGOR_CameraComponent.h"


UTGOR_DollyCamera::UTGOR_DollyCamera()
	: Super(),
	DefaultFOV(100.0f),
	DollyStrength(0.9f),
	VignetteStrength(1.5f)
{
	Abstract = false;
}


FVector4 UTGOR_DollyCamera::Initial(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm) const
{
	return FVector4(DefaultFOV, DefaultVignette, 1.0f, 0.0f);
}

FVector4 UTGOR_DollyCamera::Filter(const UTGOR_CameraComponent* Camera, const USpringArmComponent* SpringArm, const FVector& Params) const
{
	// FOV
	const float Param = Params.X;
	const float FOV = DefaultFOV * (1.0f - DollyStrength * Param);

	// Vignette effect
	const float Interval = FOV - DefaultFOV;
	if (FMath::Abs(Interval) > SMALL_NUMBER)
	{
		const float Ratio = (Camera->FieldOfView - DefaultFOV) / Interval;
		const float Vignette = FMath::Lerp(DefaultVignette, VignetteStrength * Ratio, Params.Y);
		return FVector4(FOV, Vignette, Params.Z, 0.0f);
	}

	return FVector4(FOV, 0.0f, Params.Z, 0.0f);
}

void UTGOR_DollyCamera::Apply(UTGOR_CameraComponent* Camera, USpringArmComponent* SpringArm, const FVector4& Input)
{
	// Set FOV
	Camera->FieldOfView = Input.X;

	// Apply vignette effect
	Camera->PostProcessSettings.VignetteIntensity = Input.Y;

	// Offset camera for dolly effect
	const float ReferenceFOV = FMath::Lerp(DefaultFOV, Input.X, Input.Z);
	const float SquaredArmLength = SpringArm->TargetArmLength * SpringArm->TargetArmLength;
	const float Cameradistance = FMath::Sqrt(SquaredArmLength - SpringArm->TargetOffset.SizeSquared());
	auto Conv = [](float FOV)->float { return (2.0f * FMath::Tan(FOV / 180.0f * PI * 0.5f)); };
	const float Width = Cameradistance * Conv(DefaultFOV);
	SpringArm->SocketOffset.X = (Cameradistance - Width / Conv(ReferenceFOV)) * DollyStrength;
}
