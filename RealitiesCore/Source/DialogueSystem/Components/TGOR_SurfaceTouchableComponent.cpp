// The Gateway of Realities: Planes of Existence.

#include "TGOR_SurfaceTouchableComponent.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"

const FName UTGOR_SurfaceTouchableComponent::SocketName(TEXT("SurfaceEndpoint"));

UTGOR_SurfaceTouchableComponent::UTGOR_SurfaceTouchableComponent()
	: Super()
{
}

void UTGOR_SurfaceTouchableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Use domain target to move current domain position
	if (SimulationType == ETGOR_DomainSimulationType::Position)
	{
		if (ControlType == ETGOR_DomainControlType::Projection)
		{
			CurrentTargetDomainPosition = CurrentTargetDomainCursor * DomainScaling;
		}
		else
		{
			CurrentTargetDomainPosition += (CurrentTargetDomainCursor - LastTargetDomainCursor) * (DomainScaling * CurrentIntensity);
		}

		// No simulation
		CurrentTargetDomainVelocity = FVector2D::ZeroVector;
	}
	else
	{
		if (ControlType == ETGOR_DomainControlType::Projection)
		{
			CurrentTargetDomainVelocity += (CurrentTargetDomainCursor - LastTargetDomainCursor) * (Strength * CurrentIntensity * DeltaTime);
		}
		else
		{
			CurrentTargetDomainVelocity += (CurrentTargetDomainCursor - CurrentTargetDomainPosition) * (Strength * CurrentIntensity * DeltaTime);
		}

		// Simulate with velocity
		CurrentTargetDomainVelocity -= CurrentTargetDomainPosition * (SpringTowardsCenter * DeltaTime);
		CurrentTargetDomainVelocity -= CurrentTargetDomainVelocity * (2.f * SpringDampingRatio * FMath::Sqrt(FMath::Max(Strength, SpringTowardsCenter)) * DeltaTime);
		CurrentTargetDomainPosition += CurrentTargetDomainVelocity * DeltaTime;

	}
	LastTargetDomainCursor = CurrentTargetDomainCursor;

	// Restrict domain
	CurrentTargetDomainPosition = FVector2D::Max(CurrentTargetDomainPosition, DomainMin);
	CurrentTargetDomainPosition = FVector2D::Min(CurrentTargetDomainPosition, DomainMax);
}

bool UTGOR_SurfaceTouchableComponent::GetTargetProjection(const FVector& RayOrigin, const FVector& RayDirection, FVector& Projection, float& Distance) const
{
	if (Super::GetTargetProjection(RayOrigin, RayDirection, Projection, Distance))
	{
		const FVector2D Domain = ConvertToTargetDomain(Projection);
		Distance = FMath::Clamp(Domain.Size() - 1.0f, 0.0f, Distance);
		return true;
	}
	return false;
}

void UTGOR_SurfaceTouchableComponent::UpdateTarget(const FVector& RayOrigin, const FVector& RayDirection, float Intensity)
{
	Super::UpdateTarget(RayOrigin, RayDirection, Intensity);

	const FVector Projection = ProjectRay(RayOrigin, RayDirection);
	CurrentTargetDomainCursor = ConvertToTargetDomain(Projection);
}

FTransform UTGOR_SurfaceTouchableComponent::GetTargetTransform() const
{
	FTransform Transform = GetComponentTransform();
	Transform.SetScale3D(FVector::OneVector);
	return GetRelativeTargetTransform() * Transform;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FVector2D UTGOR_SurfaceTouchableComponent::ConvertToTargetDomain(const FVector& CursorProjection) const
{
	// Project onto target plane
	FTransform Transform = GetComponentTransform();
	Transform.SetScale3D(FVector::OneVector);
	const FVector LocalLocation = Transform.InverseTransformPosition(CursorProjection);
	return FVector2D(LocalLocation) / FVector2D::Max(Size, FVector2D(KINDA_SMALL_NUMBER));
}

FVector2D UTGOR_SurfaceTouchableComponent::GetDomainPosition() const
{
	return CurrentTargetDomainPosition;
}

FTransform UTGOR_SurfaceTouchableComponent::GetRelativeTargetTransform() const
{
	const FVector Location =
		FVector::ForwardVector * CurrentTargetDomainPosition.X * Size.X * Adherence.X +
		FVector::RightVector * CurrentTargetDomainPosition.Y * Size.Y * Adherence.Y;
	const FQuat Rotation = OffsetRotation.Quaternion();
	return FTransform(Rotation, Location, FVector::OneVector);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_SurfaceTouchableComponent::HasAnySockets() const
{
	return true;
}

FTransform UTGOR_SurfaceTouchableComponent::GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace) const
{
	const FTransform RelativeTransform = GetRelativeTargetTransform();
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

void UTGOR_SurfaceTouchableComponent::QuerySupportedSockets(TArray<FComponentSocketDescription>& OutSockets) const
{
	new (OutSockets) FComponentSocketDescription(SocketName, EComponentSocketType::Socket);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
void UTGOR_SurfaceTouchableComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UTGOR_SurfaceTouchableComponent, Size))
	{
		Size = Size.GetAbs();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UTGOR_SurfaceTouchableComponent, Radius))
	{
		Radius = Radius.GetAbs();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UTGOR_SurfaceTouchableComponent, DomainMin))
	{
		DomainMin.X = FMath::Min(DomainMin.X, DomainMax.X);
		DomainMin.Y = FMath::Min(DomainMin.Y, DomainMax.Y);
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UTGOR_SurfaceTouchableComponent, DomainMax))
	{
		DomainMax.X = FMath::Max(DomainMin.X, DomainMax.X);
		DomainMax.Y = FMath::Max(DomainMin.Y, DomainMax.Y);
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif	// WITH_EDITOR

FPrimitiveSceneProxy* UTGOR_SurfaceTouchableComponent::CreateSceneProxy()
{
	class FDrawSurfaceTouchableSceneProxy final : public FDrawTouchableSceneProxy
	{
	public:
		FDrawSurfaceTouchableSceneProxy(const UTGOR_SurfaceTouchableComponent* InComponent)
			: UTGOR_TouchableComponent::FDrawTouchableSceneProxy(InComponent),
			Size(InComponent->Size),
			Radius(InComponent->Radius),
			DomainMin(InComponent->DomainMin),
			DomainMax(InComponent->DomainMax),
			Adherence(InComponent->Adherence)
		{
		}

		void DrawTargetSpace(const FMatrix& Transform, const FLinearColor& DrawColor, FPrimitiveDrawInterface* PDI) const
		{
			const FVector ScaledX = Transform.GetScaledAxis(EAxis::X).GetSafeNormal() * Size.X;
			const FVector ScaledY = Transform.GetScaledAxis(EAxis::Y).GetSafeNormal() * Size.Y;

			DrawRect(Transform, PDI, FColor::Blue, FColor::Green, ScaledX, ScaledY, 1.f, 1.1f);
			DrawRect(Transform, PDI, FColor::White, FColor::White, ScaledX * Adherence.X, ScaledY * Adherence.Y, 0.5f, 1.1f);

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
		const FVector2D Adherence;
	};

	return new FDrawSurfaceTouchableSceneProxy(this);
}
