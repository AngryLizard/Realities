// The Gateway of Realities: Planes of Existence.

#pragma once

#include "DimensionSystem/Content/TGOR_SpawnModule.h"
#include "TGOR_Stat.generated.h"

class UTGOR_StatComponent;
class UTGOR_Attribute;
class UTGOR_Response;

UENUM(BlueprintType)
enum class ETGOR_StatTypeEnumeration : uint8
{
	Primary,
	Secondary,
	Hidden
};

/**
 * 
 */
UCLASS(Blueprintable)
class SIMULATIONSYSTEM_API UTGOR_Stat : public UTGOR_SpawnModule
{
	GENERATED_BODY()
	
public:
	UTGOR_Stat();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Type of this stat (Primary is displayed as a big bar, secondary as a small bar, special only as a buff/debuff tile) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		ETGOR_StatTypeEnumeration Type;

	/** Value this stat starts with when introduced */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Initial;

	/** Update duration in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Duration;

	/** Tick function (called every stat tick) computes by how much this stat changes over time. */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Action", Meta = (Keywords = "C++"))
		float Compute(UTGOR_StatComponent* Stats, float Value, float DeltaTime) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Attributes supported by this effect */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Response>> ResponseInsertions;
	DECL_INSERTION(ResponseInsertions);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;

};
