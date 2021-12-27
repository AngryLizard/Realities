// The Gateway of Realities: Planes of Existence.

#include "TGOR_EllipsoidComponent.h"

#include "CustomisationSystem/Components/TGOR_ControlSkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

UTGOR_EllipsoidComponent::UTGOR_EllipsoidComponent()
:	Super()
{
}

FName UTGOR_EllipsoidComponent::GetControlName() const
{
	return GetFName();
}

FTransform UTGOR_EllipsoidComponent::GetControlTransform(USkinnedMeshComponent* Component) const
{
	return UTGOR_ControlSkeletalMeshComponent::GetRelativeControlTransform(Component, this);
}

void UTGOR_EllipsoidComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	ITGOR_SpawnerInterface::UpdateContent_Implementation(Dependencies);

	Ellipsoid = Dependencies.Spawner->GetMFromType<UTGOR_Ellipsoid>(SpawnEllipsoid);
}

TMap<int32, UTGOR_SpawnModule*> UTGOR_EllipsoidComponent::GetModuleType_Implementation() const
{
	return MakeModuleList(Ellipsoid);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_EllipsoidComponent::AdaptToGroundContact(const FVector& SurfaceLocation, const FVector& SurfaceNormal, float Stretch)
{
	const float Flat = 0.5f;

	FTransform Transform;
	Transform.SetLocation(SurfaceLocation - SurfaceNormal * (Flat * ELLIPSOID_RADIUS));
	Transform.SetRotation(FQuat::FindBetweenNormals(FVector::UpVector, SurfaceNormal));
	Transform.SetScale3D(FVector(Stretch, Stretch, Flat));
	SetWorldTransform(Transform);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FBoxSphereBounds UTGOR_EllipsoidComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	return FBoxSphereBounds(LocalToWorld.GetLocation(), LocalToWorld.GetScale3D() * ELLIPSOID_RADIUS, LocalToWorld.GetScale3D().GetMax() * ELLIPSOID_RADIUS);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_EllipsoidComponent::UpdateBodySetup()
{
	// No collision needed
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FPrimitiveSceneProxy* UTGOR_EllipsoidComponent::CreateSceneProxy()
{
	/** Represents a UCapsuleComponent to the scene manager. */
	class FDrawHandleSceneProxy final : public FPrimitiveSceneProxy
	{
	public:
		SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}

		FDrawHandleSceneProxy(const UTGOR_EllipsoidComponent* InComponent)
			: FPrimitiveSceneProxy(InComponent)
			, bDrawOnlyIfSelected(InComponent->bDrawOnlyIfSelected)
			, Intensity(InComponent->Intensity)
			, ShapeColor(InComponent->ShapeColor)
		{
			bWillEverBeLit = false;
		}

		virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_GetDynamicMeshElements_DrawDynamicElements);

			const FMatrix& LocalToWorld = GetLocalToWorld();
			const int32 Sides = 5;

			const FVector ScaledX = LocalToWorld.GetScaledAxis(EAxis::X) * ELLIPSOID_RADIUS;
			const FVector ScaledY = LocalToWorld.GetScaledAxis(EAxis::Y) * ELLIPSOID_RADIUS;
			const FVector ScaledZ = LocalToWorld.GetScaledAxis(EAxis::Z) * ELLIPSOID_RADIUS;

			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{
				if (VisibilityMap & (1 << ViewIndex))
				{
					const FSceneView* View = Views[ViewIndex];

					const FLinearColor DrawCapsuleColor = GetViewSelectionColor(ShapeColor, *View, IsSelected(), IsHovered(), false, IsIndividuallySelected());

					FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);
					
					for (int32 Side = 0; Side <= Sides; Side++)
					{
						const float V = 1.0f - (2.0f * Side) / Sides;
						const float S = FMath::Sqrt(1.0f - V * V);
						
						DrawCircle(PDI, LocalToWorld.GetOrigin() + ScaledZ * V, ScaledX * S, ScaledY * S, ShapeColor, 1.0f, 32, SDPG_World, 0.25f * Intensity);
						DrawCircle(PDI, LocalToWorld.GetOrigin() + ScaledX * V, ScaledY * S, ScaledZ * S, ShapeColor, 1.0f, 32, SDPG_World, 0.25f * Intensity);
						DrawCircle(PDI, LocalToWorld.GetOrigin() + ScaledY * V, ScaledZ * S, ScaledX * S, ShapeColor, 1.0f, 32, SDPG_World, 0.25f * Intensity);
					}
				}
			}
		}

		virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
		{
			const bool bProxyVisible = !bDrawOnlyIfSelected || IsSelected();

			// Should we draw this because collision drawing is enabled, and we have collision
			const bool bShowForCollision = View->Family->EngineShowFlags.Collision && IsCollisionEnabled();

			FPrimitiveViewRelevance Result;
			Result.bDrawRelevance = (IsShown(View) && bProxyVisible) || bShowForCollision;
			Result.bDynamicRelevance = true;
			Result.bShadowRelevance = IsShadowCast(View);
			Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
			return Result;
		}
		virtual uint32 GetMemoryFootprint(void) const override { return(sizeof(*this) + GetAllocatedSize()); }
		uint32 GetAllocatedSize(void) const { return(FPrimitiveSceneProxy::GetAllocatedSize()); }

	private:
		const uint32	bDrawOnlyIfSelected : 1;
		const float		Intensity;
		const FColor	ShapeColor;
	};

	return new FDrawHandleSceneProxy(this);
}


bool UTGOR_EllipsoidComponent::IsZeroExtent() const
{
	return false;
}

bool UTGOR_EllipsoidComponent::AreSymmetricRotations(const FQuat& A, const FQuat& B, const FVector& Scale3D) const
{
	return false;
	/*
	if (Scale3D.X != Scale3D.Y)
	{
		return false;
	}

	const FVector AUp = A.GetAxisZ();
	const FVector BUp = B.GetAxisZ();
	return AUp.Equals(BUp);
	*/
}
