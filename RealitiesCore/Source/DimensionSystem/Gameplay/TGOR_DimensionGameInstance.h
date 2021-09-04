// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "TGOR_DimensionGameInstance.generated.h"

/**
* TGOR_DimensionGameInstance provides custom replication based on dimensions
*/
UCLASS(Blueprintable)
class DIMENSIONSYSTEM_API UTGOR_DimensionGameInstance : public UTGOR_GameInstance
{
	GENERATED_BODY()
	
public:
	UTGOR_DimensionGameInstance(const FObjectInitializer& ObjectInitializer);
	virtual void Init() override;

	/** Uses custom replication */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Online")
		bool UseCustomReplication;

};
