// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "UISystem/Queries/TGOR_ContentQuery.h"
#include "TGOR_ContributionQuery.generated.h"

class UTGOR_Attribute;
class UTGOR_Modifier;


/**
*
*/
USTRUCT(BlueprintType)
struct SIMULATIONSYSTEM_API FTGOR_ModifierFloatPair
{
	GENERATED_USTRUCT_BODY()

		bool operator<(const FTGOR_ModifierFloatPair& Other) const;

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Attributes")
		UTGOR_Modifier* Content = nullptr;

	/** */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Attributes")
		float Value = 0.0f;

};

/**
 *
 */
UCLASS()
class SIMULATIONSYSTEM_API UTGOR_ContributionQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()

public:
	UTGOR_ContributionQuery();
	virtual TArray<UTGOR_CoreContent*> QueryContent() const override;

	/** Construct matter list from a map */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Attributes", Meta = (Keywords = "C++"))
		void AssignMap(const TMap<UTGOR_Modifier*, float>& Map, UTGOR_Attribute* Attribute);

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Attributes")
		TArray<FTGOR_ModifierFloatPair> Instances;

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Attributes")
		UTGOR_Attribute* Owner = nullptr;
};
