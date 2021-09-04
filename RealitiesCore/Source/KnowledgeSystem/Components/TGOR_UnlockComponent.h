// The Gateway of Realities: Planes of Existence.

#pragma once
#include "CoreMinimal.h"
#include "KnowledgeSystem/TGOR_TrackerInstance.h"
#include "KnowledgeSystem/TGOR_KnowledgeInstance.h"

#include "CoreSystem/Components/TGOR_Component.h"
#include "TGOR_UnlockComponent.generated.h"


//////////////////////////////////////////// DELEGATES /////////////////////////////////////////////////

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAchievementDelegate, const TSet<UTGOR_Knowledge*>&, Unlocks);

/**
* TGOR_UnlockComponent stored knowledge tree unlocks
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class KNOWLEDGESYSTEM_API UTGOR_UnlockComponent : public UTGOR_Component
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_UnlockComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Online")
		FAchievementDelegate OnAchievement;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Knowledge that is tracked by the player */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = RepNotifyPinBoard)
		FTGOR_KnowledgePin PinBoard;

	/** Knowledges that have just been unlocked */
	UPROPERTY(BlueprintReadOnly)
		TSet<UTGOR_Knowledge*> Achievements;

	/** Pending knowledge nodes */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = RepNotifyPending)
		FTGOR_KnowledgeCollection Pending;

	/** Unlocked knowledge nodes */
	UPROPERTY(BlueprintReadOnly)
		TSet<UTGOR_Knowledge*> Unlocked;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	UFUNCTION()
		void RepNotifyPinBoard();

	UFUNCTION()
		void RepNotifyPending(const FTGOR_KnowledgeCollection& Old);

	/** Clears all tree nodes and inserts roots */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Online|Internal", Meta = (Keywords = "C++"))
		void ClearKnowledgeTree();

	/** Resets all tree nodes and inserts roots */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Online|Internal", Meta = (Keywords = "C++"))
		void ResetKnowledgeTree(const FTGOR_TrackerData& Trackers);

	/** Checks pending knowledges for completion, returns true if unlocks changed */
	UFUNCTION(BlueprintPure, BlueprintAuthorityOnly, Category = "!TGOR Online|Internal", Meta = (Keywords = "C++"))
		bool CheckKnowledge(const FTGOR_TrackerData& Trackers, UTGOR_Knowledge* Knowledge) const;

	/** Checks pending knowledges for completion, returns true if unlocks changed */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Online|Internal", Meta = (Keywords = "C++"))
		bool CheckPendingKnowledges(const FTGOR_TrackerData& Trackers);

	/** Update pinboard with tracker info and removes unlocked entries */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Online|Internal", Meta = (Keywords = "C++"))
		void UpdatePinboard(const FTGOR_TrackerData& Trackers);

	/** Get pinboard entries if available, returns empty list otherwise */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Online", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		TArray<float> GetKnowledgePin(UTGOR_Knowledge* Knowledge, ETGOR_FetchEnumeration& Branches);

	/** Request switching knowledge tracker */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Online", Meta = (Keywords = "C++"))
		void RequestPin(const FTGOR_TrackerData& Trackers, const FTGOR_KnowledgePinRequest& Request);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Returns whether given knowledge has all predicates fulfilled and is unlocked */
	UFUNCTION(BlueprintPure, Category = "!TGOR Online", Meta = (Keywords = "C++"))
		bool IsUnlocked(UTGOR_Knowledge* Knowledge) const;

	/** Returns whether given knowledge is pending to be unlocked if predicates are fulfilled */
	UFUNCTION(BlueprintPure, Category = "!TGOR Online", Meta = (Keywords = "C++"))
		bool IsPending(UTGOR_Knowledge* Knowledge) const;

	/** Returns whether given knowledge is pending or unlocked */
	UFUNCTION(BlueprintPure, Category = "!TGOR Online", Meta = (Keywords = "C++"))
		bool IsRelevant(UTGOR_Knowledge* Knowledge) const;

	/** Returns whether given knowledge was just unlocked unlocked */
	UFUNCTION(BlueprintPure, Category = "!TGOR Online", Meta = (Keywords = "C++"))
		bool WasJustUnlocked(UTGOR_Knowledge* Knowledge) const;

	/** Returns and resets all recently unlocked knowledge nodes */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Online|Internal", Meta = (Keywords = "C++"))
		TArray<UTGOR_Knowledge*> FlushAchievements();

	/** Notifies player about newly unlocked knowledge nodes */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Online|Internal", Meta = (Keywords = "C++"))
		void AchievementNotify(const TSet<UTGOR_Knowledge*>& Unlocks);

	/** Figure out whether a given content is unlocked */
	UFUNCTION(BlueprintPure, Category = "!TGOR Content", meta = (Keywords = "C++"))
		bool ContainsUnlock(UTGOR_Unlock* Unlock) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	template<typename T>
	bool ContainsU(T* Unlock) const
	{
		return(ContainsUnlock(Unlock));
	}

	/** Figure out whether a given content type is unlocked */
	UFUNCTION(BlueprintPure, Category = "!TGOR Content", meta = (Keywords = "C++"))
		bool ContainsUnlockFromType(TSubclassOf<UTGOR_Unlock> Type) const;

	template<typename T>
	bool ContainsUFromType(TSubclassOf<T> Type) const
	{
		return(ContainsUnlockFromType(Type));
	}

	template<typename T>
	bool ContainsUFromType() const
	{
		return(ContainsUFromType<T>(T::StaticClass()));
	}

	/** Filters unlocked content from a list of contents */
	UFUNCTION(BlueprintPure, Category = "!TGOR Content", meta = (Keywords = "C++"))
		TArray<UTGOR_Unlock*> FilterUnlock(TArray<UTGOR_Unlock*> List) const;

	template<typename T>
	TArray<T*> FilterU(TArray<T*> List) const
	{
		TArray<T*> Out;
		for (T* Unlock : List)
		{
			if (ContainsU(Unlock))
			{
				Out.Emplace(Unlock);
			}
		}
		return Out;
	}

	/** Returns first unlocked content matching type */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Content", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		UTGOR_Unlock* GetUnlockedFromType(TSubclassOf<UTGOR_Unlock> Type) const;

	template<typename T>
	T* GetUFromType(TSubclassOf<T> Type) const
	{
		return(Cast<T>(GetUnlockedFromType(Type)));
	}

	template<typename T>
	T* GetUFromType() const
	{
		return(GetUFromType<T>(T::StaticClass()));
	}

	/** Returns all unlocked content matching type */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Content", Meta = (DeterminesOutputType = "Type", Keywords = "C++"))
		TArray<UTGOR_Unlock*> GetUnlockedListFromType(TSubclassOf<UTGOR_Unlock> Type) const;

	template<typename T>
	TArray<T*> GetUListFromType(TSubclassOf<T> Type) const
	{
		TArray<T*> Out;
		TArray<UTGOR_Unlock*> List = GetUnlockedListFromType(Type);
		for (UTGOR_Unlock* Unlock : List)
		{
			Out.Emplace(Cast<T>(Unlock));
		}
		return(Out);
	}

	template<typename T>
	TArray<T*> GetUListFromType() const
	{
		return(GetUListFromType<T>(T::StaticClass()));
	}



	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:

};
