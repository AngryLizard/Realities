// The Gateway of Realities: Planes of Existence.


#include "TGOR_Archetype.h"

#include "AnimationSystem/Instances/TGOR_AnimInstance.h"
#include "AnimationSystem/Instances/TGOR_SubAnimInstance.h"
#include "AnimationSystem/Interfaces/TGOR_AnimationInterface.h"
#include "AnimationSystem/Components/TGOR_AnimationComponent.h"
#include "AnimationSystem/Components/TGOR_AttachComponent.h"
#include "AnimationSystem/Content/TGOR_Animation.h"
#include "AnimationSystem/Content/TGOR_Performance.h"
#include "AnimationSystem/Content/TGOR_RigParam.h"
#include "DimensionSystem/Content/TGOR_Primitive.h"
#include "CustomisationSystem/Actors/TGOR_PreviewActor.h"

#if WITH_EDITOR
#include "Kismet2/BlueprintEditorUtils.h"
#endif // WITH_EDITOR

UTGOR_Archetype::UTGOR_Archetype()
	: Super()
{
	InstanceClass = UTGOR_AnimInstance::StaticClass();
	DefaultCustomisationActor = ATGOR_PreviewActor::StaticClass();
}

bool UTGOR_Archetype::Validate_Implementation()
{
	if (!*InstanceClass)
	{
		ERRET("No animation instance defined", Error, false);
	}

	return Super::Validate_Implementation();
}

TSubclassOf<AActor> UTGOR_Archetype::GetSpawnClass_Implementation() const
{
	return DefaultCustomisationActor;
}

float UTGOR_Archetype::GetRigParamValue(TSubclassOf<UTGOR_RigParam> RigParamType, const TMap<UTGOR_RigParam*, FTGOR_Normal>& Values, float Default) const
{
	// Get value from given map
	for (const auto& Pair : Values)
	{
		if (IsValid(Pair.Key) && Pair.Key->IsA(RigParamType))
		{
			return Pair.Value.Value;
		}
	}

	// Get default value from slot
	UTGOR_RigParam* SlotParam = Instanced_RigParamInsertions.GetOfType<UTGOR_RigParam>(RigParamType);
	if (IsValid(SlotParam))
	{
		return SlotParam->DefaultValue;
	}

	return Default;
}

UTGOR_Performance* UTGOR_Archetype::GetPerformance(TSubclassOf<UTGOR_Performance> Type) const
{
	return Instanced_PerformanceInsertions.GetOfType<UTGOR_Performance>(Type);
}

UTGOR_RigParam* UTGOR_Archetype::GetRigParam(TSubclassOf<UTGOR_RigParam> Type) const
{
	return Instanced_RigParamInsertions.GetOfType<UTGOR_RigParam>(Type);
}

UTGOR_AnimInstance* UTGOR_Archetype::GetAnimationInstance(UTGOR_AnimationComponent* Target)
{
	if (IsValid(Target))
	{
		return Target->AnimInstance.Get();
	}
	return nullptr;
}

void UTGOR_Archetype::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(AnimationInsertions);
	MOV_INSERTION(PerformanceInsertions);
	MOV_INSERTION(RigParamInsertions);
}



#if WITH_EDITOR

void UTGOR_Archetype::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	FProperty* ChangedProperty = PropertyChangedEvent.Property;

	if (ChangedProperty)
	{
		if (ChangedProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UTGOR_Archetype, InstanceClass))
		{
			UBlueprint* Blueprint = UBlueprint::GetBlueprintFromClass(GetClass());
			if (Blueprint)
			{
				FBlueprintEditorUtils::RefreshAllNodes(Blueprint);
			}
		}
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif // WITH_EDITOR