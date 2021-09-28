// The Gateway of Realities: Planes of Existence.

#include "TGOR_TrackerComponent.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "DimensionSystem/Data/TGOR_WorldData.h"
#include "KnowledgeSystem/Content/TGOR_Tracker.h"
#include "CoreSystem/Storage/TGOR_Package.h"

#include "Net/UnrealNetwork.h"

UTGOR_TrackerComponent::UTGOR_TrackerComponent()
	: Super()
{
}

bool UTGOR_TrackerComponent::PreAssemble(UTGOR_DimensionData* Dimension)
{
	Super::PreAssemble(Dimension);

	//Assemble owner and associated register components
	AActor* Actor = GetOwner();
	if (IsValid(Actor))
	{
		if (Actor->Implements<UTGOR_RegisterInterface>())
		{
			RegisterObjects.Add(Actor);
		}

		const TSet<UActorComponent*>& Components = Actor->GetComponents();
		for (UActorComponent* Component : Components)
		{
			if (Component->Implements<UTGOR_RegisterInterface>())
			{
				RegisterObjects.Add(Component);
			}
		}
	}
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////

TSet<UTGOR_CoreContent*> UTGOR_TrackerComponent::GetContext() const
{
	TSet<UTGOR_CoreContent*> ContentContext;

	// Preassemble all associated components
	for (const TScriptInterface<ITGOR_RegisterInterface>& RegisterObject : RegisterObjects)
	{
		ContentContext.Append(RegisterObject->Execute_GetActiveContent(RegisterObject.GetObject()));
	}
	return ContentContext;
}

void UTGOR_TrackerComponent::PutTracker(TSubclassOf<UTGOR_Tracker> Type, const TSet<UTGOR_CoreContent*>& Appendum, float Delta)
{
	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	UTGOR_Tracker* Tracker = ContentManager->GetTFromType<UTGOR_Tracker>(Type);
	if (IsValid(Tracker))
	{
		FTGOR_TrackerInstance& Instance = Trackers.Data.FindOrAdd(Tracker);
		TSet<UTGOR_CoreContent*> ContentContext = Appendum.Union(GetContext());
		if (ContentContext.Remove(nullptr) > 0)
		{
			RPORT(Tracker->GetName() + " inserted null context!");
		}
		Instance.Put(ContentContext, Delta);
	}
}

void UTGOR_TrackerComponent::FlushTracker(FTGOR_TrackerData& Target)
{
	Target.Merge(Trackers);
	Trackers.Data.Reset();
}