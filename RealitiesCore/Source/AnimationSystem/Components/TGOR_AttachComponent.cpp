// The Gateway of Realities: Planes of Existence.

#include "TGOR_AttachComponent.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "DimensionSystem/Content/TGOR_Spawner.h"

#include "CustomisationSystem/Components/TGOR_ControlSkeletalMeshComponent.h"
#include "DimensionSystem/Content/TGOR_Primitive.h"

#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "TGOR_HandleComponent.h"

#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"
#include "SceneManagement.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"


FTGOR_MovementDynamic FTGOR_ConeTraceOutput::GetDynamicFromTrace(const FTGOR_MovementSpace& Space) const
{
	FTGOR_MovementDynamic Dynamic;
	Dynamic.Linear = Location;
	Dynamic.Angular = FQuat::FindBetweenNormals(FVector::UpVector, Normal);

	const FVector RadialVelocity = Space.AngularVelocity ^ Delta;

	// Angular dynamics
	Dynamic.AngularVelocity = Space.AngularVelocity;
	Dynamic.AngularAcceleration = Space.AngularAcceleration;

	// Linear dynamics
	Dynamic.LinearVelocity = Space.LinearVelocity + RadialVelocity;
	Dynamic.LinearAcceleration = Space.LinearAcceleration + (RadialVelocity ^ Space.AngularVelocity);

	return Dynamic;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_AttachComponent::UTGOR_AttachComponent()
:	Super()
{
	LimitRadius = 50.0f;
	LimitAngleX = 45.0f;
	LimitAngleY = 45.0f;
}

void UTGOR_AttachComponent::OnChildAttached(USceneComponent* ChildComponent)
{
	Super::OnChildAttached(ChildComponent);

	UTGOR_HandleComponent* Handle = Cast<UTGOR_HandleComponent>(ChildComponent);
	if (IsValid(Handle))
	{
		Handle->MovementCone = this;
	}
}

void UTGOR_AttachComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	ITGOR_SpawnerInterface::UpdateContent_Implementation(Dependencies);

	AttachCone = Dependencies.Spawner->GetMFromType<UTGOR_AttachCone>(SpawnAttachCone);
}

TMap<int32, UTGOR_SpawnModule*> UTGOR_AttachComponent::GetModuleType_Implementation() const
{
	return MakeModuleList(AttachCone);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_AttachComponent::TraceCenter(UTGOR_PilotComponent* Component, const FTGOR_MovementSpace& Space, double TraceRadius, double LengthMultiplier, FTGOR_ConeTraceOutput& Output) const
{
	check(GetAttachParent() && "Root cone not supported for TraceHandle");
	const FTransform ConeRelative = GetAttachParent()->GetComponentTransform() * Component->GetComponentTransform().Inverse();

	// Component is not guaranteed to be parent!
	const FVector Location = Space.Linear + Space.Angular * ConeRelative.TransformPosition(GetAlignmentLocation());

	Output.Direction = Space.Angular * ConeRelative.TransformVectorNoScale(GetAlignmentDirection());
	const FVector Translation = Output.Direction * (LimitRadius * LengthMultiplier);

	FHitResult Hit;
	if (Component->MovementSphereTraceSweep(TraceRadius, Location, Translation, Hit))
	{
		Output.Location = Hit.ImpactPoint;
		Output.Normal = Hit.Normal;
		Output.Delta = Output.Location - Location;

		Output.Parent = Component->FindReparentToComponent(Hit.GetComponent(), Hit.BoneName);
		return true;
	}
	else
	{
		Output.Location = Location + Translation;
		Output.Normal = -Output.Direction; // < Could utilise Alignment here somehow, but wrong axis
		Output.Delta = Output.Location - Location;

		Output.Parent.Mobility = Component;
		Output.Parent.Index = INDEX_NONE;
		return false;
	}
}

bool UTGOR_AttachComponent::TraceMoving(UTGOR_PilotComponent* Component, const FTGOR_MovementSpace& Space, double MaxSpeed, double LerpMultiplier, double TraceRadius, double LengthMultiplier, FTGOR_ConeTraceOutput& Output) const
{
	MaxSpeed = FMath::Max(MaxSpeed, 1.0);
	if (TraceCenter(Component, Space, TraceRadius, LengthMultiplier, Output))
	{
		const FVector Location = Output.Location - Output.Delta;
		const FVector Direction = FVector::VectorPlaneProject(Space.RelativeLinearVelocity, Output.Direction);

		const FTransform Transform = GetComponentTransform();
		const double AngleLimit = GetProjectedLimit(Transform.InverseTransformVector(Direction));

		const double TraceLength = LimitRadius * LengthMultiplier;
		const FVector MoveOffset = Space.RelativeLinearVelocity * (FMath::Sin(AngleLimit) * TraceLength * LerpMultiplier / MaxSpeed);
		const FVector Translation = Output.Location + Output.Direction * TraceLength + MoveOffset - Location;

		FHitResult Hit;
		if (Component->MovementSphereTraceSweep(TraceRadius, Location, Translation, Hit))
		{
			const FVector ProjectionVector = Hit.ImpactPoint - Output.Location;
			const double ProjectionNorm = ProjectionVector.SizeSquared();
			if (!FMath::IsNearlyZero(ProjectionNorm, 0.1))
			{
				const FVector Normal = FVector::VectorPlaneProject(Output.Normal, ProjectionVector / ProjectionNorm);
				const double Alpha = Space.RelativeLinearVelocity.Size() * LerpMultiplier / MaxSpeed;
				Output.Normal = (Output.Normal + Normal * Alpha).GetSafeNormal();

			}
		}
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_AttachComponent::IsInside(const FVector& Point) const
{
	float Radius = Point.Size();
	if (Radius > LimitRadius)
	{
		return false;
	}

	if (FMath::IsNearlyZero(Radius))
	{
		return true;
	}

	// Make sure we're inside the angle limit
	const FVector2D Flat = FVector2D(Point.Y, Point.Z);
	const double Spread = Flat.Size();
	if (!FMath::IsNearlyZero(Spread))
	{
		const FVector2D Direction = Flat / Spread;
		const double Angle = FMath::RadiansToDegrees(FMath::Acos(Point.X / Radius));
		const double LimitAngle = FMath::Square(Direction.X) * LimitAngleX + FMath::Square(Direction.Y) * LimitAngleY;
		return Angle < LimitAngle;
	}
	return false;
}

double UTGOR_AttachComponent::GetProjectedLimit(const FVector& Vector) const
{
	// Make sure we're inside the angle limit
	const FVector2D Flat = FVector2D(Vector.Y, Vector.Z);
	const double Spread = Flat.Size();
	if (!FMath::IsNearlyZero(Spread))
	{
		const FVector2D Direction = Flat / Spread;
		return FMath::Square(Direction.X) * LimitAngleX + FMath::Square(Direction.Y) * LimitAngleY;
	}
	return 0.0;
}

FVector UTGOR_AttachComponent::ProjectInside(const FVector& Point) const
{
	FVector Project = Point;

	double Radius = Point.Size();
	if (!FMath::IsNearlyZero(Radius))
	{
		const FVector Normal = Point / Radius;

		// Make sure we're inside the radius limit
		Radius = FMath::Min(Radius, LimitRadius);

		// Make sure we're inside the angle limit
		const FVector2D Flat = FVector2D(Normal.Y, Normal.Z);
		const double Spread = Flat.Size();
		if (!FMath::IsNearlyZero(Spread))
		{
			const FVector2D Direction = Flat / Spread;
			const double Angle = FMath::RadiansToDegrees(FMath::Acos(Normal.X));
			const double LimitAngle = FMath::Square(Direction.X) * LimitAngleX + FMath::Square(Direction.Y) * LimitAngleY;
			if (Angle > LimitAngle)
			{
				// Reproject to correct angle
				const double Cos = FMath::Cos(FMath::DegreesToRadians(LimitAngle));
				const double Shrink = FMath::Sqrt(1.0f - FMath::Square(Cos));
				Project.X = Cos * Radius;
				Project.Y = Direction.X * Shrink * Radius;
				Project.Z = Direction.Y * Shrink * Radius;
				return Project;
			}
			else
			{
			}
		}
		else if(Project.X < 0.0f)
		{
			// If we're aligned behind forward direction we just project to the origin
			Radius = 0.0f;
		}

		// Recompute in case radius changed
		Project = Normal * Radius;
	}

	return Project;
}

void UTGOR_AttachComponent::SetLimits(double Radius, double AngleX, double AngleY)
{
	LimitRadius = FMath::Max(0.f, Radius);
	LimitAngleX = FMath::Clamp(AngleX, 0.f, 180.0f);
	LimitAngleY = FMath::Clamp(AngleY, 0.f, 180.0f);
	OffsetAngleX = FMath::Clamp(OffsetAngleX, -LimitAngleX, LimitAngleX);
	OffsetAngleY = FMath::Clamp(OffsetAngleY, -LimitAngleY, LimitAngleY);
	UpdateBounds();
	UpdateBodySetup();
	MarkRenderStateDirty();
}

void UTGOR_AttachComponent::SetOffsets(double OffX, double OffY)
{
	OffsetAngleX = FMath::Clamp(OffX, -LimitAngleX, LimitAngleX);
	OffsetAngleY = FMath::Clamp(OffY, -LimitAngleY, LimitAngleY);
	UpdateBounds();
	UpdateBodySetup();
	MarkRenderStateDirty();
}

FQuat UTGOR_AttachComponent::GetAlignmentRotation() const
{
	const FQuat OffX = FQuat(FVector(0, 0, 1), FMath::DegreesToRadians(OffsetAngleX));
	const FQuat OffY = FQuat(FVector(0, 1, 0), FMath::DegreesToRadians(OffsetAngleY));
	return OffX * OffY;
}

FVector UTGOR_AttachComponent::GetAlignmentDirection() const
{
	return GetRelativeTransform().TransformVectorNoScale(GetAlignmentRotation().GetAxisX());
}

FVector UTGOR_AttachComponent::GetAlignmentLocation() const
{
	return GetRelativeLocation();
}

void UTGOR_AttachComponent::DebugDrawCollision(double Size, double Duration)
{
	if (ShapeBodySetup)
	{
		const FTransform& Transform = GetComponentTransform();
		for (const FKConvexElem& Elem : ShapeBodySetup->AggGeom.ConvexElems)
		{
			for (const FVector& Vertex : Elem.VertexData)
			{
				DrawDebugPoint(GetWorld(), Transform.TransformPosition(Vertex), Size, FColor::Red, false, Duration);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FBoxSphereBounds UTGOR_AttachComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	const double MaxLimitAngleCos = FMath::Max(FMath::Abs(FMath::Cos(LimitAngleX)), FMath::Abs(FMath::Cos(LimitAngleY)));
	FVector BoxPoint = FVector(FMath::Sin(LimitAngleX), FMath::Sin(LimitAngleY), MaxLimitAngleCos) * LimitRadius;
	return FBoxSphereBounds(FVector::ZeroVector, BoxPoint, LimitRadius).TransformBy(LocalToWorld);
}

#if WITH_EDITOR
void UTGOR_AttachComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	// We only want to modify the property that was changed at this point
	// things like propagation from CDO to instances don't work correctly if changing one property causes a different property to change
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UTGOR_AttachComponent, LimitRadius) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(UTGOR_AttachComponent, LimitAngleX) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(UTGOR_AttachComponent, LimitAngleY))
	{
		SetLimits(LimitRadius, LimitAngleX, LimitAngleY);
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UTGOR_AttachComponent, OffsetAngleX) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(UTGOR_AttachComponent, OffsetAngleY))
	{
		SetOffsets(OffsetAngleX, OffsetAngleY);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif	// WITH_EDITOR

////////////////////////////////////////////////////////////////////////////////////////////////////


FVector CalcLimitConeVert(double AngleX, double AngleY, double Phi, double Ratio)
{
	const double CosP = FMath::Cos(Phi);
	const double SinP = FMath::Sin(Phi);

	const double A = FMath::Square(CosP) * AngleX + FMath::Square(SinP) * AngleY;
	const double CosA = FMath::Cos(A * Ratio);
	const double SinA = FMath::Sin(A * Ratio);

	return FVector(CosA, CosP * SinA, SinP * SinA);
}



template <EShapeBodySetupHelper UpdateBodySetupAction>
bool InvalidateOrUpdateConeBodySetup(TObjectPtr<UBodySetup>& ShapeBodySetup, bool bUseArchetypeBodySetup, double LimitRadius, double AngleX, double AngleY)
{
	check((bUseArchetypeBodySetup && UpdateBodySetupAction == EShapeBodySetupHelper::InvalidateSharingIfStale) || (!bUseArchetypeBodySetup && UpdateBodySetupAction == EShapeBodySetupHelper::UpdateBodySetup));

	const int32 Count = ShapeBodySetup->AggGeom.ConvexElems.Num();
	check(Count > 0);

	if (UpdateBodySetupAction == EShapeBodySetupHelper::UpdateBodySetup)
	{
		TArray<FVector> Vertices;

		const int32 ConeSides = 16;
		const int32 SphereSides = 4;

		auto CreateElem = [&](FKConvexElem& Elem, const double A, const double B) {

			Elem.VertexData.Reset((ConeSides + 1) * SphereSides);
			Elem.VertexData.Emplace(FVector::ZeroVector);
			Elem.VertexData.Emplace(FVector(LimitRadius, 0, 0));
			for (int32 ConeSide = 0; ConeSide <= ConeSides; ConeSide++)
			{
				const double Azi = 2.0 * PI * (A + (B - A) * (double)ConeSide / (double)(ConeSides));
				for (int32 SphereSide = 1; SphereSide <= SphereSides; SphereSide++)
				{
					const double Ratio = (double)SphereSide / (double)(SphereSides);
					Elem.VertexData.Emplace(CalcLimitConeVert(AngleX, AngleY, Azi, Ratio) * LimitRadius);
				}
			}

			Elem.SetTransform(FTransform::Identity);
			Elem.ElemBox = FBox(Elem.VertexData);
		};

		for (int32 Index = 0; Index < Count; Index++)
		{
			CreateElem(ShapeBodySetup->AggGeom.ConvexElems[Index], ((double)(Index)) / Count, ((double)(Index + 1)) / Count);
		}

		ShapeBodySetup->BodySetupGuid = FGuid::NewGuid();
		ShapeBodySetup->bHasCookedCollisionData = true;
		ShapeBodySetup->InvalidatePhysicsData();
		ShapeBodySetup->CreatePhysicsMeshes();
	}
	else
	{
		ShapeBodySetup = nullptr;
		bUseArchetypeBodySetup = false;
	}

	return bUseArchetypeBodySetup;
}


void UTGOR_AttachComponent::UpdateBodySetup()
{
	const double AngleX = FMath::DegreesToRadians(LimitAngleX);
	const double AngleY = FMath::DegreesToRadians(LimitAngleY);
	if (PrepareSharedBodySetup<UTGOR_AttachComponent>())
	{
		bUseArchetypeBodySetup = InvalidateOrUpdateConeBodySetup<EShapeBodySetupHelper::InvalidateSharingIfStale>(ShapeBodySetup, bUseArchetypeBodySetup, LimitRadius, AngleX, AngleY);
	}

	if (ShapeBodySetup == nullptr || !IsValid(ShapeBodySetup))
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

		ShapeBodySetup->bGenerateMirroredCollision = false;
		ShapeBodySetup->bDoubleSidedGeometry = true;
		ShapeBodySetup->CollisionTraceFlag = CTF_UseSimpleAsComplex;
		ShapeBodySetup->AggGeom.ConvexElems.Add(FKConvexElem());
		//ShapeBodySetup->AggGeom.ConvexElems.Add(FKConvexElem());
		//ShapeBodySetup->AggGeom.ConvexElems.Add(FKConvexElem());
		//ShapeBodySetup->AggGeom.ConvexElems.Add(FKConvexElem());
		bUseArchetypeBodySetup = false;	//We're making our own body setup, so don't use the archetype's.

		//Update bodyinstance and shapes
		/*
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
							FPhysicsInterface::SetUserData(Shape, (void*)ShapeBodySetup->AggGeom.ConvexElems[0].GetUserData());
						}
					}
				});
			}
		}
		*/
	}

	if (!bUseArchetypeBodySetup)
	{
		InvalidateOrUpdateConeBodySetup<EShapeBodySetupHelper::UpdateBodySetup>(ShapeBodySetup, bUseArchetypeBodySetup, LimitRadius, AngleX, AngleY);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FPrimitiveSceneProxy* UTGOR_AttachComponent::CreateSceneProxy()
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

		FDrawCylinderSceneProxy(const UTGOR_AttachComponent* InComponent)
			: FPrimitiveSceneProxy(InComponent)
			, bDrawOnlyIfSelected(InComponent->bDrawOnlyIfSelected)
			, LimitRadius(InComponent->LimitRadius)
			, LimitAngleX(FMath::DegreesToRadians(InComponent->LimitAngleX))
			, LimitAngleY(FMath::DegreesToRadians(InComponent->LimitAngleY))
			, Alignment(InComponent->GetAlignmentRotation())
			, ShapeColor(InComponent->ShapeColor)
		{
			bWillEverBeLit = false;
		}

		virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_GetDynamicMeshElements_DrawDynamicElements);

			const FMatrix& LocalToWorld = GetLocalToWorld();
			const int32 ConeSides = FMath::Clamp<int32>(LimitRadius / 4.f, 16, 64);

			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{
				if (VisibilityMap & (1 << ViewIndex))
				{
					const FSceneView* View = Views[ViewIndex];
					const FLinearColor DrawColor = GetViewSelectionColor(ShapeColor, *View, IsSelected(), IsHovered(), false, IsIndividuallySelected());

					FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);

					// Draw cone
					FVector Prev = LocalToWorld.TransformPosition(CalcLimitConeVert(LimitAngleX, LimitAngleY, 0.0f, 1.0f) * LimitRadius); // CalcConeVert
					for (int32 ConeSide = 1; ConeSide <= ConeSides; ConeSide++)
					{
						double Fraction = (double)ConeSide / (double)(ConeSides);
						double Azi = 2.0 * PI * Fraction;


						const FVector Current = LocalToWorld.TransformPosition(CalcLimitConeVert(LimitAngleX, LimitAngleY, Azi, 1.0f) * LimitRadius); // CalcConeVert
						PDI->DrawLine(LocalToWorld.GetOrigin(), Current, DrawColor, SDPG_World, 0.1f);
						PDI->DrawLine(Prev, Current, DrawColor, SDPG_World, 0.5f);
						Prev = Current;
					}

					// Draw forward
					const FVector X = LocalToWorld.GetScaledAxis(EAxis::X);
					const FVector Top = LocalToWorld.GetOrigin() + X * LimitRadius * 0.5f;
					PDI->DrawLine(LocalToWorld.GetOrigin(), Top, DrawColor, SDPG_World, 0.5f);

					// Draw alignment
					const FVector F = LocalToWorld.TransformVector(Alignment.GetAxisX());
					const FVector Align = LocalToWorld.GetOrigin() + F * LimitRadius;
					PDI->DrawLine(LocalToWorld.GetOrigin(), Align, DrawColor, SDPG_World, 1.0f);

					const FVector Y = LocalToWorld.TransformVector(Alignment.GetAxisY());
					const FVector Z = LocalToWorld.TransformVector(Alignment.GetAxisZ());
					DrawCircle(PDI, Align, Y, Z, DrawColor, LimitRadius / 8, ConeSides, SDPG_World, 1.0f);
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
		const float		LimitRadius;
		const float		LimitAngleX;
		const float		LimitAngleY;
		const FQuat		Alignment;
		const FColor	ShapeColor;
	};

	return new FDrawCylinderSceneProxy(this);
}



bool UTGOR_AttachComponent::IsZeroExtent() const
{
	return (LimitRadius == 0.f);
}

bool UTGOR_AttachComponent::AreSymmetricRotations(const FQuat& A, const FQuat& B, const FVector& Scale3D) const
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
