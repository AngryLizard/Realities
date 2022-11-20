// The Gateway of Realities: Planes of Existence.

#include "TGOR_AreaComponent.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

UTGOR_AreaComponent::UTGOR_AreaComponent()
	: Super()
{
}

bool UTGOR_AreaComponent::IsLocationInside(const FVector& Location) const
{
	const FVector Local = GetComponentTransform().InverseTransformPosition(Location);
	return Local.Size2D() < Radius && FMath::Abs(Local.Z) < Radius;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FBoxSphereBounds UTGOR_AreaComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	return FBoxSphereBounds(LocalToWorld.GetLocation(), LocalToWorld.GetScale3D() * Radius, LocalToWorld.GetScale3D().GetMax() * Radius);
}

#if WITH_EDITOR
void UTGOR_AreaComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	// We only want to modify the property that was changed at this point
	// things like propagation from CDO to instances don't work correctly if changing one property causes a different property to change
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UTGOR_AreaComponent, Radius))
	{
		Radius = FMath::Max(Radius, 0.f);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif	// WITH_EDITOR

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_AreaComponent::UpdateBodySetup()
{
	// No collision needed ever
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FPrimitiveSceneProxy* UTGOR_AreaComponent::CreateSceneProxy()
{
	/** Represents a UCapsuleComponent to the scene manager. */
	class FDrawCylinderSceneProxy final : public FPrimitiveSceneProxy
	{
	public:
		SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}

		FDrawCylinderSceneProxy(const UTGOR_AreaComponent* InComponent)
			: FPrimitiveSceneProxy(InComponent)
			, bDrawOnlyIfSelected(InComponent->bDrawOnlyIfSelected)
			, ShapeColor(InComponent->ShapeColor)
			, Radius(InComponent->Radius)
		{
			bWillEverBeLit = false;
		}

		virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_GetDynamicMeshElements_DrawDynamicElements);

			const FMatrix& LocalToWorld = GetLocalToWorld();
			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{
				if (VisibilityMap & (1 << ViewIndex))
				{
					const FSceneView* View = Views[ViewIndex];
					const FLinearColor DrawColor = GetViewSelectionColor(ShapeColor, *View, IsSelected(), IsHovered(), false, IsIndividuallySelected());

					FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);

					const FVector ScaledX = LocalToWorld.GetScaledAxis(EAxis::X) * Radius;
					const FVector ScaledY = LocalToWorld.GetScaledAxis(EAxis::Y) * Radius;
					const FVector ScaledZ = LocalToWorld.GetScaledAxis(EAxis::Z) * Radius;

					DrawCircle(PDI, LocalToWorld.GetOrigin(), ScaledX, ScaledY, ShapeColor, 1.0f, 4, SDPG_World);
					PDI->DrawLine(LocalToWorld.GetOrigin(), LocalToWorld.GetOrigin() + ScaledZ, ShapeColor, SDPG_World);
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
		const FColor	ShapeColor;
		const float		Radius;
	};

	return new FDrawCylinderSceneProxy(this);
}

bool UTGOR_AreaComponent::IsZeroExtent() const
{
	return false;
}

bool UTGOR_AreaComponent::AreSymmetricRotations(const FQuat& A, const FQuat& B, const FVector& Scale3D) const
{
	return false;
	if (Scale3D.X != Scale3D.Y)
	{
		return false;
	}

	const FVector AUp = A.GetAxisZ();
	const FVector BUp = B.GetAxisZ();
	return AUp.Equals(BUp);
}
