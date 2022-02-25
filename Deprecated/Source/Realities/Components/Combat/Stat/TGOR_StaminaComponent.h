///////////////////////////////////////////////////
// The Gateway of Realities: Planes of Existence //
// Copyright 2016-2017                           //
///////////////////////////////////////////////////

#pragma once

#include "Realities/Components/Combat/Stat/TGOR_StatComponent.h"
#include "TGOR_StaminaComponent.generated.h"

/**
* UTGOR_StaminaComponent adds stamina functionality to an actor.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REALITIES_API UTGOR_StaminaComponent : public UTGOR_StatComponent
{
	GENERATED_BODY()

public:	
	UTGOR_StaminaComponent();

};
