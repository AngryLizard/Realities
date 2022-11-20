// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Coresystem/Storage/TGOR_Serialisation.h"
#include "Components/ActorComponent.h"

#include "RealitiesUtility/Structures/TGOR_Time.h"

#include "CoreSystem/Storage/TGOR_PackageGroup.h"
#include "CoreSystem/Storage/TGOR_PackageNetwork.h"
#include "CoreSystem/Storage/TGOR_Serialisation.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_DialogueInstance.generated.h"

//////////////////////////////////////////// LOGGING /////////////////////////////////////////////////////

DECLARE_LOG_CATEGORY_EXTERN(TGOR_DialogueLogs, Log, All);

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_Dialogue;
class UTGOR_Activator;
class UTGOR_Participant;

/**
* FTGOR_SpectacleConfiguration
*/
USTRUCT(BlueprintType)
struct DIALOGUESYSTEM_API FTGOR_SpectacleConfiguration
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER();

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	/** Currently active participant designation */
	TMap<UTGOR_Participant*, TWeakObjectPtr<AActor>> Participants;
};

template<>
struct TStructOpsTypeTraits<FTGOR_SpectacleConfiguration> : public TStructOpsTypeTraitsBase2<FTGOR_SpectacleConfiguration>
{
	enum
	{
		WithNetSerializer = true
	};
};


/**
* FTGOR_DialogueState
*/
USTRUCT(BlueprintType)
struct DIALOGUESYSTEM_API FTGOR_SpectacleState
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER();

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	/** Currently active slot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ActiveSlot = INDEX_NONE;

	/** Currently active configuration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_SpectacleConfiguration Configuration;
};

template<>
struct TStructOpsTypeTraits<FTGOR_SpectacleState> : public TStructOpsTypeTraitsBase2<FTGOR_SpectacleState>
{
	enum
	{
		WithNetSerializer = true
	};
};
