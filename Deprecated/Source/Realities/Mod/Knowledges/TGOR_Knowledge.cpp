// The Gateway of Realities: Planes of Existence.


#include "TGOR_Knowledge.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Mod/Trackers/TGOR_Tracker.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"

#include "Realities/Utility/TGOR_Math.h"

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

	Children = GetIListFromType<UTGOR_Knowledge>();
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
