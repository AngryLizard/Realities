// The Gateway of Realities: Planes of Existence.

#include "TGOR_SurfaceActivatorComponent.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"

const FName UTGOR_SurfaceActivatorComponent::SocketName(TEXT("SurfaceEndpoint"));

UTGOR_SurfaceActivatorComponent::UTGOR_SurfaceActivatorComponent()
	: Super()
{
}

FVector UTGOR_SurfaceActivatorComponent::WorldToTarget(const FVector& Location) const
{
	// Project onto target plane
	FTransform Transform = GetComponentTransform();
	Transform.SetScale3D(FVector::OneVector);
	const FVector LocalLocation = Transform.InverseTransformPosition(Location);
	const FVector2D Domain = FVector2D(LocalLocation) / FVector2D::Max(Size, FVector2D(KINDA_SMALL_NUMBER));
	return FVector(FVector2D::Max(FVector2D::Min(Domain, DomainMax), DomainMin), 0.0f);
}

FVector UTGOR_SurfaceActivatorComponent::TargetToWorld(const FVector& Local) const
{
	FTransform Transform = GetComponentTransform();
	Transform.SetScale3D(FVector::OneVector);
	return Transform.TransformPosition(GetRelativeTargetTransform(Local).GetLocation());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTransform UTGOR_SurfaceActivatorComponent::GetRelativeTargetTransform(const FVector& Local) const
{
	const FVector Location =
		FVector::ForwardVector * Local.X * Size.X +
		FVector::RightVector * Local.Y * Size.Y;
	const FQuat Rotation = OffsetRotation.Quaternion();
	return FTransform(Rotation, Location, FVector::OneVector);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_SurfaceActivatorComponent::HasAnySockets() const
{
	return true;
}

FTransform UTGOR_SurfaceActivatorComponent::GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace) const
{
	const FTransform RelativeTransform = GetRelativeTargetTransform(FVector::ZeroVector);
	FTransform SocketTransform = RelativeTransform * GetComponentTransform();
	SocketTransform.SetScale3D(FVector::OneVector);

	switch (TransformSpace)
	{
	case RTS_World:
	{
		return SocketTransform;
		break;
	}
	case RTS_Actor:
	{
		if (const AActor* Actor = GetOwner())
		{
			return SocketTransform.GetRelativeTransform(Actor->GetTransform());
		}
		break;
	}
	case RTS_Component:
	{
		return RelativeTransform;
	}
	}
	return RelativeTransform;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_SurfaceActivatorComponent::QuerySupportedSockets(TArray<FComponentSocketDescription>& OutSockets) const
{
	new (OutSockets) FComponentSocketDescription(SocketName, EComponentSocketType::Socket);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
void UTGOR_SurfaceActivatorComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UTGOR_SurfaceActivatorComponent, Size))
	{
		Size = Size.GetAbs();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UTGOR_SurfaceActivatorComponent, Radius))
	{
		Radius = Radius.GetAbs();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UTGOR_SurfaceActivatorComponent, DomainMin))
	{
		DomainMin.X = FMath::Min(DomainMin.X, DomainMax.X);
		DomainMin.Y = FMath::Min(DomainMin.Y, DomainMax.Y);
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UTGOR_SurfaceActivatorComponent, DomainMax))
	{
		DomainMax.X = FMath::Max(DomainMin.X, DomainMax.X);
		DomainMax.Y = FMath::Max(DomainMin.Y, DomainMax.Y);
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif	// WITH_EDITOR

FPrimitiveSceneProxy* UTGOR_SurfaceActivatorComponent::CreateSceneProxy()
{
	class FDrawSurfaceActivatorSceneProxy final : public FDrawActivatorSceneProxy
	{
	public:
		FDrawSurfaceActivatorSceneProxy(const UTGOR_SurfaceActivatorComponent* InComponent)
			: UTGOR_ActivatorComponent::FDrawActivatorSceneProxy(InComponent),
			Size(InComponent->Size),
			Radius(InComponent->Radius),
			DomainMin(InComponent->DomainMin),
			DomainMax(InComponent->DomainMax)
		{
		}

		void DrawTargetSpace(const FMatrix& Transform, const FLinearColor& DrawColor, FPrimitiveDrawInterface* PDI) const
		{
			const FVector ScaledX = Transform.GetScaledAxis(EAxis::X).GetSafeNormal() * Size.X;
			const FVector ScaledY = Transform.GetScaledAxis(EAxis::Y).GetSafeNormal() * Size.Y;

			DrawRect(Transform, PDI, FColor::Blue, FColor::Green, ScaledX, ScaledY, 1.f, 1.1f);

			PDI->DrawLine(
				Transform.GetOrigin() + ScaledX,
				Transform.GetOrigin() + ScaledY,
				FColor::Red, SDPG_World, 0.75f);

			PDI->DrawLine(
				Transform.GetOrigin() + ScaledX,
				Transform.GetOrigin() - ScaledY,
				FColor::Red, SDPG_World, 0.75f);
		}

	private:
		const FVector2D Size;
		const FVector2D Radius;
		const FVector2D DomainMin;
		const FVector2D DomainMax;
	};

	return new FDrawSurfaceActivatorSceneProxy(this);
}
