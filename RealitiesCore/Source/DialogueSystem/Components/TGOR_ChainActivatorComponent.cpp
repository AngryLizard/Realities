// The Gateway of Realities: Planes of Existence.

#include "TGOR_ChainActivatorComponent.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"

#pragma optimize("", off)
FTransform LerpTransforms(const FTransform& From, const FTransform& To, float Time)
{
	const FVector Location = FMath::Lerp(From.GetLocation(), To.GetLocation(), Time);
	const FQuat Rotation = FQuat::FastLerp(From.GetRotation(), To.GetRotation(), Time);
	const FVector Scale = FMath::Lerp(From.GetScale3D(), To.GetScale3D(), Time);
	return FTransform(Rotation, Location, Scale);
}

FTransform TangentTransforms(const FTGOR_ChainCurve& Curve)
{
	return FTransform(FVector(Curve.Tangent, 0, 0)) * Curve.Transform;
}

FTransform EvalCurve(const FTGOR_ChainCurve& From, const FTGOR_ChainCurve& To, float Time)
{
	const FTransform A = From.Transform;
	const FTransform B = TangentTransforms(From);
	const FTransform C = TangentTransforms(To);
	const FTransform D = To.Transform;

	const float T = Time; //(3.0f - 2.0f * Time) * Time * Time;

	const FTransform E = LerpTransforms(A, B, T);
	const FTransform F = LerpTransforms(B, C, T);
	const FTransform G = LerpTransforms(C, D, T);

	const FTransform H = LerpTransforms(E, F, T);
	const FTransform I = LerpTransforms(F, G, T);

	return LerpTransforms(H, I, T);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ChainActivatorComponent::UTGOR_ChainActivatorComponent()
	: Super()
{
}

void UTGOR_ChainActivatorComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get prev attached chain child
	USceneComponent* Parent = GetAttachParent();
	TArray<UTGOR_ChainActivatorComponent*> Chains;

	TArray<USceneComponent*> Children;
	Parent->GetChildrenComponents(false, Children);
	for (USceneComponent* Child : Children)
	{
		UTGOR_ChainActivatorComponent* Component = Cast<UTGOR_ChainActivatorComponent>(Child);
		if (IsValid(Component) && Component != this)
		{
			Chains.Add(Component);
		}
	}

	// Build chain, O(n^2), consider using linked list if performance becomes a problem
	ChainCache.Reset(1);
	ChainCache.Add(this);
	ChainIndex = 0;

	bool bFoundChainLink = true;
	while (bFoundChainLink)
	{
		// Look for matching chain link to extend the chain, stop iterating if none was found
		bFoundChainLink = false;
		for (auto It = Chains.CreateIterator(); It; It++)
		{
			if (ChainCache[0]->PrevChainLink == (*It)->GetFName())
			{
				ChainCache.Insert((*It), 0);
				bFoundChainLink = true;
				It.RemoveCurrent();
				ChainIndex++;
				break;
			}
			else if (ChainCache.Last()->NextChainLink == (*It)->GetFName())
			{
				ChainCache.Add((*It));
				bFoundChainLink = true;
				It.RemoveCurrent();
				break;
			}
		}
	}
}

void UTGOR_ChainActivatorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*
	ChainPosition += ChainVelocity * DeltaTime;
	ChainPosition = FMath::Clamp(ChainPosition, -1, ChainCache.Num());

	// Update link information
	const int32 Start = FMath::Min(FMath::FloorToInt(ChainPosition), ChainCache.Num() - 1);
	CurrentFromLink = ChainCache.IsValidIndex(Start) ? ChainCache[Start] : nullptr;

	const int32 End = FMath::Max(FMath::CeilToInt(ChainPosition), 0);
	CurrentToLink = ChainCache.IsValidIndex(End) ? ChainCache[End] : nullptr;

	CurrentCurveTime = FMath::Clamp(ChainPosition - Start, 0.f, 1.f);
	*/

	if (bEnableDebugDraw)
	{
		FTGOR_ChainCurve PrevFrom, PrevTo;
		GetPrevCurve(PrevFrom, PrevTo);
		FTGOR_ChainCurve NextFrom, NextTo;
		GetNextCurve(NextFrom, NextTo);

		const int32 PointNum = 8;
		for (int32 Index = 1; Index <= PointNum; Index++)
		{
			const FVector PrevLocation = EvalCurve(PrevFrom, PrevTo, (float(Index) - 0.5f) / PointNum).GetLocation();
			DrawDebugPoint(GetWorld(), PrevLocation, 1.5f, FColor::Blue, false, -1.0f, SDPG_Foreground);
			const FVector NextLocation = EvalCurve(NextFrom, NextTo, float(Index) / PointNum).GetLocation();
			DrawDebugPoint(GetWorld(), NextLocation, 1.5f, FColor::Red, false, -1.0f, SDPG_Foreground);
		}
	}
}


FVector UTGOR_ChainActivatorComponent::WorldToTarget(const FVector& Location) const
{
	/*
	// Project onto target plane
	FTransform Transform = GetComponentTransform();
	Transform.SetScale3D(FVector::OneVector);
	const FVector LocalLocation = Transform.InverseTransformPosition(Location);
	const FVector2D Domain = FVector2D(LocalLocation) / FVector2D::Max(Size, FVector2D(KINDA_SMALL_NUMBER));
	return FVector(FVector2D::Max(FVector2D::Min(Domain, DomainMax), DomainMin), 0.0f);
	*/

	return FVector::ZeroVector;
}

FVector UTGOR_ChainActivatorComponent::TargetToWorld(const FVector& Local) const
{
	// Update link information
	const int32 Start = FMath::Min(FMath::FloorToInt(Local.Z), ChainCache.Num() - 1);
	UTGOR_ChainActivatorComponent* FromLink = ChainCache.IsValidIndex(Start) ? ChainCache[Start] : nullptr;

	const int32 End = FMath::Max(FMath::CeilToInt(Local.Z), 0);
	UTGOR_ChainActivatorComponent* ToLink = ChainCache.IsValidIndex(End) ? ChainCache[End] : nullptr;

	float CurveTime = FMath::Clamp(float(Local.Z) - float(Start), 0.f, 1.f);

	FTGOR_ChainCurve From, To;
	GetCurve(FromLink, ToLink, From, To);
	FTransform Transform = EvalCurve(From, To, CurveTime);

	Transform.SetScale3D(FVector::OneVector);
	return (FTransform(OffsetRotation.Quaternion()) * Transform).GetLocation();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ChainActivatorComponent* UTGOR_ChainActivatorComponent::FindChainLink(const FName& Name) const
{
	if (Name.IsNone())
	{
		return nullptr;
	}

	// Get prev attached chain child
	USceneComponent* Parent = GetAttachParent();

	TArray<USceneComponent*> Children;
	Parent->GetChildrenComponents(false, Children);
	for (USceneComponent* Child : Children)
	{
		UTGOR_ChainActivatorComponent* Component = Cast<UTGOR_ChainActivatorComponent>(Child);
		if (IsValid(Component) && Component->GetFName() == Name)
		{
			return Component;
		}
	}
	return nullptr;
}

void UTGOR_ChainActivatorComponent::GetCurve(const UTGOR_ChainActivatorComponent* PrevLink, const UTGOR_ChainActivatorComponent* NextLink, FTGOR_ChainCurve& From, FTGOR_ChainCurve& To)
{
	if (PrevLink)
	{
		const FTransform Transform = PrevLink->GetComponentTransform();

		// Create origin curves
		From.Transform = Transform;
		From.Tangent = PrevLink->NextChainTangent;

		if (NextLink)
		{
			// Get next chain link curve
			To.Transform = NextLink->GetComponentTransform();
			To.Tangent = -NextLink->PrevChainTangent;
		}
		else
		{
			// Assume tangent itself as default
			To.Transform = TangentTransforms(From);
			To.Tangent = -PrevLink->NextChainTangent;
		}
	}
	else if(NextLink)
	{
		const FTransform Transform = NextLink->GetComponentTransform();

		// Create origin curves
		To.Transform = Transform;
		To.Tangent = -NextLink->PrevChainTangent;

		if (PrevLink)
		{
			// Get prev chain link curve
			From.Transform = PrevLink->GetComponentTransform();
			From.Tangent = PrevLink->NextChainTangent;
		}
		else
		{
			// Assume tangent itself as default
			From.Transform = TangentTransforms(To);
			From.Tangent = NextLink->PrevChainTangent;
		}
	}

}

void UTGOR_ChainActivatorComponent::GetPrevCurve(FTGOR_ChainCurve& From, FTGOR_ChainCurve& To) const
{
	GetCurve(FindChainLink(PrevChainLink), this, To, From);
}

void UTGOR_ChainActivatorComponent::GetNextCurve(FTGOR_ChainCurve& From, FTGOR_ChainCurve& To) const
{
	GetCurve(this, FindChainLink(NextChainLink), From, To);
}

#if WITH_EDITOR
void UTGOR_ChainActivatorComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif	// WITH_EDITOR

FPrimitiveSceneProxy* UTGOR_ChainActivatorComponent::CreateSceneProxy()
{
	class FDrawChainActivatorSceneProxy final : public FDrawActivatorSceneProxy
	{
	public:
		FDrawChainActivatorSceneProxy(const UTGOR_ChainActivatorComponent* InComponent)
			: UTGOR_ActivatorComponent::FDrawActivatorSceneProxy(InComponent)
		{
			PrevTangent = InComponent->PrevChainTangent;
			NextTangent = InComponent->NextChainTangent;
		}

		virtual void DrawTargetSpace(const FMatrix& Transform, const FLinearColor& DrawColor, FPrimitiveDrawInterface* PDI) const override
		{
			FDrawActivatorSceneProxy::DrawTargetSpace(Transform, DrawColor, PDI);

			const FVector ScaledX = Transform.GetUnitAxis(EAxis::X);

			PDI->DrawLine(Transform.GetOrigin(), Transform.GetOrigin() - ScaledX * PrevTangent, FColor::Blue, SDPG_Foreground, 1.0f);
			PDI->DrawPoint(Transform.GetOrigin() - ScaledX * PrevTangent, FColor::Blue, 5.0f, SDPG_Foreground);

			PDI->DrawLine(Transform.GetOrigin(), Transform.GetOrigin() + ScaledX * NextTangent, FColor::Red, SDPG_Foreground, 1.0f);
			PDI->DrawPoint(Transform.GetOrigin() + ScaledX * NextTangent, FColor::Red, 5.0f, SDPG_Foreground);
		}

	private:
		float PrevTangent;
		float NextTangent;
	};

	return new FDrawChainActivatorSceneProxy(this);
}
#pragma optimize("", on)