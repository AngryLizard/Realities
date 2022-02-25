// The Gateway of Realities: Planes of Existence.


#include "TGOR_Bodypart.h"

#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Utility/TGOR_ActorUtility.h"
#include "Components/SkeletalMeshComponent.h"
#include "Realities/Mod/Customisations/TGOR_Customisation.h"
#include "Realities/Mod/Colours/TGOR_Colour.h"
#include "Realities/Mod/Skins/TGOR_Skin.h"


UTGOR_Bodypart::UTGOR_Bodypart()
	: Super()
{
	FollowsMasterPose = true;
}

void UTGOR_Bodypart::PostBuildResource()
{
	Super::PostBuildResource();
}


void UTGOR_Bodypart::OverrideMesh(USkeletalMeshComponent* Component)
{
	// Set component mesh
	Component->SetSkeletalMesh(Mesh, false);
	
	// Override all materials
	UTGOR_ActorUtility::OverrideMesh(Component, MaterialOverride);

	//SINGLETON_CHK
	//Singleton->ContentManager->GetTListFromType<UTGOR_Bodypart>();
	//GetIListFromType<UTGOR_Bodypart>(UTGOR_Bodypart::StaticClass());
}

void UTGOR_Bodypart::AttachMesh(USkeletalMeshComponent* Component, USkeletalMeshComponent* MasterComponent)
{
	Component->AttachToComponent(MasterComponent, FAttachmentTransformRules::KeepRelativeTransform);
	Component->SetMasterPoseComponent(MasterComponent);
}

void UTGOR_Bodypart::ResetMesh(USkeletalMeshComponent* Component)
{
	//Mesh.Mesh->SetSkeletalMesh(nullptr, false); <-- MAYBE IMPORTANT (But so far nothing broke without it)
}