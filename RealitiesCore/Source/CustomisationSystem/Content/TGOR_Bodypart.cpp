// The Gateway of Realities: Planes of Existence.


#include "TGOR_Bodypart.h"

#include "Components/SkeletalMeshComponent.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "CoreSystem/Utility/TGOR_ActorUtility.h"
#include "CustomisationSystem/Content/TGOR_Skin.h"
#include "CustomisationSystem/Content/TGOR_Mask.h"
#include "CustomisationSystem/Content/TGOR_Palette.h"
#include "CustomisationSystem/Content/TGOR_Section.h"
#include "CustomisationSystem/UserData/TGOR_BodypartUserData.h"

#include "CustomisationSystem/Components/TGOR_ModularSkeletalMeshComponent.h"


#ifdef WITH_EDITOR
#include "Logging/MessageLog.h"
#endif


UTGOR_Bodypart::UTGOR_Bodypart()
	: Super()
{
	FollowsMasterPose = true;
	IsBodypartRoot = false;
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

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_Bodypart::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

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