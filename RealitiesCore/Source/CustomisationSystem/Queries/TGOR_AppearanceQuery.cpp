// The Gateway of Realities: Planes of Existence.


#include "TGOR_AppearanceQuery.h"
#include "CustomisationSystem/Content/TGOR_Modular.h"
#include "CustomisationSystem/Components/TGOR_ModularSkeletalMeshComponent.h"

UTGOR_AppearanceQuery::UTGOR_AppearanceQuery()
	: Super()
{

}

void UTGOR_AppearanceQuery::CreatureSetup()
{
	UTGOR_ModularSkeletalMeshComponent* SkeletalMesh = GetSkeletalMesh();
	if (!IsValid(SkeletalMesh))
	{
		ERROR("No Character mesh defined", Error)
	}

	UTGOR_Modular* Modular = GetModular();
	if (!IsValid(Modular))
	{
		ERROR("No Modular class defined", Error)
	}

	//@TODO: AppearanceCache = SkeletalMesh->CurrentAppearance;
}

void UTGOR_AppearanceQuery::ApplyCached()
{
	UTGOR_Modular* Modular = GetModular();
	if (IsValid(Modular))
	{
		ApplyAppearance(AppearanceCache, Modular, true);
	}
}

void UTGOR_AppearanceQuery::CacheCurrent()
{
	UTGOR_Modular* Modular = GetModular();
	UTGOR_ModularSkeletalMeshComponent* SkeletalMesh = GetSkeletalMesh();
	if (IsValid(Modular) && IsValid(SkeletalMesh))
	{
		//@TODO: AppearanceCache = SkeletalMesh->CurrentAppearance;
		ApplyAppearance(AppearanceCache, Modular, false);
	}

}