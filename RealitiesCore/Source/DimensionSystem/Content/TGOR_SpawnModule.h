// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "RealitiesUGC/Mod/TGOR_Insertions.h"

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_SpawnModule.generated.h"

UCLASS(Blueprintable)
class DIMENSIONSYSTEM_API UTGOR_SpawnModule : public UTGOR_CoreContent
{
	GENERATED_BODY()

public:
	UTGOR_SpawnModule();
	virtual bool Validate_Implementation() override;


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:
};

