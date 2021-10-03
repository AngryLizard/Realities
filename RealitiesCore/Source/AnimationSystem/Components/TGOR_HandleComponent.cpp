// The Gateway of Realities: Planes of Existence.

#include "TGOR_HandleComponent.h"

#include "TGOR_AttachComponent.h"
#include "TGOR_AnimationComponent.h"
#include "PhysicsSystem/Components/TGOR_RigidComponent.h"
#include "CustomisationSystem/Components/TGOR_ControlSkeletalMeshComponent.h"
#include "DimensionSystem/Tasks/TGOR_LinearPilotTask.h"


UTGOR_HandleComponent::UTGOR_HandleComponent()
:	Super(), MovementCone(nullptr)
{
}

FName UTGOR_HandleComponent::GetControlName() const
{
	return GetFName();
}

FTransform UTGOR_HandleComponent::GetControlTransform(UTGOR_ControlSkeletalMeshComponent* Component) const
{
	return Component->GetRelativeControlTransform(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_HandleComponent::TraceHandle(UTGOR_PilotComponent* Component, const FTGOR_MovementSpace& Space, float Multiplier, FTGOR_HandleTraceOutput& Output)
{
	UTGOR_LinearPilotTask* Task = GetPOfType<UTGOR_LinearPilotTask>();
	if (IsValid(MovementCone) && IsValid(Task))
	{
		const float Radius = GetScaledCapsuleHalfHeight();

		check(MovementCone->GetAttachParent() && "Root cone not supported for TraceHandle");
		const FTransform ConeRelative = MovementCone->GetAttachParent()->GetComponentTransform() * Component->GetComponentTransform().Inverse();

		// Component is not guaranteed to be parent!
		const FVector Location = Space.Linear + Space.Angular * ConeRelative.TransformPosition(MovementCone->GetAlignmentLocation());

		Output.Direction = Space.Angular * ConeRelative.TransformVectorNoScale(MovementCone->GetAlignmentDirection());
		const FVector Translation = Output.Direction * (MovementCone->LimitRadius * Multiplier);

		FTGOR_MovementParent Parent;

		FHitResult Hit;
		if (Component->MovementSphereTraceSweep(Radius, Location, Translation, Hit))
		{
			Output.Linear = Hit.ImpactPoint;
			Output.Angular = FQuat::FindBetweenNormals(FVector::UpVector, Hit.Normal);

			Parent = Component->FindReparentToComponent(Hit.GetComponent(), Hit.BoneName);
		}
		else
		{
			Output.Linear = Location + Translation;
			Output.Angular = FQuat::FindBetweenNormals(FVector::UpVector, -Output.Direction); // < Could utilise Alignment here somehow, but wrong axis

			Parent.Mobility = Component;
			Parent.Index = INDEX_NONE;
		}

		Output.Delta = Output.Linear - Location;
		const FVector RadialVelocity = Space.AngularVelocity ^ Output.Delta;

		//TODO: If we don't care about dynamics we can use this instead and save some computation:
		//Task->SimulatePosition(Target);

		// Angular dynamics
		Output.AngularVelocity = Space.AngularVelocity;
		Output.AngularAcceleration = Space.AngularAcceleration;

		// Linear dynamics
		Output.LinearVelocity = Space.LinearVelocity + RadialVelocity;
		Output.LinearAcceleration = Space.LinearAcceleration + (RadialVelocity ^ Space.AngularVelocity);

		// Reparent
		Task->Parent(Parent.Mobility, Parent.Index);
		Task->SimulateDynamic(Output);

		Flatness = 1.0f;
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FPrimitiveSceneProxy* UTGOR_HandleComponent::CreateSceneProxy()
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
