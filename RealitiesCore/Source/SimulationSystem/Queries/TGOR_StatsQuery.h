// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "../Components/TGOR_StatComponent.h"

#include "UISystem/Queries/TGOR_ContentQuery.h"
#include "TGOR_StatsQuery.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////


/**
 * 
 */
UCLASS()
class SIMULATIONSYSTEM_API UTGOR_StatsQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_StatsQuery();
	TArray<UTGOR_CoreContent*> QueryContent() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Assigns StatComponent and filters stat states */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Stats", Meta = (Keywords = "C++"))
		virtual void AssignComponent(UTGOR_StatComponent* StatComponent, ETGOR_StatTypeEnumeration Filter);

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Stats")
		TArray<FTGOR_StatOutput> Stats;
};
