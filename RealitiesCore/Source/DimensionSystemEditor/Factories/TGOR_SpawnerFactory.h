// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActorFactories/ActorFactory.h"
#include "TGOR_SpawnerFactory.generated.h"

class UTGOR_Spawner;

/** Factory for creating actor from spawner */
UCLASS(hidecategories = Object)
class DIMENSIONSYSTEMEDITOR_API UTGOR_SpawnerFactory : public UActorFactory
{
	GENERATED_UCLASS_BODY()

	//~ Begin UActorFactory Interface
	virtual void PostSpawnActor(UObject* Asset, AActor* NewActor) override;
	virtual AActor* GetDefaultActor(const FAssetData& AssetData) override;
	virtual bool CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg) override;
	//~ End UActorFactory Interface
};

