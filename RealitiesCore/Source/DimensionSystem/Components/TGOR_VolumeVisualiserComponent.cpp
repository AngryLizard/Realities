// The Gateway of Realities: Planes of Existence.

#include "TGOR_VolumeVisualiserComponent.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "DimensionSystem/Volumes/TGOR_PhysicsVolume.h"


// Sets default values for this component's properties
UTGOR_VolumeVisualiserComponent::UTGOR_VolumeVisualiserComponent()
	: Super(),
	SampleDistance(500.0f),
	SampleLength(50.0f)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FBoxSphereBounds UTGOR_VolumeVisualiserComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	ATGOR_PhysicsVolume* Volume = GetOuterATGOR_PhysicsVolume();
	return FBoxSphereBounds(Volume->ComputeBoundingBox());
}

#if WITH_EDITOR
void UTGOR_VolumeVisualiserComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	// We only want to modify the property that was changed at this point
	// things like propagation from CDO to instances don't work correctly if changing one property causes a different property to change
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UTGOR_VolumeVisualiserComponent, SampleDistance) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(UTGOR_VolumeVisualiserComponent, SampleLength) )
	{
		UpdateVisuals();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UTGOR_VolumeVisualiserComponent::UpdateVisuals()
{
	UpdateBounds();
	UpdateBodySetup();
	MarkRenderStateDirty();
}

#endif	// WITH_EDITOR

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_VolumeVisualiserComponent::UpdateBodySetup()
{
	// No collision needed
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FPrimitiveSceneProxy* UTGOR_VolumeVisualiserComponent::CreateSceneProxy()
{
	/** Represents a UTGOR_VolumeVisualiserComponent to the scene manager. */
	class FDrawHandleSceneProxy final : public FPrimitiveSceneProxy
	{
	public:
		SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}

		FDrawHandleSceneProxy(const UTGOR_VolumeVisualiserComponent* InComponent)
			: FPrimitiveSceneProxy(InComponent)
			, bDrawOnlyIfSelected(InComponent->bDrawOnlyIfSelected)
			, ShapeColor(InComponent->ShapeColor)
		{
			FEditorScriptExecutionGuard ScriptGuard;

			bWillEverBeLit = false;

			ATGOR_PhysicsVolume* Volume = InComponent->GetOuterATGOR_PhysicsVolume();
			const FTransform Transform = Volume->GetActorTransform();

			const FBox Box = Volume->ComputeBoundingBox();
			const FVector Extend = Box.GetExtent();
			const FVector Center = Box.GetCenter();

			VolumePoints.Reset();
			VolumeVectors.Reset();
			MaxVolumeVector = 0.0f;

			const float Distance = InComponent->SampleDistance;
			if (Distance > 1.0f)
			{
				const int32 SampleNum = Extend.X * Extend.Y * Extend.Z * 8 / (Distance * Distance * Distance);
				VolumePoints.Reserve(SampleNum);
				VolumeVectors.Reserve(SampleNum);

				FRandomStream RandomStream(10);
				for (int32 Index = 0; Index < SampleNum; Index++)
				{
					const FVector Vector = FVector(RandomStream.FRandRange(-1, 1), RandomStream.FRandRange(-1, 1), RandomStream.FRandRange(-1, 1));
					const FVector Location = Center + Vector * Extend;
					if (Volume->IsInside(Location))
					{
						VolumePoints.Emplace(Transform.InverseTransformPosition(Location));

						// TODO: Take parent (additive) into consideration
						FTGOR_PhysicsProperties Properties = Volume->ComputeSurroundings(Location);
						VolumeVectors.Emplace(Transform.InverseTransformVector(Properties.Gravity * InComponent->SampleLength));
						MaxVolumeVector = FMath::Max(MaxVolumeVector, Properties.Gravity.Size());
					}
				}
			}
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

					const int32 Num = VolumePoints.Num();
					for (int32 Index = 0; Index < Num; Index++)
					{
						const FVector Start = VolumePoints[Index];
						const FVector End = Start + VolumeVectors[Index] / MaxVolumeVector;
						const FVector Mid = (Start + Start + End) / 3;
						PDI->DrawLine(LocalToWorld.TransformPosition(Start), LocalToWorld.TransformPosition(Mid), DrawColor, SDPG_World, 3.0f);
						PDI->DrawLine(LocalToWorld.TransformPosition(Start), LocalToWorld.TransformPosition(End), DrawColor, SDPG_World, 2.0f);
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
		const FColor	ShapeColor;
		TArray<FVector> VolumePoints;
		TArray<FVector> VolumeVectors;
		float MaxVolumeVector;
	};

	return new FDrawHandleSceneProxy(this);
}


bool UTGOR_VolumeVisualiserComponent::IsZeroExtent() const
{
	return false;
}

bool UTGOR_VolumeVisualiserComponent::AreSymmetricRotations(const FQuat& A, const FQuat& B, const FVector& Scale3D) const
{
	return false;
}
