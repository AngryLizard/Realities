// The Gateway of Realities: Planes of Existence.


#include "TGOR_Bodypart.h"

#include "Components/SkeletalMeshComponent.h"
#include "Realities/CoreSystem/TGOR_Singleton.h"
#include "Realities/CoreSystem/Utility/TGOR_ActorUtility.h"
#include "Realities/AnimationSystem/Content/TGOR_Archetype.h"
#include "Realities/CustomisationSystem/Content/TGOR_Skin.h"
#include "Realities/CustomisationSystem/Content/TGOR_Mask.h"
#include "Realities/CustomisationSystem/Content/TGOR_Palette.h"
#include "Realities/CustomisationSystem/Content/TGOR_Section.h"
#include "Realities/CustomisationSystem/UserData/TGOR_BodypartUserData.h"

#include "Realities/CustomisationSystem/Components/TGOR_ModularSkeletalMeshComponent.h"


#ifdef WITH_EDITOR
#include "Logging/MessageLog.h"
#endif


UTGOR_Bodypart::UTGOR_Bodypart()
	: Super()
{
	FollowsMasterPose = true;
}

void UTGOR_Bodypart::PostBuildResource()
{
	Super::PostBuildResource();
}

bool UTGOR_Bodypart::Validate_Implementation()
{
	if (!Mesh)
	{
		ERRET("No mesh defined", Error, false);
	}

	return Super::Validate_Implementation();
}


void UTGOR_Bodypart::OverrideMesh(UTGOR_ChildSkeletalMeshComponent* Component)
{
	// Set component mesh
	Component->SetSkeletalMesh(Mesh, false);
	
	// Override all materials
	UTGOR_ActorUtility::OverrideMesh(Component, MaterialOverride);

	//SINGLETON_CHK
	//Singleton->ContentManager->GetTListFromType<UTGOR_Bodypart>();
	//GetIListFromType<UTGOR_Bodypart>(UTGOR_Bodypart::StaticClass());
}

void UTGOR_Bodypart::AttachMesh(UTGOR_ChildSkeletalMeshComponent* Component, UTGOR_ModularSkeletalMeshComponent* MasterComponent, bool IgnoreFollowsMasterPose)
{
	if (IsValid(Component) && IsValid(MasterComponent) && Component != MasterComponent)
	{
		Component->AttachToComponent(MasterComponent, FAttachmentTransformRules::KeepRelativeTransform);
		if (FollowsMasterPose && !IgnoreFollowsMasterPose)
		{
			Component->SetMasterPoseComponent(MasterComponent);
		}
	}
}

void UTGOR_Bodypart::ResetMesh(UTGOR_ChildSkeletalMeshComponent* Component)
{
	//Mesh.Mesh->SetSkeletalMesh(nullptr, false); <-- MAYBE IMPORTANT (But so far nothing broke without it)
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_Bodypart::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(ChildInsertions);
	MOV_INSERTION(SectionInsertions);
	MOV_INSERTION(SkinInsertions);
	MOV_INSERTION(MaskInsertions);
	MOV_INSERTION(DefaultPaletteInsertion);
	MOV_INSERTION(CustomisationInsertions);
}


#if WITH_EDITOR

void UTGOR_Bodypart::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (IsValid(Mesh))
	{
		UTGOR_BodypartUserData* UserData = Mesh->GetAssetUserData<UTGOR_BodypartUserData>();
		if (!IsValid(UserData))
		{
			Mesh = nullptr;

			FMessageLog BodypartCheckLog("BodypartCheck");
			//BodypartCheckLog.Error(FText::FromString("Bodypart has to have TGOR_BodypartUserData"));
			BodypartCheckLog.Notify(FText::FromString("Bodypart has to have TGOR_BodypartUserData"), EMessageSeverity::Error, true);

		}
	}
}

#endif