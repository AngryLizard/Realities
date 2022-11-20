// The Gateway of Realities: Planes of Existence.


#include "TGOR_AimTargetComponent.h"
#include "TargetSystem/Content/TGOR_Target.h"
#include "DimensionSystem/Content/TGOR_Spawner.h"


UTGOR_AimTargetComponent::UTGOR_AimTargetComponent()
	: Super()
{
	TargetRadius = 64.0f;
	CanBeTargeted = true;
}

void UTGOR_AimTargetComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	Targets = Dependencies.Spawner->GetMListFromType<UTGOR_Target>(SpawnTargets);
}

TMap<int32, UTGOR_SpawnModule*> UTGOR_AimTargetComponent::GetModuleType_Implementation() const
{
	TMap<int32, UTGOR_SpawnModule*> Modules;

	const int32 Num = Targets.Num();
	for (int32 Index = 0; Index < Num; Index++)
	{
		Modules.Emplace(Index, Targets[Index]);
	}
	return Modules;
}

FTransform UTGOR_AimTargetComponent::GetTargetTransform() const
{
	return GetComponentTransform();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FVector UTGOR_AimTargetComponent::ProjectRay(const FVector& RayOrigin, const FVector& RayDirection) const
{
	const FVector Location = GetComponentLocation();
	const FVector Projection = (Location - RayOrigin).ProjectOnToNormal(RayDirection);
	return RayOrigin + Projection;
}

float UTGOR_AimTargetComponent::ComputeRelativeDistance(const FVector& Location, const FVector& Center, float Threshold) const
{
	const float ScaledThreshold = TargetRadius * Threshold * GetComponentScale().GetMax();
	return (Location - Center).Size() / ScaledThreshold;
}

FVector UTGOR_AimTargetComponent::WorldToTarget(const FVector& Location) const
{
	const FTransform Transform = GetTargetTransform();
	return Transform.InverseTransformPosition(Location);
}

FVector UTGOR_AimTargetComponent::TargetToWorld(const FVector& Local) const
{
	const FTransform Transform = GetTargetTransform();
	return Transform.TransformPosition(Local);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FBoxSphereBounds UTGOR_AimTargetComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	return FBoxSphereBounds(FVector::ZeroVector, FVector(TargetRadius), TargetRadius).TransformBy(LocalToWorld);
}

#if WITH_EDITOR
void UTGOR_AimTargetComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	// We only want to modify the property that was changed at this point
	// things like propagation from CDO to instances don't work correctly if changing one property causes a different property to change
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UTGOR_AimTargetComponent, TargetRadius))
	{
		TargetRadius = FMath::Max(TargetRadius, 0.f);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif	// WITH_EDITOR

////////////////////////////////////////////////////////////////////////////////////////////////////

// Copy from SphereComponent
template <EShapeBodySetupHelper UpdateBodySetupAction, typename BodySetupType>
bool InvalidateOrUpdateSphereBodySetup(BodySetupType& ShapeBodySetup, bool bUseArchetypeBodySetup, float SphereRadius)
{
	check((bUseArchetypeBodySetup && UpdateBodySetupAction == EShapeBodySetupHelper::InvalidateSharingIfStale) || (!bUseArchetypeBodySetup && UpdateBodySetupAction == EShapeBodySetupHelper::UpdateBodySetup));
	check(ShapeBodySetup->AggGeom.SphereElems.Num() == 1);
	FKSphereElem* SphereElem = ShapeBodySetup->AggGeom.SphereElems.GetData();

	// check for mal formed values
	float Radius = SphereRadius;
	if (Radius < KINDA_SMALL_NUMBER)
	{
		Radius = 0.1f;
	}

	if (UpdateBodySetupAction == EShapeBodySetupHelper::UpdateBodySetup)
	{
		// now set the PhysX data values
		SphereElem->Center = FVector::ZeroVector;
		SphereElem->Radius = Radius;
	}
	else
	{
		if (SphereElem->Radius != Radius)
		{
			ShapeBodySetup = nullptr;
			bUseArchetypeBodySetup = false;
		}
	}

	return bUseArchetypeBodySetup;
}

void UTGOR_AimTargetComponent::UpdateBodySetup()
{
	if (PrepareSharedBodySetup<UTGOR_AimTargetComponent>())
	{
		bUseArchetypeBodySetup = InvalidateOrUpdateSphereBodySetup<EShapeBodySetupHelper::InvalidateSharingIfStale>(ShapeBodySetup, bUseArchetypeBodySetup, TargetRadius);
	}

	if (!IsValid(ShapeBodySetup))
	{
		ShapeBodySetup = NewObject<UBodySetup>(this, NAME_None, RF_Transient);
		if (GUObjectArray.IsDisregardForGC(this))
		{
			ShapeBodySetup->AddToRoot();
		}

		// If this component is in GC cluster, make sure we add the body setup to it to
		ShapeBodySetup->AddToCluster(this);
		// if we got created outside of game thread, but got added to a cluster, 
		// we no longer need the Async flag
		if (ShapeBodySetup->HasAnyInternalFlags(EInternalObjectFlags::Async) && GUObjectClusters.GetObjectCluster(ShapeBodySetup))
		{
			ShapeBodySetup->ClearInternalFlags(EInternalObjectFlags::Async);
		}

		ShapeBodySetup->CollisionTraceFlag = CTF_UseSimpleAsComplex;
		ShapeBodySetup->AggGeom.SphereElems.Add(FKSphereElem());
		ShapeBodySetup->bNeverNeedsCookedCollisionData = true;
		bUseArchetypeBodySetup = false;	//We're making our own body setup, so don't use the archetype's.

		//Update bodyinstance and shapes
		BodyInstance.BodySetup = ShapeBodySetup;
		{
			if (BodyInstance.IsValidBodyInstance())
			{
				FPhysicsCommand::ExecuteWrite(BodyInstance.GetActorReferenceWithWelding(), [this](const FPhysicsActorHandle& Actor)
				{
					TArray<FPhysicsShapeHandle> Shapes;
					BodyInstance.GetAllShapes_AssumesLocked(Shapes);

					for (FPhysicsShapeHandle& Shape : Shapes)	//The reason we iterate is we may have multiple scenes and thus multiple shapes, but they are all pointing to the same geometry
					{
						//Update shape with the new body setup. Make sure to only update shapes owned by this body instance
						if (BodyInstance.IsShapeBoundToBody(Shape))
						{
							FPhysicsInterface::SetUserData(Shape, (void*)ShapeBodySetup->AggGeom.SphereElems[0].GetUserData());
						}
					}
				});
			}
		}
	}

	if (!bUseArchetypeBodySetup)
	{
		InvalidateOrUpdateSphereBodySetup<EShapeBodySetupHelper::UpdateBodySetup>(ShapeBodySetup, bUseArchetypeBodySetup, TargetRadius);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FPrimitiveSceneProxy* UTGOR_AimTargetComponent::CreateSceneProxy()
{
	return new FDrawAimTargetSceneProxy(this);
}

bool UTGOR_AimTargetComponent::IsZeroExtent() const
{
	return TargetRadius == 0.f;
}

bool UTGOR_AimTargetComponent::AreSymmetricRotations(const FQuat& A, const FQuat& B, const FVector& Scale3D) const
{
	// All rotations are equal when scale is uniform.
	// Not detecting rotations around non-uniform scale.
	return Scale3D.GetAbs().AllComponentsEqual() || A.Equals(B);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SIZE_T UTGOR_AimTargetComponent::FDrawAimTargetSceneProxy::GetTypeHash() const
{
	static size_t UniquePointer;
	return reinterpret_cast<size_t>(&UniquePointer);
}

UTGOR_AimTargetComponent::FDrawAimTargetSceneProxy::FDrawAimTargetSceneProxy(const UTGOR_AimTargetComponent* InComponent)
	: FPrimitiveSceneProxy(InComponent)
	, bDrawOnlyIfSelected(InComponent->bDrawOnlyIfSelected)
	, ShapeColor(InComponent->ShapeColor)
	, TargetRadius(InComponent->CanBeTargeted ? InComponent->TargetRadius : 0.0f)
	, ActivationThreshold(InComponent->ActivationThreshold)
{
	bWillEverBeLit = false;
}

void UTGOR_AimTargetComponent::FDrawAimTargetSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_GetDynamicMeshElements_DrawDynamicElements);

	const FMatrix& Transform = GetLocalToWorld();
	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
	{
		if (VisibilityMap & (1 << ViewIndex))
		{
			const FSceneView* View = Views[ViewIndex];
			const FLinearColor DrawColor = GetViewSelectionColor(ShapeColor, *View, IsSelected(), IsHovered(), false, IsIndividuallySelected());

			FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);

			// Taking into account the min and maximum drawing distance
			const float DistanceSqr = (View->ViewMatrices.GetViewOrigin() - Transform.GetOrigin()).SizeSquared();
			if (DistanceSqr < FMath::Square(GetMinDrawDistance()) || DistanceSqr > FMath::Square(GetMaxDrawDistance()))
			{
				continue;
			}

			DrawTargetSpace(Transform, DrawColor, PDI);

			if (TargetRadius > SMALL_NUMBER)
			{
				const FVector ScaledX = Transform.GetScaledAxis(EAxis::X).GetSafeNormal();
				const FVector ScaledY = Transform.GetScaledAxis(EAxis::Y).GetSafeNormal();
				const FVector ScaledZ = Transform.GetScaledAxis(EAxis::Z).GetSafeNormal();

				const int32 SphereSides = FMath::Clamp<int32>(TargetRadius / 4.f, 16, 64);
				DrawCircle(PDI, Transform.GetOrigin(), ScaledX, ScaledY, DrawColor, TargetRadius, SphereSides, SDPG_World);
				DrawCircle(PDI, Transform.GetOrigin(), ScaledX, ScaledZ, DrawColor, TargetRadius, SphereSides, SDPG_World);
				DrawCircle(PDI, Transform.GetOrigin(), ScaledY, ScaledZ, DrawColor, TargetRadius, SphereSides, SDPG_World);

				if (IsSelected())
				{
					const FLinearColor TransparentColor = FLinearColor(DrawColor.R, DrawColor.G, DrawColor.B, DrawColor.A * 0.5f);
					DrawCircle(PDI, Transform.GetOrigin(), ScaledX, ScaledY, TransparentColor, TargetRadius * ActivationThreshold, SphereSides, SDPG_World);
					DrawCircle(PDI, Transform.GetOrigin(), ScaledX, ScaledZ, TransparentColor, TargetRadius * ActivationThreshold, SphereSides, SDPG_World);
					DrawCircle(PDI, Transform.GetOrigin(), ScaledY, ScaledZ, TransparentColor, TargetRadius * ActivationThreshold, SphereSides, SDPG_World);

					DrawArrowHead(PDI, Transform.GetOrigin() + ScaledX * SphereSides, Transform.GetOrigin() + ScaledX * SphereSides * 0.1f, TargetRadius, DrawColor, SDPG_World);
				}
			}
		}
	}
}

FPrimitiveViewRelevance UTGOR_AimTargetComponent::FDrawAimTargetSceneProxy::GetViewRelevance(const FSceneView* View) const
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
