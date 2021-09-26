// The Gateway of Realities: Planes of Existence.


#include "TGOR_UnlockComponent.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "RealitiesUGC/Mod/TGOR_ContentManager.h"
#include "KnowledgeSystem/Content/TGOR_Unlock.h"
#include "KnowledgeSystem/Content/TGOR_Knowledge.h"
#include "KnowledgeSystem/Content/TGOR_RootKnowledge.h"
#include "CoreSystem/Utility/TGOR_CoreEnumerations.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

UTGOR_UnlockComponent::UTGOR_UnlockComponent()
	: Super()
{
}

void UTGOR_UnlockComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UTGOR_UnlockComponent, PinBoard, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UTGOR_UnlockComponent, Pending, COND_OwnerOnly);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_UnlockComponent::RepNotifyPinBoard()
{
}

void UTGOR_UnlockComponent::RepNotifyPending(const FTGOR_KnowledgeCollection& Old)
{
	TSet<UTGOR_Knowledge*> Unlocks;

	// Go from root if empty
	TArray<UTGOR_Knowledge*> Queue = Old.Data.Array();
	if (Queue.Num() == 0)
	{
		SINGLETON_CHK;
		UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
		Queue.Append(ContentManager->GetTListFromType<UTGOR_Knowledge>(UTGOR_RootKnowledge::StaticClass()));
	}

	// Diff with old pending list to see which knowledges got newly unlocked.
	while (Queue.Num() > 0)
	{
		UTGOR_Knowledge* Knowledge = Queue.Pop();
		if (!Pending.Data.Contains(Knowledge))
		{
			// If knowledge isn't pending anymore we must have unlocked it!
			Unlocks.Add(Knowledge);
			Unlocked.Add(Knowledge);

			// Check children in case even more stuff got unlocked.
			const TArray<UTGOR_Knowledge*>& Children = Knowledge->Instanced_ChildInsertions.Collection;
			Queue.Append(Children);
		}
	}

	AchievementNotify(Unlocks);
}

void UTGOR_UnlockComponent::ClearKnowledgeTree()
{
	// Clear trackers
	Pending.Data.Reset();
	Unlocked.Reset();
	Achievements.Reset();
}

void UTGOR_UnlockComponent::ResetKnowledgeTree(const FTGOR_TrackerData& Trackers)
{
	ClearKnowledgeTree();

	// Add root nodes
	SINGLETON_CHK;
	UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
	Pending.Data.Append(ContentManager->GetTListFromType<UTGOR_Knowledge>(UTGOR_RootKnowledge::StaticClass()));

	// Fast forward through achievements
	while (CheckPendingKnowledges(Trackers)) {}
}

bool UTGOR_UnlockComponent::CheckKnowledge(const FTGOR_TrackerData& Trackers, UTGOR_Knowledge* Knowledge) const
{
	for (const FTGOR_TrackerQuery& Query : Knowledge->Trackers)
	{
		if (const FTGOR_TrackerInstance* Ptr = Trackers.Data.Find(Query.Tracker))
		{
			const float Value = Ptr->Count(Query.Filter);
			if (Value < Query.Threshold)
			{
				return false;
			}
		}
		else if (Query.Threshold > 0.0f)
		{
			return false;
		}
	}
	return true;
}


bool UTGOR_UnlockComponent::CheckPendingKnowledges(const FTGOR_TrackerData& Trackers)
{
	// Track whether we have unlocked anything new
	bool HasUnlocked = false;
	TSet<UTGOR_Knowledge*> Unlocks;

	// Check all pending knowledges
	for (auto Itr = Pending.Data.CreateIterator(); Itr; ++Itr)
	{
		if (CheckKnowledge(Trackers, *Itr))
		{
			// Add to unlocked list
			Unlocks.Add(*Itr);
			Unlocked.Add(*Itr);

			// Set children pending
			Pending.Data.Append((*Itr)->Instanced_ChildInsertions.Collection);

			// Remove from pending
			Itr.RemoveCurrent();
			HasUnlocked = true;
		}
	}

	if (HasUnlocked)
	{
		AchievementNotify(Unlocks);
		return true;
	}
	return false;
}

void UTGOR_UnlockComponent::UpdatePinboard(const FTGOR_TrackerData& Trackers)
{
	// Check all pinned knowledges
	if (IsValid(PinBoard.Knowledge))
	{
		// We only track pending knowledges.
		if (IsRelevant(PinBoard.Knowledge))
		{
			// Check tracker status and set pin value
			const int32 Num = PinBoard.Knowledge->Trackers.Num();
			PinBoard.Progress.SetNumZeroed(Num);
			for (int32 Index = 0; Index < Num; Index++)
			{
				const FTGOR_TrackerQuery& Query = PinBoard.Knowledge->Trackers[Index];
				if (const FTGOR_TrackerInstance* Ptr = Trackers.Data.Find(Query.Tracker))
				{
					PinBoard.Progress[Index] = Ptr->Count(Query.Filter);
				}
				else
				{
					PinBoard.Progress[Index] = 0;
				}
			}
		}
		else
		{
			PinBoard.Knowledge = nullptr;
		}
	}
}

TArray<float> UTGOR_UnlockComponent::GetKnowledgePin(UTGOR_Knowledge* Knowledge, ETGOR_FetchEnumeration& Branches)
{
	Branches = ETGOR_FetchEnumeration::Empty;
	if (PinBoard.Knowledge == Knowledge)
	{
		Branches = ETGOR_FetchEnumeration::Found;
		return PinBoard.Progress;
	}
	return TArray<float>();
}


void UTGOR_UnlockComponent::RequestPin(const FTGOR_TrackerData& Trackers, const FTGOR_KnowledgePinRequest& Request)
{
	if (IsValid(Request.Knowledge) && IsRelevant(Request.Knowledge))
	{
		PinBoard.Knowledge = Request.Knowledge;
		UpdatePinboard(Trackers );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_UnlockComponent::IsUnlocked(UTGOR_Knowledge* Knowledge) const
{
	return Unlocked.Contains(Knowledge);
}

bool UTGOR_UnlockComponent::IsPending(UTGOR_Knowledge* Knowledge) const
{
	return Pending.Data.Contains(Knowledge);
}

bool UTGOR_UnlockComponent::IsRelevant(UTGOR_Knowledge* Knowledge) const
{
	if (IsPending(Knowledge))
	{
		return true;
	}
	return IsUnlocked(Knowledge);
}

bool UTGOR_UnlockComponent::WasJustUnlocked(UTGOR_Knowledge* Knowledge) const
{
	return Achievements.Contains(Knowledge);
}

TArray<UTGOR_Knowledge*> UTGOR_UnlockComponent::FlushAchievements()
{
	TArray<UTGOR_Knowledge*> Unlocks = Achievements.Array();
	Achievements.Reset();
	return Unlocks;
}

void UTGOR_UnlockComponent::AchievementNotify(const TSet<UTGOR_Knowledge*>& Unlocks)
{
	Achievements.Append(Unlocks);
	OnAchievement.Broadcast(Unlocks);
}

bool UTGOR_UnlockComponent::ContainsUnlock(UTGOR_Unlock* Unlock) const
{
	for (UTGOR_Knowledge* Knowledge : Unlocked)
	{
		if (Knowledge->Instanced_UnlockInsertions.Contains(Unlock))// ContainsInsertion(Unlock))
		{
			return true;
		}
	}
	return false;
}

bool UTGOR_UnlockComponent::ContainsUnlockFromType(TSubclassOf<UTGOR_Unlock> Type) const
{
	for (UTGOR_Knowledge* Knowledge : Unlocked)
	{
		if (Knowledge->Instanced_UnlockInsertions.ContainsOfType(Type)) //ContainsInsertionFromType(Type))
		{
			return true;
		}
	}
	return false;
}

TArray<UTGOR_Unlock*> UTGOR_UnlockComponent::FilterUnlock(TArray<UTGOR_Unlock*> List) const
{
	TArray<UTGOR_Unlock*> Out;
	for (UTGOR_Unlock* Unlock : List)
	{
		if (ContainsUnlock(Unlock))
		{
			Out.Emplace(Unlock);
		}
	}
	return Out;
}

UTGOR_Unlock* UTGOR_UnlockComponent::GetUnlockedFromType(TSubclassOf<UTGOR_Unlock> Type) const
{
	for (UTGOR_Knowledge* Knowledge : Unlocked)
	{
		UTGOR_Unlock* Unlock = Knowledge->Instanced_UnlockInsertions.GetOfType(Type); //GetInsertionFromType(Type);
		if (IsValid(Unlock))
		{
			return Unlock;
		}
	}
	return nullptr;
}

TArray<UTGOR_Unlock*> UTGOR_UnlockComponent::GetUnlockedListFromType(TSubclassOf<UTGOR_Unlock> Type) const
{
	TArray<UTGOR_Unlock*> List;
	for (UTGOR_Knowledge* Knowledge : Unlocked)
	{
		List.Append(Knowledge->Instanced_UnlockInsertions.GetListOfType(Type));//GetInsertionListFromType(Type));
	}
	return List;
}
