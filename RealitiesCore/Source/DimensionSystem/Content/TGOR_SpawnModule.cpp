// The Gateway of Realities: Planes of Existence.

#include "TGOR_SpawnModule.h"
#include "UObject/ConstructorHelpers.h"

UTGOR_SpawnModule::UTGOR_SpawnModule()
	: Super()
{
	DebugWidget = ConstructorHelpers::FClassFinder<UTGOR_SpawnModuleWidget>(TEXT("/RealitiesCore/UI/Debug/UMG_DefaultModuleWidget.UMG_DefaultModuleWidget_C")).Class;
}
