// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/VolumeTexture.h"
#include "TGOR_WorldPainterTexture.generated.h"

/**
 * Container for a UTextureVolume that can be used for WorldPainter
 */
UCLASS(ClassGroup = Rendering, BlueprintType)
class REALITIESPROCEDURAL_API UTGOR_WorldPainterTexture : public UObject
{
public:
	GENERATED_UCLASS_BODY()

	/** The UTexture object. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Texture)
		UVolumeTexture* Texture;

	/*
	UPROPERTY()
		float PainterAngle;

	UPROPERTY()
		FVector2D PainterLocation;

	UPROPERTY()
		FVector2D PainterSize; */

#if WITH_EDITOR
	/** Creates a new UVirtualTexture2D and stores it in the contained Texture. */
	void BuildTexture(const TArray<TArray<FColor>>& RawData, int32 PixelSize);
#endif

};
