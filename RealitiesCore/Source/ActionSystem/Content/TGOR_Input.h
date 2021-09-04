// The Gateway of Realities: Planes of Existence.

#pragma once

#include "RealitiesUtility/Structures/TGOR_Normal.h"



#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_Input.generated.h"

UENUM(BlueprintType)
enum class ETGOR_InputRepEnumeration : uint8
{
	Local,
	Binary,
	Gradual
};

/**
 * 
 */
UCLASS()
class ACTIONSYSTEM_API UTGOR_Input : public UTGOR_CoreContent
{
	GENERATED_BODY()
	
public:
	UTGOR_Input();

	/** Whether this event is relevant for replication. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		bool IsReplicated() const;

	/** Whether this event is replicated as an on/off switch. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		bool IsBinary() const;

	/** Whether given value is above the threshold. */
	UFUNCTION(BlueprintPure, Category = "!TGOR Storage", Meta = (Keywords = "C++"))
		bool IsActive(const FTGOR_Normal& Value) const;

protected:

	/** Whether this input is replicated. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Storage")
		ETGOR_InputRepEnumeration Replication;

	/** Trigger threshold. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Storage", meta = (ClampMin = "0.0", ClampMax = "1.0"))
		float Threshold;
};
