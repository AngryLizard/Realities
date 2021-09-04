// The Gateway of Realities: Planes of Existence.


#include "TGOR_Knowledge.h"
#include "CoreSystem/TGOR_Singleton.h"
#include "KnowledgeSystem/Content/TGOR_Unlock.h"
#include "KnowledgeSystem/Content/TGOR_Tracker.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"

#include "RealitiesUtility/Utility/TGOR_Math.h"

////////////////////////////////////////////////////////////////////////////////////////////////////


FTGOR_TrackerQuery::FTGOR_TrackerQuery()
	: Threshold(1.0f), Tracker(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////


UTGOR_Knowledge::UTGOR_Knowledge()
	: Super()
{
}

void UTGOR_Knowledge::PostBuildResource()
{
	Super::PostBuildResource();

	//Children = GetIListFromType<UTGOR_Knowledge>();
	const TArray<UTGOR_Knowledge*>& Children = Instanced_ChildInsertions.Collection;
	for (UTGOR_Knowledge* Child : Children)
	{
		Child->Parents.AddUnique(this);
	}

	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	for (FTGOR_TrackerQuery& Query : Trackers)
	{
		Query.Tracker = ContentManager->GetTFromType<UTGOR_Tracker>(Query.Type);
	}
}

void UTGOR_Knowledge::MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success)
{
	Super::MoveInsertion(Insertion, Action, Success);

	MOV_INSERTION(ChildInsertions);
	MOV_INSERTION(UnlockInsertions);
}
