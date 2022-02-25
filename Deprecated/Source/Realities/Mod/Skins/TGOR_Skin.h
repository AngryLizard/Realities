// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"

#include "Realities/Base/Content/TGOR_Content.h"
#include "TGOR_Skin.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class REALITIES_API UTGOR_Skin : public UTGOR_Content
{
	GENERATED_BODY()

public:
	UTGOR_Skin();

	/** Texture mapped to their material parameter name */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skin")
		TMap<FName, UTexture2D*> Textures;
};