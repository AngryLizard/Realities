// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreSystem/Storage/TGOR_PackageGroup.h"

#include "TGOR_TrackerInstance.generated.h"

class UTGOR_Tracker;
class UTGOR_CoreContent;


/**
*
*/
USTRUCT(BlueprintType)
struct KNOWLEDGESYSTEM_API FTGOR_TrackerEntry
{
	GENERATED_USTRUCT_BODY()

public:
	FTGOR_TrackerEntry();
	void Merge(TArray<FTGOR_TrackerEntry>& Entries, const TArray<FTGOR_TrackerEntry>& Other, const FTGOR_TrackerEntry& Entry);

	CTGOR_GroupPackageCache LegacyCache;
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Counter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<UTGOR_CoreContent*, int32> Subs;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	float Count(const TArray<FTGOR_TrackerEntry>& Entries, const TArray<TSubclassOf<UTGOR_CoreContent>>& Filter, int32 Count) const;
	void Put(TArray<FTGOR_TrackerEntry>& Entries, const TArray<UTGOR_CoreContent*>& Registry, int32 Index, float Delta);
};

/**
* TGOR_TrackerInstance stores tracker history
*/
USTRUCT(BlueprintType)
struct KNOWLEDGESYSTEM_API FTGOR_TrackerInstance
{
	GENERATED_USTRUCT_BODY()

		FTGOR_TrackerInstance();
	void Merge(const FTGOR_TrackerInstance& Other);

	CTGOR_GroupPackageCache LegacyCache;
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Root entry for counter management */
	UPROPERTY(BlueprintReadOnly)
		FTGOR_TrackerEntry Root;

	/** List of entries since we can't have map recursion */
	UPROPERTY(BlueprintReadOnly)
		TArray<FTGOR_TrackerEntry> Entries;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

		float Count(const TArray<TSubclassOf<UTGOR_CoreContent>>& Filter) const;
		float Count() const;

		void Put(const TSet<UTGOR_CoreContent*>& Registry, float Delta = 1.0f);
		void Put(float Delta = 1.0f);
};

/**
* Stores multiple trackers
*/
USTRUCT(BlueprintType)
struct KNOWLEDGESYSTEM_API FTGOR_TrackerData
{
	GENERATED_USTRUCT_BODY()

		FTGOR_TrackerData();
	void Merge(const FTGOR_TrackerData& Other);

	CTGOR_GroupPackageCache LegacyCache;
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Stored trackers */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Pawn", meta = (Keywords = "C++"))
		TMap<UTGOR_Tracker*, FTGOR_TrackerInstance> Data;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

};