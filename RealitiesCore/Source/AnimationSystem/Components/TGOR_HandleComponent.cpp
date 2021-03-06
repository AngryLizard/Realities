// The Gateway of Realities: Planes of Existence.

#include "TGOR_HandleComponent.h"

#include "TGOR_AttachComponent.h"
#include "TGOR_AnimationComponent.h"
#include "PhysicsSystem/Components/TGOR_RigidPawnComponent.h"
#include "CustomisationSystem/Components/TGOR_ControlSkeletalMeshComponent.h"
#include "DimensionSystem/Tasks/TGOR_LinearPilotTask.h"
#include "DimensionSystem/Content/TGOR_Pilot.h"

#include "UObject/ConstructorHelpers.h"

UTGOR_HandleComponent::UTGOR_HandleComponent()
:	Super(), MovementCone(nullptr)
{
	SpawnPilots.Emplace(ConstructorHelpers::FClassFinder<UTGOR_Pilot>(TEXT("/RealitiesCore/Content/Core/Pilots/PLT_Attached.PLT_Attached_C")).Class);
}

FName UTGOR_HandleComponent::GetControlName() const
{
	return GetFName();
}

FTransform UTGOR_HandleComponent::GetControlTransform(USkinnedMeshComponent* Component) const
{
	return UTGOR_ControlSkeletalMeshComponent::GetRelativeControlTransform(Component, this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_HandleComponent::GetGroundContact(FVector& SurfaceLocation, FVector& SurfaceNormal) const
{
	const FTGOR_MovementPosition Position = ComputePosition();
	SurfaceLocation = Position.Linear;
	SurfaceNormal = Position.Angular.GetAxisZ();
}

/*
FPrimitiveSceneProxy* UTGOR_HandleComponent::CreateSceneProxy()
{
	class FDrawHandleSceneProxy final : public FPrimitiveSceneProxy
	{
	public:
		SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}

		FDrawHandleSceneProxy(const UTGOR_HandleComponent* InComponent)
			: FPrimitiveSceneProxy(InComponent)
			, bDrawOnlyIfSelected(InComponent->bDrawOnlyIfSelected)
			, CapsuleRadius(InComponent->CapsuleRadius)
			, CapsuleHalfHeight(InComponent->CapsuleHalfHeight)
			, Flatness(InComponent->Flatness)
			, ShapeColor(InComponent->ShapeColor)
		{
			bWillEverBeLit = false;
		}

		// Copy from DrawWireChoppedCone with linethickness
		void DrawThickWireChoppedCone(FPrimitiveDrawInterface* PDI, const FVector& Base, const FVector& X, const FVector& Y, const FVector& Z, const FLinearColor& Color, float Radius, float TopRadius, float HalfHeight, int32 NumSides, uint8 DepthPriority) const
		{
			const float	AngleDelta = 2.0f * PI / NumSides;
			FVector	LastVertex = Base + X * Radius;
			FVector LastTopVertex = Base + X * TopRadius;

			for (int32 SideIndex = 0; SideIndex < NumSides; SideIndex++)
			{
				const FVector Vertex = Base + (X * FMath::Cos(AngleDelta * (SideIndex + 1)) + Y * FMath::Sin(AngleDelta * (SideIndex + 1))) * Radius;
				const FVector TopVertex = Base + (X * FMath::Cos(AngleDelta * (SideIndex + 1)) + Y * FMath::Sin(AngleDelta * (SideIndex + 1))) * TopRadius;

				PDI->DrawLine(LastVertex - Z * HalfHeight, Vertex - Z * HalfHeight, Color, DepthPriority, 0.75f);
				PDI->DrawLine(LastTopVertex + Z * HalfHeight, TopVertex + Z * HalfHeight, Color, DepthPriority, 0.75f);
				PDI->DrawLine(LastVertex - Z * HalfHeight, LastTopVertex + Z * HalfHeight, Color, DepthPriority, 0.75f);

				LastVertex = Vertex;
				LastTopVertex = TopVertex;
			}
		}

		virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_GetDynamicMeshElements_DrawDynamicElements);


			const FMatrix& LocalToWorld = GetLocalToWorld();
			const int32 CapsuleSides = FMath::Clamp<int32>(CapsuleRadius / 4.f, 16, 64);

			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{
				if (VisibilityMap & (1 << ViewIndex))
				{
					const FSceneView* View = Views[ViewIndex];
					const FLinearColor DrawCapsuleColor = GetViewSelectionColor(ShapeColor, *View, IsSelected(), IsHovered(), false, IsIndividuallySelected());

					FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);
					DrawWireCapsule(PDI, LocalToWorld.GetOrigin(), LocalToWorld.GetScaledAxis(EAxis::X), LocalToWorld.GetScaledAxis(EAxis::Y), LocalToWorld.GetScaledAxis(EAxis::Z), DrawCapsuleColor, CapsuleRadius, CapsuleHalfHeight, CapsuleSides, SDPG_World);

					const float BotRadius = CapsuleRadius * (1.0f + Flatness);
					const float TopRadius = CapsuleRadius * (1.0f - Flatness);
					const float HalfHeight = CapsuleHalfHeight * (1.0f - Flatness);
					DrawThickWireChoppedCone(PDI, LocalToWorld.GetOrigin(), LocalToWorld.GetScaledAxis(EAxis::X), LocalToWorld.GetScaledAxis(EAxis::Y), LocalToWorld.GetScaledAxis(EAxis::Z), DrawCapsuleColor, BotRadius, TopRadius, HalfHeight, CapsuleSides * 2, SDPG_World);
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
		const float		CapsuleRadius;
		const float		CapsuleHalfHeight;
		const float		Flatness;
		const FColor	ShapeColor;
	};

	return new FDrawHandleSceneProxy(this);
}
*/