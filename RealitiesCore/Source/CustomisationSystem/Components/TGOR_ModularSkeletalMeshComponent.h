// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "CustomisationSystem/TGOR_BodypartInstance.h"

#include "CoreSystem/Storage/TGOR_SaveInterface.h"
#include "DimensionSystem/Interfaces/TGOR_SpawnerInterface.h"
#include "Components/SkeletalMeshComponent.h"
#include "TGOR_ModularSkeletalMeshComponent.generated.h"


/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CUSTOMISATIONSYSTEM_API UTGOR_ModularSkeletalMeshComponent : public USkeletalMeshComponent, public ITGOR_SaveInterface, public ITGOR_SpawnerInterface
{
	GENERATED_BODY()
	
public:
	UTGOR_ModularSkeletalMeshComponent();

	virtual void UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies) override;
	virtual TMap<int32, UTGOR_SpawnModule*> GetModuleType_Implementation() const override;

};
