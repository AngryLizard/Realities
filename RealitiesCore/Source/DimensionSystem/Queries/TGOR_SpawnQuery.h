// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "UISystem/Queries/TGOR_ContentQuery.h"
#include "TGOR_SpawnQuery.generated.h"

class UTGOR_SpawnModule;
class UTGOR_IdentityComponent;


/**
* 
 */
USTRUCT(BlueprintType)
struct DIMENSIONSYSTEM_API FTGOR_SpawnModulePair
{
	GENERATED_USTRUCT_BODY()

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UObject* Object = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Index = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_SpawnModule* Module = nullptr;
};

/**
 * UTGOR_SpawnQuery
 */
UCLASS()
class DIMENSIONSYSTEM_API UTGOR_SpawnQuery : public UTGOR_ContentQuery
{
	GENERATED_BODY()
	
public:
	UTGOR_SpawnQuery();
	virtual TArray<UTGOR_CoreContent*> QueryContent() const override;

	/** Constructs assemble recipes using currently running action slot as matter source */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Game", Meta = (Keywords = "C++"))
		virtual void AssignIdentity(UTGOR_IdentityComponent* IdentityComponent);

	/** Associated spawn objects */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Game")
		TArray<FTGOR_SpawnModulePair> SpawnObjects;
};
