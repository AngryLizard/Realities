// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Realities/Utility/Datastructures/TGOR_Time.h"
#include "Realities/Utility/Datastructures/TGOR_Normal.h"
#include "Realities/Base/Instances/System/TGOR_IndexInstance.h"
#include "Realities/Base/Instances/Inventory/TGOR_ItemInstance.h"
#include "Components/ActorComponent.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_ActionInstance.generated.h"

class UTGOR_Action;
class UTGOR_ActionStorage;
class UTGOR_ActionComponent;


UENUM(BlueprintType)
enum class ETGOR_ActionStateEnumeration : uint8
{
	Suspended,
	Equip,
	Operate,
	Unequip,
	Dead
};


/**
 * The instance of an action. Contains the serializable data.
 */
USTRUCT(BlueprintType, Blueprintable)
struct REALITIES_API FTGOR_ActionInstance
{
	GENERATED_BODY()
		SERIALISE_INIT_HEADER;

public:

	FTGOR_ActionInstance();
	FTGOR_ActionInstance(UTGOR_Action* Item);
	FTGOR_ActionInstance(UTGOR_ActionStorage* NewStorage);

	CTGOR_GroupPackageCache LegacyCache;
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	bool operator<(const FTGOR_ActionInstance& Other) const; // Compares held item storage index (for weak ordering)
	bool operator==(const FTGOR_ActionInstance& Other) const; // Compares item storages for equality

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TGOR Storage")
		UTGOR_ActionStorage* Storage;

	/** Instantiate storage from given content. */
	void SetupStorage(UTGOR_Action* NewItem);
};

template<>
struct TStructOpsTypeTraits<FTGOR_ActionInstance> : public TStructOpsTypeTraitsBase2<FTGOR_ActionInstance>
{
	enum
	{
		WithNetSerializer = true
	};
};


/**
 * The instance of an action. Contains the serializable data.
 */
USTRUCT(BlueprintType, Blueprintable)
struct REALITIES_API FTGOR_ActionPayload
{
	GENERATED_BODY()
		SERIALISE_INIT_HEADER;

public:

	FTGOR_ActionPayload();
	FTGOR_ActionPayload(UTGOR_Action* Item);
	FTGOR_ActionPayload(UTGOR_ActionStorage* NewStorage);

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TGOR Storage")
		UTGOR_ActionStorage* Storage;

	/** Instantiate storage from given content. */
	void SetupStorage(UTGOR_Action* NewItem);
};

template<>
struct TStructOpsTypeTraits<FTGOR_ActionPayload> : public TStructOpsTypeTraitsBase2<FTGOR_ActionPayload>
{
	enum
	{
		WithNetSerializer = true
	};
};




/**
* TGOR_ActionState stores an action trigger with additional content information for simulated proxies
*/
USTRUCT(BlueprintType)
struct FTGOR_ActionState
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;

	FTGOR_ActionState();
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	/** Action storage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_ActionInstance Action;

	/** Currently active item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_ItemInstance Item;
};

template<>
struct TStructOpsTypeTraits<FTGOR_ActionState> : public TStructOpsTypeTraitsBase2<FTGOR_ActionState>
{
	enum
	{
		WithNetSerializer = true
	};
};