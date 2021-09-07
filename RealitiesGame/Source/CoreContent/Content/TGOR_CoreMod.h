// The Gateway of Realities: Planes of Existence.

#pragma once

#include "RealitiesUGC/Mod/TGOR_Mod.h"
#include "TGOR_CoreMod.generated.h"


UCLASS()
class CORECONTENT_API UTGOR_CoreMod : public UTGOR_Mod
{
	GENERATED_BODY()

	public:
		UTGOR_CoreMod(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};

UCLASS()
class CORECONTENT_API UTGOR_TestMod : public UTGOR_Mod
{
	GENERATED_BODY()

public:
	UTGOR_TestMod(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
