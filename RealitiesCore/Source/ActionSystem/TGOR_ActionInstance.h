// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Components/ActorComponent.h"

#include "RealitiesUtility/Structures/TGOR_Time.h"

#include "CoreSystem/Storage/TGOR_PackageGroup.h"
#include "CoreSystem/Storage/TGOR_PackageNetwork.h"
#include "CoreSystem/Storage/TGOR_Serialisation.h"

#include "TargetSystem/TGOR_AimInstance.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_ActionInstance.generated.h"

//////////////////////////////////////////// LOGGING /////////////////////////////////////////////////////

DECLARE_LOG_CATEGORY_EXTERN(TGOR_ActionLogs, Log, All);

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Input;
class UTGOR_Action;
class UTGOR_Action;
class UTGOR_Loadout;

////////////////////////////////////////////// ENUMS //////////////////////////////////////////////////////

UENUM(BlueprintType)
enum class ETGOR_ActionStateEnumeration : uint8
{
	Suspended,
	Prepare,
	Operate,
	Finish,
	Dead
};


/**
* TGOR_ActionInputs stores the inputs for a given action
*/
USTRUCT(BlueprintType)
struct ACTIONSYSTEM_API FTGOR_ActionInputs
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;

	FTGOR_ActionInputs();
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	/** Active inputs */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<UTGOR_Input*, float> Inputs;
};

template<>
struct TStructOpsTypeTraits<FTGOR_ActionInputs> : public TStructOpsTypeTraitsBase2<FTGOR_ActionInputs>
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
struct ACTIONSYSTEM_API FTGOR_ActionState
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;

	FTGOR_ActionState();
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	/** Current target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_AimInstance Aim;

	/** Currently active slot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ActiveSlot;

	/** Schedule time of last state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_Time Time;

	/** Current state of the action. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ETGOR_ActionStateEnumeration State;
};

template<>
struct TStructOpsTypeTraits<FTGOR_ActionState> : public TStructOpsTypeTraitsBase2<FTGOR_ActionState>
{
	enum
	{
		WithNetSerializer = true
	};
};



/**
* TGOR_CreatureActionInstance stores character action information
*/
USTRUCT(BlueprintType)
struct ACTIONSYSTEM_API FTGOR_LoadoutInstance
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;

	FTGOR_LoadoutInstance();

	CTGOR_GroupPackageCache LegacyCache;
	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Action")
		UTGOR_Loadout* Loadout;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "!TGOR Action")
		TArray<UTGOR_Action*> Actions;
};

template<>
struct TStructOpsTypeTraits<FTGOR_LoadoutInstance> : public TStructOpsTypeTraitsBase2<FTGOR_LoadoutInstance>
{
	enum
	{
		WithNetSerializer = true
	};
};


/**
*
*/
USTRUCT(BlueprintType)
struct ACTIONSYSTEM_API FTGOR_ActionLog
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Domain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Message;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_Time Timestamp;
};

/**
* 
*/
USTRUCT(BlueprintType)
struct ACTIONSYSTEM_API FTGOR_ActionDebugInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Action* Action;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<UTGOR_Input*, float> Inputs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_AimInstance Aim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FString> Logs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool HasNoCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool HasValidTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool HasValidMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsInRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool HasCondition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsRunning;
};