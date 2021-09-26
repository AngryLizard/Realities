// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "../Components/TGOR_StatComponent.h"

#include "UISystem/Queries/TGOR_ContentQuery.h"
#include "TGOR_EffectsQuery.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

////////////////////////////////////////////// STRUCT //////////////////////////////////////////////////////


USTRUCT(BlueprintType)
struct FTGOR_EffectOutput
{
	GENERATED_USTRUCT_BODY()

	/** Effect content. */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Stats")
		UTGOR_Effect* Effect = nullptr;

	/** Effect value within range in [0, 1]. */
	UPROPERTY(BlueprintReadWrite, Category = "!TGOR Stats")
		float Value = 0.0f;
};

/**
 * 
 */
UCLASS()
class SIMULATIONSYSTEM_API UTGOR_EffectsQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_EffectsQuery();
	TArray<UTGOR_CoreContent*> QueryContent() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Assigns StatComponent and filters stat states */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Stats", Meta = (Keywords = "C++"))
		virtual void AssignComponent(UTGOR_StatComponent* StatComponent);

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Stats")
		TArray<FTGOR_EffectOutput> Effects;
};
