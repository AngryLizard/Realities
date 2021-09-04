#pragma once

#include "CoreMinimal.h"

#include "CoreSystem/Storage/TGOR_PackageGroup.h"
#include "CoreSystem/Storage/TGOR_PackageNetwork.h"
#include "CoreSystem/Storage/TGOR_Serialisation.h"

#include "TGOR_ProcessInstance.generated.h"

class UTGOR_Module;
class UTGOR_Process;
class UTGOR_ProcessStorage;
class UTGOR_DimensionComponent;

/**
 * The instance of an item. Contains the serializable data to make the item individual.
 */
USTRUCT(BlueprintType, Blueprintable)
struct INVENTORYSYSTEM_API FTGOR_ProcessInstance
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
	bool operator==(const FTGOR_ProcessInstance& Other) const; // Compares storage pointer and serial
	bool Update(UTGOR_DimensionComponent* Owner, float DeltaSeconds); // Updates storage (tick and serial increment for replication), return true if something changed
	void Post(); // Post-process storage, done after update and side-effects are handled

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, DisplayName = "ProcessStorage", Category = "Inventory")
		UTGOR_ProcessStorage* Storage;

	/** Serial to promote replication. TODO: Replace with custom NetDelta implementation */
	UPROPERTY()
		uint16 Serial;

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
