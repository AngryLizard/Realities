// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#define TWOPI (6.28318530718f)
#define PARTPI (0.17453292519f)

#include "CoreSystem/Interfaces/TGOR_SingletonInterface.h"
#include "Blueprint/UserWidget.h"
#include "TGOR_Widget.generated.h"

/**
* TGOR_Widget for general widget functionality (click/hover events, movement)
*/
UCLASS()
class UISYSTEM_API UTGOR_Widget : public UUserWidget, public ITGOR_SingletonInterface
{
	GENERATED_BODY()
	
public:
	UTGOR_Widget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

};
