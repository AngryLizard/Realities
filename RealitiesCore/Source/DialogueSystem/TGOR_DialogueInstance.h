// The Gateway of Realities: Planes of Existence.

#pragma once

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

/**
* FTGOR_DialogueState
*/
USTRUCT(BlueprintType)
struct DIALOGUESYSTEM_API FTGOR_DialogueState
{
	GENERATED_USTRUCT_BODY()
		SERIALISE_INIT_HEADER;

	FTGOR_DialogueState();
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	/** Currently active slot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ActiveSlot;
};

template<>
struct TStructOpsTypeTraits<FTGOR_DialogueState> : public TStructOpsTypeTraitsBase2<FTGOR_DialogueState>
{
	enum
	{
		WithNetSerializer = true
	};
};