// The Gateway of Realities: Planes of Existence.

#include "TGOR_TouchableComponent.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"

UTGOR_TouchableComponent::UTGOR_TouchableComponent()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_TouchableComponent::GetTargetProjection(const FVector& RayOrigin, const FVector& RayDirection, FVector& Projection, float& Distance) const
{
	Projection = ProjectRay(RayOrigin, RayDirection);

	const FVector Location = GetComponentLocation();
	Distance = (Projection - Location).Size() / TargetRadius;
	return Distance <= 1.0f;
}


void UTGOR_TouchableComponent::UpdateTarget(const FVector& RayOrigin, const FVector& RayDirection, float Intensity)
{
	CurrentIntensity = Intensity;
}

FVector UTGOR_TouchableComponent::ProjectRay(const FVector& RayOrigin, const FVector& RayDirection) const
{
	const FVector Location = GetComponentLocation();
	const FVector Projection = (Location - RayOrigin).ProjectOnToNormal(RayDirection);
	return RayOrigin + Projection;
}

FTransform UTGOR_TouchableComponent::GetTargetTransform() const
{
	return GetComponentTransform();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

float UTGOR_TouchableComponent::GetTargetIntensity() const
{
	return CurrentIntensity;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_TouchableComponent::FDrawTouchableSceneProxy::FDrawTouchableSceneProxy(const UTGOR_TouchableComponent* InComponent)
	: UTGOR_AimTargetComponent::FDrawAimTargetSceneProxy(InComponent)
	, PreviewSize(InComponent->PreviewSize)
	, LocalX(InComponent->OffsetRotation.RotateVector(FVector::ForwardVector))
	, LocalY(InComponent->OffsetRotation.RotateVector(FVector::RightVector))
{
}

void UTGOR_TouchableComponent::FDrawTouchableSceneProxy::DrawTargetSpace(const FMatrix& Transform, const FLinearColor& DrawColor, FPrimitiveDrawInterface* PDI) const
{
	// Draw preview
	const FVector ScaledLocalX = Transform.TransformVector(LocalX).GetSafeNormal() * PreviewSize;
	const FVector ScaledLocalY = Transform.TransformVector(LocalY).GetSafeNormal() * PreviewSize;
	DrawRect(Transform, PDI, DrawColor, DrawColor, ScaledLocalX, ScaledLocalY, 0.75f, 0.9f);

	PDI->DrawLine(
		Transform.GetOrigin() + ScaledLocalX,
		Transform.GetOrigin() + ScaledLocalY,
		DrawColor, SDPG_World, 1.f);

	PDI->DrawLine(
		Transform.GetOrigin() + ScaledLocalX,
		Transform.GetOrigin() - ScaledLocalY,
		DrawColor, SDPG_World, 1.f);
}

void UTGOR_TouchableComponent::FDrawTouchableSceneProxy::DrawRect(const FMatrix& Transform, FPrimitiveDrawInterface* PDI, const FLinearColor& ColorX, const FLinearColor& ColorY, const FVector& ScaledX, const FVector& ScaledY, const  float thickness, float border) const
{
	PDI->DrawLine(
		Transform.GetOrigin() + ScaledX + ScaledY * border,
		Transform.GetOrigin() + ScaledX - ScaledY * border,
		ColorX, SDPG_World, thickness);

	PDI->DrawLine(
		Transform.GetOrigin() + ScaledX * border + ScaledY,
		Transform.GetOrigin() - ScaledX * border + ScaledY,
		ColorY, SDPG_World, thickness);

	PDI->DrawLine(
		Transform.GetOrigin() - ScaledX + ScaledY * border,
		Transform.GetOrigin() - ScaledX - ScaledY * border,
		ColorX, SDPG_World, thickness);

	PDI->DrawLine(
		Transform.GetOrigin() + ScaledX * border - ScaledY,
		Transform.GetOrigin() - ScaledX * border - ScaledY,
		ColorY, SDPG_World, thickness);
}

FPrimitiveSceneProxy* UTGOR_TouchableComponent::CreateSceneProxy()
{
	return new FDrawTouchableSceneProxy(this);
}

bool UTGOR_TouchableComponent::AreSymmetricRotations(const FQuat& A, const FQuat& B, const FVector& Scale3D) const
{
	return false;
}
