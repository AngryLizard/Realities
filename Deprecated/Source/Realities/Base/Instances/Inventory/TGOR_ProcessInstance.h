#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Storage/TGOR_PackageGroup.h"
#include "Realities/Utility/Storage/TGOR_PackageNetwork.h"

#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "TGOR_ProcessInstance.generated.h"

class UTGOR_Module;
class UTGOR_Process;
class UTGOR_ProcessStorage;

/**
 * The instance of an item. Contains the serializable data to make the item individual.
 */
USTRUCT(BlueprintType, Blueprintable)
struct REALITIES_API FTGOR_ProcessInstance
{
	GENERATED_BODY()
	SERIALISE_INIT_HEADER;

public:

	FTGOR_ProcessInstance();
	FTGOR_ProcessInstance(UTGOR_Process* NewProcess);
	FTGOR_ProcessInstance(UTGOR_ProcessStorage* NewStorage);

	CTGOR_GroupPackageCache LegacyCache;
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	bool operator<(const FTGOR_ProcessInstance& Other) const; // Compares held item storage index (for weak ordering)
	bool operator==(const FTGOR_ProcessInstance& Other) const; // Compares item storages for equality

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, DisplayName = "ProcessStorage", Category = "Inventory")
		UTGOR_ProcessStorage* Storage;

	/** Instantiate storage from given content. */
	void SetupStorage(UTGOR_Process* NewProcess);
};

template<>
struct TStructOpsTypeTraits<FTGOR_ProcessInstance> : public TStructOpsTypeTraitsBase2<FTGOR_ProcessInstance>
{
	enum
	{
		WithNetSerializer = true
	};
};
