// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "TGOR_DisplayInstance.generated.h"

USTRUCT(BlueprintType)
struct FTGOR_Display
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Content")
		FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Content", meta = (AllowPrivateAccess = "true", DisplayThumbnail = "true", DisplayName = "Image", AllowedClasses = "Texture,MaterialInterface,SlateTextureAtlasInterface"))
		UObject* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Content")
		float Aspect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Content")
		FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Content")
		FLinearColor CustomColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Content")
		bool UseCustomColor;


	FTGOR_Display()
	{
		Icon = nullptr;
		Aspect = 1.0f;
		CustomColor = FLinearColor::Black;
		UseCustomColor = false;
	}
};