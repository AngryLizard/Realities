// The Gateway of Realities: Planes of Existence.


#include "TGORRigUnit_Utility.h"

#include "RealitiesUtility/Structures/TGOR_Matrix3x3.h"
#include "Units/RigUnitContext.h"

////////////////////////////////////////////////////////////////////////////////////////////////////


FVector ComputeScaleBetween(const FVector& S, const FVector& A, const FVector& B)
{
	FVector Scale = S;
	if (!FMath::IsNearlyZero(A.X, 0.1f))
	{
		Scale.X = B.X / A.X;
	}
	if (!FMath::IsNearlyZero(A.Y, 0.1f))
	{
		Scale.Y = B.Y / A.Y;
	}
	if (!FMath::IsNearlyZero(A.Z, 0.1f))
	{
		Scale.Z = B.Z / A.Z;
	}
	return Scale;
}

void FTGORRigUnit_Propagate::PropagateChainTorwards(const FRigElementKey& Current, const FRigElementKey& Next, const FVector& Target, FRigHierarchyContainer* Hierarchy, bool bPropagateToChildren)
{
	FTransform Transform = Hierarchy->GetGlobalTransform(Current);
	const FTransform Local = Hierarchy->GetLocalTransform(Next);

	// Rotate to match
	const FVector CurrentDelta = Transform.TransformVectorNoScale(Local.GetLocation());
	const FVector TargetDelta = Target - Transform.GetLocation();
	const FQuat Rotation = FQuat::FindBetweenVectors(CurrentDelta, TargetDelta);
	Transform.SetRotation(Rotation * Transform.GetRotation());

	// Scale to match
	Transform.SetScale3D(ComputeScaleBetween(Transform.GetScale3D(), Local.GetLocation(), Transform.InverseTransformVectorNoScale(TargetDelta)));

	// Update chain element
	Hierarchy->SetGlobalTransform(Current, Transform, bPropagateToChildren);

	// Propagate to only next in line if propagation is turned off
	if (!bPropagateToChildren)
	{
		Hierarchy->SetGlobalTransform(Next, Local * Transform, false);
	}
}

FTGORRigUnit_Propagate_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	FRigHierarchyContainer* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
	}
	else
	{
		PropagateChainTorwards(Key, NextKey, TargetLocation, Hierarchy, PropagateToChildren != ETGOR_Propagation::Off);
	}
}

FString FTGORRigUnit_Propagate::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_CustomiseInitial_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	FRigHierarchyContainer* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		CachedItems.Reset();
		Transforms.Reset();
		return;
	}

	if (Context.State == EControlRigState::Update)
	{
		const int32 ItemCount = Items.Num();
		if (CachedItems.Num() == 0 && ItemCount > 0)
		{
			for (const FRigUnit_BoneCustomisation& Item : Items)
			{
				FCachedRigElement Element = FCachedRigElement(Item.Key, Hierarchy);
				CachedItems.Add(Element);
				Transforms.Add(Hierarchy->GetInitialTransform(Element));
			}
		}

		for (int32 Index = 0; Index < ItemCount; Index++)
		{
			const FRigUnit_BoneCustomisation& Item = Items[Index];
			FCachedRigElement& Element = CachedItems[Index];
			FTransform Transform = Transforms[Index];
			FVector Scale = Transform.GetScale3D();
			Scale.SetComponentForAxis(ScaleAxis, Scale.GetComponentForAxis(ScaleAxis) * Item.Scale);
			Transform.SetScale3D(Scale);
			Hierarchy->SetInitialTransform(Element, Transform);
		}

	}
}

FString FTGORRigUnit_CustomiseInitial::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_CloneTransforms_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	FRigHierarchyContainer* Hierarchy = ExecuteContext.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
	}
	else
	{
		const int32 Num = Items.Num();
		for (int32 Index = 0; Index < Num; Index++)
		{
			const FTransform Transform = Hierarchy->GetGlobalTransform(Items[Index]);
			const FRigElementKey NewKey = FRigElementKey(Items[Index].Name, TargetType);
			if (Hierarchy->GetIndex(NewKey) != INDEX_NONE)
			{
				Hierarchy->SetGlobalTransform(NewKey, Transform, false);
			}
		}
	}
}

FString FTGORRigUnit_CloneTransforms::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_TriangleEstimateDirection_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	const FRigHierarchyContainer* Hierarchy = Context.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		BaseCache.Reset();
		PivotCache.Reset();
		LeftCache.Reset();
		RightCache.Reset();
	}
	else
	{
		Origin = FVector::ZeroVector;
		Direction = FVector::ForwardVector;

		if (!BaseCache.UpdateCache(BaseKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Base '%s' is not valid."), *BaseKey.ToString());
		}
		else if (!PivotCache.UpdateCache(PivotKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Pivot '%s' is not valid."), *PivotKey.ToString());
		}
		else if (!LeftCache.UpdateCache(LeftKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Left '%s' is not valid."), *LeftKey.ToString());
		}
		else if (!RightCache.UpdateCache(RightKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Right '%s' is not valid."), *RightKey.ToString());
		}
		else
		{
			const FTransform Base = Hierarchy->GetGlobalTransform(BaseCache);
			const FVector Pivot = Hierarchy->GetGlobalTransform(PivotCache).GetLocation();
			const FVector Left = Hierarchy->GetGlobalTransform(LeftCache).GetLocation();
			const FVector Right = Hierarchy->GetGlobalTransform(RightCache).GetLocation();

			const FVector Normal = (Base.GetLocation() - Pivot).GetSafeNormal();

			// Project average left/right location onto pivot direction
			Origin = ((Right + Left) / 2).ProjectOnToNormal(Normal);

			// Project forward direction along right/left plane
			Direction = FVector::VectorPlaneProject(Base.TransformVector(Axis), (Right - Left).GetSafeNormal());

			if (Project)
			{
				// Project direction to be orthogonal to pivot direction
				Direction = FVector::VectorPlaneProject(Direction, Normal);
			}

			Direction = Direction.GetSafeNormal();
		}
	}
}

FString FTGORRigUnit_TriangleEstimateDirection::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_TransformToPlane_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	const FRigHierarchyContainer* Hierarchy = Context.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
	}
	else
	{
		Output = FPlane(Transform.GetLocation(), Transform.TransformVectorNoScale(Axis));

	}
}

FString FTGORRigUnit_TransformToPlane::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_MeanDirection_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	const FRigHierarchyContainer* Hierarchy = Context.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		MeanCache.Reset();
	}
	else
	{
		Output = FVector::ForwardVector;

		if (!MeanCache.UpdateCache(MeanKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Mean '%s' is not valid."), *MeanKey.ToString());
		}
		else
		{
			const FVector Mean = Hierarchy->GetGlobalTransform(MeanCache).GetLocation();

			const int32 Num = SourceChain.Num();
			if (Num > 1)
			{
				const FVector First = Hierarchy->GetGlobalTransform(SourceChain.First()).GetLocation();
				const FVector Last = Hierarchy->GetGlobalTransform(SourceChain.Last()).GetLocation();
				const float InvSquared = 1.0f / (Last - First).SizeSquared();

				float MeanWeight = 0.0f;
				FVector MeanSum = FVector::ZeroVector;
				for (int32 Index = 1; Index < Num; Index++)
				{
					const FVector Location = Hierarchy->GetGlobalTransform(SourceChain[Index]).GetLocation();
					const float Weight = (Location - Mean).SizeSquared() * InvSquared;
					MeanSum += (Location - First) * Weight;
					MeanWeight += Weight;
				}

				Output = (MeanSum / MeanWeight).GetSafeNormal();
			}
		}
	}
}

FString FTGORRigUnit_MeanDirection::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

FTGORRigUnit_PowerDirection_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	const FRigHierarchyContainer* Hierarchy = Context.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
	}
	else
	{
		Output = FVector::ForwardVector;

		TArray<FVector> Samples;
		const int32 Num = SourceChain.Num();
		Samples.Reserve(Num);
		if (Num >= 2)
		{
			FVector Mean = FVector::ZeroVector;
			for (int32 Index = 0; Index < Num; Index++)
			{
				const FVector Sample = Hierarchy->GetGlobalTransform(SourceChain[Index]).GetLocation();
				Samples.Emplace(Sample);
				Mean += Sample;
			}
			Mean /= Num;

			for (FVector& Sample : Samples)
			{
				Sample -= Mean;
			}

			Output = (Samples[1] - Samples[0]).GetSafeNormal();
			Output = FTGOR_Matrix3x3(Samples).PowerMethod(Output, Iterations);
		}
	}
}

FString FTGORRigUnit_PowerDirection::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

float FTGORRigUnit_ChainLength::ComputeInitialChainLength(const FRigElementKeyCollection& Chain, const FRigHierarchyContainer* Hierarchy)
{
	float Length = 0.0f;

	const int32 Num = Chain.Num();
	if (Num > 2)
	{
		FVector Current = Hierarchy->GetInitialGlobalTransform(Chain[0]).GetLocation();
		for (int32 Index = 1; Index < Num; Index++)
		{
			const FVector Next = Hierarchy->GetInitialGlobalTransform(Chain[Index]).GetLocation();
			Length += (Current - Next).Size();
			Current = Next;
		}
	}
	return Length;
}

FTGORRigUnit_ChainLength_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	const FRigHierarchyContainer* Hierarchy = Context.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
	}
	else
	{
		ChainMaxLength = ComputeInitialChainLength(SourceChain, Hierarchy) * ChainLengthMultiplier;
	}
}

FString FTGORRigUnit_ChainLength::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}





void FTGORRigUnit_ChainAnalysis::ChainAnalysis(const FRigElementKeyCollection& Chain, const FRigHierarchyContainer* Hierarchy, float Multiplier, float& ChainMaxLength, float& CurrentLength, float& InitialLength)
{
	ChainMaxLength = FTGORRigUnit_ChainLength::ComputeInitialChainLength(Chain, Hierarchy) * Multiplier;

	const FVector First = Hierarchy->GetGlobalTransform(Chain.First()).GetLocation();
	const FVector Last = Hierarchy->GetGlobalTransform(Chain.Last()).GetLocation();
	CurrentLength = (Last - First).Size();

	const FVector InitialFirst = Hierarchy->GetInitialGlobalTransform(Chain.First()).GetLocation();
	const FVector InitialLast = Hierarchy->GetInitialGlobalTransform(Chain.Last()).GetLocation();
	InitialLength = (InitialLast - InitialFirst).Size();
}

FTGORRigUnit_ChainAnalysis_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	const FRigHierarchyContainer* Hierarchy = Context.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
	}
	else
	{
		if (SourceChain.Num() < 2)
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Chain has to have length at least 2."));
		}
		else
		{
			ChainAnalysis(SourceChain, Hierarchy, ChainLengthMultiplier, ChainMaxLength, CurrentLength, InitialLength);

			if (FMath::IsNearlyZero(ChainMaxLength))
			{
				UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Max chain length is null."));
			}
			else
			{
				LengthRatio = CurrentLength / ChainMaxLength;
			}
		}
	}
}

FString FTGORRigUnit_ChainAnalysis::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}

////////////////////////////////////////////////////////////////////////////////////////////////////


void FTGORRigUnit_Rebase::Rebase(FTransform& Transform, const FTransform& Current, const FTransform& From, const FTransform& To, float Unit)
{
	Transform = (Current * From.Inverse());
	Transform.SetLocation(Transform.GetLocation() * Unit);
	Transform *= To;
}

FTGORRigUnit_Rebase_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	if (Context.State == EControlRigState::Init)
	{
	}
	else
	{
		Rebase(Output, Transform, FromSpace, ToSpace, UnitScale);
	}
}


FString FTGORRigUnit_Rebase::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}

////////////////////////////////////////////////////////////////////////////////////////////////////


void FTGORRigUnit_ConvertSpace::ConvertSpace(FTransform& Transform, const FTransform& Current, const FTransform& From, const FTransform& To, float Unit)
{
	/*
	Transform = (Current * From.Inverse());
	Transform.SetLocation(Transform.GetLocation() * Unit);
	Transform *= To;
	*/

	Transform.SetLocation((Current.GetLocation() - From.GetLocation()) * Unit + To.GetLocation());
	Transform.SetRotation((Current.GetRotation() * From.GetRotation().Inverse()) * To.GetRotation());
	Transform.NormalizeRotation();
	//Transform.SetScale3D(Transform.GetScale3D() * Scale);
}

FTGORRigUnit_ConvertSpace_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	if (Context.State == EControlRigState::Init)
	{
	}
	else
	{
		ConvertSpace(Output, Transform, FromSpace, ToSpace, UnitScale);
	}
}


FString FTGORRigUnit_ConvertSpace::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FQuat FTGORRigUnit_RotateToward::ComputeHeadingRotation(const FVector& SourceForward, const FVector& TargetForward, const FVector& SourceUp, const FVector& TargetUp)
{
	FQuat Rotation = FQuat::FindBetweenNormals(SourceForward, TargetForward);
	const FVector UpProject = FVector::VectorPlaneProject(TargetUp, TargetForward).GetSafeNormal();

	// Flip if on the other hemisphere for more accurate results
	FVector UpTarget = Rotation * SourceUp;
	if ((UpTarget | UpProject) < 0.0f)
	{
		// This corresponds to a 180° rotation around forward axis
		Rotation = FQuat(TargetForward.X, TargetForward.Y, TargetForward.Z, 0.0f) * Rotation;
		Rotation = FQuat::FindBetweenNormals(-UpTarget, UpProject) * Rotation;
	}
	else
	{
		Rotation = FQuat::FindBetweenNormals(UpTarget, UpProject) * Rotation;
	}

	return Rotation.GetNormalized();
}


FTGORRigUnit_RotateToward_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()

	if (Context.State == EControlRigState::Init)
	{
	}
	else
	{
		if (SourceForward.IsNearlyZero())
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Source forward is null."));
		}
		else if (TargetForward.IsNearlyZero())
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Target forward is null."));
		}
		else if (SourceUp.IsNearlyZero())
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Source up is null."));
		}
		else if (TargetUp.IsNearlyZero())
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Target up is null."));
		}
		else
		{
			Output = ComputeHeadingRotation(SourceForward, TargetForward.GetSafeNormal(), SourceUp, TargetUp.GetSafeNormal());
		}
	}
}

FString FTGORRigUnit_RotateToward::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}

////////////////////////////////////////////////////////////////////////////////////////////////////


FTGORRigUnit_GetScaleLength_Execute()
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_RIGUNIT()
	const FRigHierarchyContainer* Hierarchy = Context.Hierarchy;

	if (Context.State == EControlRigState::Init)
	{
		ConeCache.Reset();
	}
	else
	{
		if (!ConeCache.UpdateCache(ConeKey, Hierarchy))
		{
			UE_CONTROLRIG_RIGUNIT_REPORT_WARNING(TEXT("Cone '%s' is not valid."), *ConeKey.ToString());
		}
		else
		{
			const FTransform Transform = Hierarchy->GetGlobalTransform(ConeCache);
			Output = Transform.GetScale3D().GetComponentForAxis(Axis) * 100.0f;
		}
	}
}

FString FTGORRigUnit_GetScaleLength::ProcessPinLabelForInjection(const FString& InLabel) const
{
	FString Formula;
	return FString::Printf(TEXT("%s: TODO"), *InLabel);
}

/// ///////////////////////////////////////////////////////// Linker doesn't wanna give it to me? fine, just copy paste!

bool FCachedRigElement::UpdateCache(const FRigHierarchyContainer* InContainer)
{
	if (InContainer)
	{
		if (!IsValid() || InContainer->Version != ContainerVersion)
		{
			return UpdateCache(GetKey(), InContainer);
		}
		return IsValid();
	}
	return false;
}

bool FCachedRigElement::UpdateCache(const FRigElementKey& InKey, const FRigHierarchyContainer* InContainer)
{
	if (InContainer)
	{
		if (!IsValid() || !IsIdentical(InKey, InContainer))
		{
			Reset();

			int32 Idx = InContainer->GetIndex(InKey);
			if (Idx != INDEX_NONE)
			{
				Key = InKey;
				Index = (uint16)Idx;
			}

			ContainerVersion = InContainer->Version;
		}
		return IsValid();
	}
	return false;
}

bool FCachedRigElement::IsIdentical(const FRigElementKey& InKey, const FRigHierarchyContainer* InContainer)
{
	return InKey == Key && InContainer->Version == ContainerVersion;
}

/// /////////////////////////////////////////////////////////
