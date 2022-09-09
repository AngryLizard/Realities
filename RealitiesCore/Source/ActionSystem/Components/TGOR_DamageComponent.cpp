// The Gateway of Realities: Planes of Existence.

#include "TGOR_DamageComponent.h"

#include "TargetSystem/Content/TGOR_Target.h"

#include "TargetSystem/Components/TGOR_InteractableComponent.h"
#include "ActionSystem/Components/TGOR_ImpactComponent.h"


UTGOR_DamageComponent::UTGOR_DamageComponent()
	: Super()
{
	SetGenerateOverlapEvents(true);
}

void UTGOR_DamageComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &UTGOR_DamageComponent::OnDamageOverlap);
}

void UTGOR_DamageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TimelineCurrentTime += DeltaTime;
	if (TimelineCurrentTime > TimelineTotalTime)
	{
		DestroyComponent();
	}
}

void UTGOR_DamageComponent::InitialiseDamage(USkeletalMeshComponent* MeshComp, float Radius, float TotalTime, float Strength)
{
	TimelineTotalTime = TotalTime;
	TimelineCurrentTime = 0.f;
	ImpactStrength = Strength;

	SetSphereRadius(Radius, true);
}

void UTGOR_DamageComponent::HitAround(const FVector& Origin, float Radius, const FTGOR_ForceInstance& Forces)
{
	UWorld* World = GetWorld();

	// Generate Trace params
	TArray<AActor*> Ignore;
	FCollisionQueryParams TraceParams(FName(TEXT("ProjectileTrace")), true, GetOwner());
	TraceParams.bTraceComplex = false;
	TraceParams.bReturnFaceIndex = false;
	TraceParams.bReturnPhysicalMaterial = false;

	// Generate shape info
	const FCollisionShape Shape = FCollisionShape::MakeSphere(Radius);

	// Find interactables
	TArray<FOverlapResult> Overlaps;
	if (World->OverlapMultiByChannel(Overlaps, Origin, FQuat::Identity, HitChannel, Shape, TraceParams))
	{
		for (const FOverlapResult& Overlap : Overlaps)
		{
			/*
			UTGOR_InteractableComponent* Interactable = Cast<UTGOR_InteractableComponent>(Overlap.GetComponent());
			if (IsValid(Interactable))
			{
				for (UTGOR_Target* Target : Interactable->Targets)
				{
					FTGOR_AimPoint Point;
					if (Target->OverlapTarget(Interactable, Origin, Radius, Point))
					{
						FTGOR_InfluenceInstance Influence;
						Influence.Target = Point.Target;
						Influence.Forces = Forces * (1.0f - Point.Distance);
						Influence.Instigator = GetOwner();
						Influence.Location = Origin;

						Interactable->Influence(Influence);
					}
				}
			}
			*/
		}
	}
}

void UTGOR_DamageComponent::HitAroundSelf(const FTGOR_ForceInstance& Forces)
{
	const FVector Origin = GetComponentLocation();
	const float Radius = GetScaledSphereRadius();
	HitAround(Origin, Radius, Forces);
}

void UTGOR_DamageComponent::OnDamageOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (UTGOR_ImpactComponent* ImpactComponent = Cast<UTGOR_ImpactComponent>(OtherComp))
	{
		if (ImpactComponent->GetOwner() != GetOwner())
		{
			const FVector Direction = GetForwardVector();
			ImpactComponent->ApplyImpact(Direction, ImpactStrength);
		}
	}
}

FPrimitiveSceneProxy* UTGOR_DamageComponent::CreateSceneProxy()
{
	/** Represents an DamageComponent to the scene manager. */
	class FDamageSceneProxy final : public FPrimitiveSceneProxy
	{
	public:
		SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}

		/** Initialization constructor. */
		FDamageSceneProxy(const USphereComponent* InComponent)
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

					PDI->DrawLine(LocalToWorld.GetOrigin(), LocalToWorld.GetOrigin() + ScaledX * SphereRadius * 2.0f, DrawSphereColor, SDPG_World, 1.0f);
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

	return new FDamageSceneProxy(this);
}