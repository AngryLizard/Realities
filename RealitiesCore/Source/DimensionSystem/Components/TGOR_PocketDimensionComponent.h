// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "TGOR_DimensionLoaderComponent.h"
#include "../Interfaces/TGOR_SpawnerInterface.h"
#include "TGOR_PocketDimensionComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DIMENSIONSYSTEM_API UTGOR_PocketDimensionComponent : public UTGOR_DimensionLoaderComponent, public ITGOR_SpawnerInterface
{
	GENERATED_BODY()

public:	
	UTGOR_PocketDimensionComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies) override;
	virtual TMap<int32, UTGOR_SpawnModule*> GetModuleType_Implementation() const override;
	
	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Dimension spawned by this loader. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		TSubclassOf<UTGOR_Dimension> SpawnDimension;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/* Dimension to be used for this pocket dimension */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		UTGOR_Dimension* PocketDimension;

	/* Identifier to be used for this pocket dimension */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		FString PocketIdentifier;

	/** Sets pocket dimension level volume to given force */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void SetExternal(const FVector& Force);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Load dimension from content class */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void BuildDimension();

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:
};