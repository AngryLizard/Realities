// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_ChatMessageInstance.generated.h"

class UTGOR_Channel;
class UTGOR_Command;

UENUM(BlueprintType)
enum class ETGOR_ChatDomainEnumeration : uint8
{
	System,
	Private,
	Public
};

/**
* TGOR_ChatMessageInstance stores a chat message
*/
USTRUCT(BlueprintType)
struct PLAYERSYSTEM_API FTGOR_ChatMessageInstance
{
	GENERATED_USTRUCT_BODY()
	
public:
	FTGOR_ChatMessageInstance();
	
	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Domain */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ETGOR_ChatDomainEnumeration Domain;

	/** Receiver command */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Command* Command;

	/** Receiver Channel */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Channel* Channel;

	/** Receiver user */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 UserSerial;

	/** Associated name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Sender;

	/** Message text */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Message;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FTGOR_ChatMessageInstance> : public TStructOpsTypeTraitsBase2<FTGOR_ChatMessageInstance>
{
	enum
	{
		WithNetSerializer = true
	};
};