// The Gateway of Realities: Planes of Existence.


#include "TGOR_InteractableComponent.h"
#include "TargetSystem/Content/TGOR_Target.h"
#include "DimensionSystem/Content/TGOR_Spawner.h"
#include "CoreSystem/TGOR_Singleton.h"

#define LOCTEXT_NAMESPACE "TGOR" 

UTGOR_InteractableComponent::UTGOR_InteractableComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTGOR_InteractableComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	Targets = Dependencies.Spawner->GetMListFromType<UTGOR_Target>(SpawnTargets);
}

TMap<int32, UTGOR_SpawnModule*> UTGOR_InteractableComponent::GetModuleType_Implementation() const
{
	TMap<int32, UTGOR_SpawnModule*> Modules;

	const int32 Num = Targets.Num();
	for (int32 Index = 0; Index < Num; Index++)
	{
		Modules.Emplace(Index, Targets[Index]);
	}
	return Modules;
}

void UTGOR_InteractableComponent::Influence(const FTGOR_InfluenceInstance& Influence)
{
}

FPrimitiveSceneProxy* UTGOR_InteractableComponent::CreateSceneProxy()
{
	/** Represents an interactableComponent to the scene manager. */
	class FInteractableSceneProxy final : public FPrimitiveSceneProxy
	{
	public:
		SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}

		/** Initialization constructor. */
		FInteractableSceneProxy(const USphereComponent* InComponent)
			: FPrimitiveSceneProxy(InComponent)
			, bDrawOnlyIfSelected(InComponent->bDrawOnlyIfSelected)
			, SphereColor(InComponent->ShapeColor)
			, SphereRadius(InComponent->GetUnscaledSphereRadius())
		{
			bWillEverBeLit = false;
		}

		// FPrimitiveSceneProxy interface.

		virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_SphereSceneProxy_GetDynamicMeshElements);

			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{
				if (VisibilityMap & (1 << ViewIndex))
				{
					const FSceneView* View = Views[ViewIndex];
					FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);

					const FMatrix& LocalToWorld = GetLocalToWorld();
					const FLinearColor DrawSphereColor = GetViewSelectionColor(SphereColor, *View, IsSelected(), IsHovered(), false, IsIndividuallySelected());

					// Taking into account the min and maximum drawing distance
					const float DistanceSqr = (View->ViewMatrices.GetViewOrigin() - LocalToWorld.GetOrigin()).SizeSquared();
					if (DistanceSqr < FMath::Square(GetMinDrawDistance()) || DistanceSqr > FMath::Square(GetMaxDrawDistance()))
					{
						continue;
					}

					float AbsScaleX = LocalToWorld.GetScaledAxis(EAxis::X).Size();
					float AbsScaleY = LocalToWorld.GetScaledAxis(EAxis::Y).Size();
					float AbsScaleZ = LocalToWorld.GetScaledAxis(EAxis::Z).Size();
					float MinAbsScale = FMath::Min3(AbsScaleX, AbsScaleY, AbsScaleZ);

					FVector ScaledX = LocalToWorld.GetUnitAxis(EAxis::X) * MinAbsScale;
					FVector ScaledY = LocalToWorld.GetUnitAxis(EAxis::Y) * MinAbsScale;
					FVector ScaledZ = LocalToWorld.GetUnitAxis(EAxis::Z) * MinAbsScale;

					const int32 SphereSides = FMath::Clamp<int32>(SphereRadius / 4.f, 16, 64);
					DrawCircle(PDI, LocalToWorld.GetOrigin(), ScaledX, ScaledY, DrawSphereColor, SphereRadius, SphereSides, SDPG_World);
					DrawCircle(PDI, LocalToWorld.GetOrigin(), ScaledX, ScaledZ, DrawSphereColor, SphereRadius, SphereSides, SDPG_World);
					DrawCircle(PDI, LocalToWorld.GetOrigin(), ScaledY, ScaledZ, DrawSphereColor, SphereRadius, SphereSides, SDPG_World);

					DrawArrowHead(PDI, LocalToWorld.GetOrigin() + ScaledX * SphereRadius, LocalToWorld.GetOrigin(), SphereRadius, DrawSphereColor, SDPG_World);
				}
			}
		}

		virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
		{
			const bool bVisibleForSelection = !bDrawOnlyIfSelected || IsSelected();
			const bool bVisibleForShowFlags = true; // @TODO

			// Should we draw this because collision drawing is enabled, and we have collision
			const bool bShowForCollision = View->Family->EngineShowFlags.Collision && IsCollisionEnabled();

			FPrimitiveViewRelevance Result;
			Result.bDrawRelevance = (IsShown(View) && bVisibleForSelection && bVisibleForShowFlags) || bShowForCollision;
			Result.bDynamicRelevance = true;
			Result.bShadowRelevance = IsShadowCast(View);
			Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
			return Result;
		}

		virtual uint32 GetMemoryFootprint(void) const override { return(sizeof(*this) + GetAllocatedSize()); }
		uint32 GetAllocatedSize(void) const { return(FPrimitiveSceneProxy::GetAllocatedSize()); }

	private:
		const uint32				bDrawOnlyIfSelected : 1;
		const FColor				SphereColor;
		const float					SphereRadius;
	};

	return new FInteractableSceneProxy(this);
}

#undef LOCTEXT_NAMESPACE