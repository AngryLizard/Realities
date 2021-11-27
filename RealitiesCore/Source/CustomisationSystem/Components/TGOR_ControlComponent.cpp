// The Gateway of Realities: Planes of Existence.

#include "TGOR_ControlComponent.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "CustomisationSystem/Components/TGOR_ControlSkeletalMeshComponent.h"

#include "Net/UnrealNetwork.h"
#include "SceneManagement.h"
#include "Engine/Engine.h"


UTGOR_ControlComponent::UTGOR_ControlComponent()
:	Super(),
	ControlType(ETGOR_ControlType::Pivot)
{
}

FName UTGOR_ControlComponent::GetControlName() const
{
	return GetFName();
}

FTransform UTGOR_ControlComponent::GetControlTransform(USkinnedMeshComponent* Component) const
{
	return UTGOR_ControlSkeletalMeshComponent::GetRelativeControlTransform(Component, this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FBoxSphereBounds UTGOR_ControlComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	return FBoxSphereBounds(LocalToWorld.GetLocation(), LocalToWorld.GetScale3D() * CONTROL_RADIUS, LocalToWorld.GetScale3D().GetMax() * CONTROL_RADIUS);
}

#if WITH_EDITOR
void UTGOR_ControlComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UTGOR_ControlComponent::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	const FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	Super::PostEditChangeChainProperty(PropertyChangedEvent);
}

void UTGOR_ControlComponent::PostEditComponentMove(bool bFinished)
{
	Super::PostEditComponentMove(bFinished);
}

#endif	// WITH_EDITOR

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ControlComponent::UpdateBodySetup()
{
	// No collision needed ever
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FPrimitiveSceneProxy* UTGOR_ControlComponent::CreateSceneProxy()
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

		FDrawCylinderSceneProxy(const UTGOR_ControlComponent* InComponent)
			: FPrimitiveSceneProxy(InComponent)
			, bDrawOnlyIfSelected(InComponent->bDrawOnlyIfSelected)
			, Type(InComponent->ControlType)
			, ShapeColor(InComponent->ShapeColor)
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

					const FVector ScaledX = LocalToWorld.GetScaledAxis(EAxis::X) * CONTROL_RADIUS;
					const FVector ScaledY = LocalToWorld.GetScaledAxis(EAxis::Y) * CONTROL_RADIUS;
					const FVector ScaledZ = LocalToWorld.GetScaledAxis(EAxis::Z) * CONTROL_RADIUS;

					if (Type == ETGOR_ControlType::Pivot)
					{
						DrawCircle(PDI, LocalToWorld.GetOrigin(), ScaledX, ScaledY, ShapeColor, 1.0f, 4, SDPG_World, 1.0f);
						DrawCircle(PDI, LocalToWorld.GetOrigin(), ScaledX, ScaledZ, ShapeColor, 1.0f, 4, SDPG_World, 1.0f);
						DrawCircle(PDI, LocalToWorld.GetOrigin(), ScaledY, ScaledZ, ShapeColor, 1.0f, 4, SDPG_World, 1.0f);
						PDI->DrawLine(LocalToWorld.GetOrigin() - ScaledX, LocalToWorld.GetOrigin() + ScaledX, ShapeColor, SDPG_World, 0.5f);
						PDI->DrawLine(LocalToWorld.GetOrigin() - ScaledY, LocalToWorld.GetOrigin() + ScaledY, ShapeColor, SDPG_World, 0.5f);
						PDI->DrawLine(LocalToWorld.GetOrigin() - ScaledZ, LocalToWorld.GetOrigin() + ScaledZ, ShapeColor, SDPG_World, 0.5f);
					}
					else if (Type == ETGOR_ControlType::Vector)
					{
						DrawCircle(PDI, LocalToWorld.GetOrigin(), ScaledY/2, ScaledZ/2, ShapeColor, 1.0f, 4, SDPG_World, 0.5f);
						PDI->DrawLine(LocalToWorld.GetOrigin(), LocalToWorld.GetOrigin() + ScaledX, ShapeColor, SDPG_World, 1.0f);
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
		const ETGOR_ControlType Type;
		const FColor	ShapeColor;
	};

	return new FDrawCylinderSceneProxy(this);
}



bool UTGOR_ControlComponent::IsZeroExtent() const
{
	return false;
}

bool UTGOR_ControlComponent::AreSymmetricRotations(const FQuat& A, const FQuat& B, const FVector& Scale3D) const
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
