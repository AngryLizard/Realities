// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_Palette.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class CUSTOMISATIONSYSTEM_API UTGOR_Palette : public UTGOR_CoreContent
{
	GENERATED_BODY()
	
public:
	UTGOR_Palette();

	/** Color texture */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Palette")
		UTexture* Color;

	/** Material texture */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Palette")
		UTexture* Material;

};
