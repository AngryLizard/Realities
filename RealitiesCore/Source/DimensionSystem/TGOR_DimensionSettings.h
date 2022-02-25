// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_DimensionSettings.generated.h"

UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Dimension Settings"))
class DIMENSIONSYSTEM_API UTGOR_DimensionSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UTGOR_DimensionSettings();

	/* Axis used by movement projection */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Dimension")
		TEnumAsByte<EAxis::Type> PilotProjectionAxis = EAxis::X;

};