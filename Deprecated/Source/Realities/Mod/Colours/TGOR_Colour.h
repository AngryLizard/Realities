// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Engine/Texture2D.h"

#include "Realities/Base/Content/TGOR_Content.h"
#include "TGOR_Colour.generated.h"

/**
 * 
 */

UCLASS(Blueprintable)
class REALITIES_API UTGOR_Colour : public UTGOR_Content
{
	GENERATED_BODY()
	
public:
	UTGOR_Colour();

	/** Colour Hue enum. Describes the colour hue in a machine friendly way. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Colour")
		ETGOR_ColorHue ColorHue;

	/** Colour Type enum. Describes what this color is like in a machine friendly way! */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Colour")
		ETGOR_ColorType ColorType;

	/** Colour group */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Colour")
		FText GroupName;

	/** Colour lights */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Colour")
		FLinearColor Lights;

	/** Colour medium */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Colour")
		FLinearColor Medium;

	/** Colour darks */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Colour")
		FLinearColor Darks;

	/** Colour texture */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Colour")
		UTexture2D* Texture;

	/** Colour roughness */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Colour")
		float Roughness;

	/** Colour metallic */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Colour")
		float Metallic;

	/** Colour glow */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Colour")
		float Glow;

	/** Colour glow */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Colour")
		float EffectControl;


};
