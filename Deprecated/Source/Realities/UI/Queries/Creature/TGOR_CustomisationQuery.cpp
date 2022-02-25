// The Gateway of Realities: Planes of Existence.


#include "TGOR_CustomisationQuery.h"
#include "Realities/Mod/Spawner/Creatures/TGOR_Creature.h"
#include "Realities/Components/Creature/TGOR_ModularSkeletalMeshComponent.h"
#include "Realities/Base/Creature/TGOR_BodypartNode.h"

UTGOR_CustomisationQuery::UTGOR_CustomisationQuery()
	: Super()
{

}

UTGOR_BodypartNode* UTGOR_CustomisationQuery::CreatureSetup()
{
	UTGOR_ModularSkeletalMeshComponent* SkeletalMesh = GetSkeletalMesh();
	if (!IsValid(SkeletalMesh))
	{
		ERRET("No Character mesh defined", Error, nullptr)
	}

	UTGOR_Creature* Creature = GetCreature();
	if (!IsValid(Creature))
	{
		ERRET("No Creature class defined", Error, nullptr)
	}

	UTGOR_BodypartNode* NodeRoot = SkeletalMesh->GetAppearanceRoot();
	if (!IsValid(NodeRoot))
	{
		AppearanceCache.Bodyparts.Empty();
		SkeletalMesh->ApplyAppearance(AppearanceCache, Creature);
		NodeRoot = SkeletalMesh->GetAppearanceRoot();
	}
	else
	{
		AppearanceCache = SkeletalMesh->ConstructCurrentAppearance();
	}
	return NodeRoot;
}

void UTGOR_CustomisationQuery::ApplyCached()
{
	UTGOR_Creature* Creature = GetCreature();
	if (IsValid(Creature))
	{
		ApplyAppearance(AppearanceCache, Creature, true);
	}
}

void UTGOR_CustomisationQuery::CacheCurrent()
{
	UTGOR_Creature* Creature = GetCreature();
	UTGOR_ModularSkeletalMeshComponent* SkeletalMesh = GetSkeletalMesh();
	if (IsValid(Creature) && IsValid(SkeletalMesh))
	{
		AppearanceCache = SkeletalMesh->ConstructCurrentAppearance();
		ApplyAppearance(AppearanceCache, Creature, false);
	}

}