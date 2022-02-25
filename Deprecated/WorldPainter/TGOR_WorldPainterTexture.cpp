// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGOR_WorldPainterTexture.h"
#include "RenderUtils.h"

UTGOR_WorldPainterTexture::UTGOR_WorldPainterTexture(const FObjectInitializer& ObjectInitializer)
	: UObject(ObjectInitializer)
	, Texture(nullptr)
{
}

#if WITH_EDITOR

void UTGOR_WorldPainterTexture::BuildTexture(const TArray<TArray<FColor>>& RawData, int32 PixelSize)
{
	if (!IsValid(Texture))
	{
		Texture = NewObject<UVolumeTexture>(this, "VolumeTexture", EObjectFlags::RF_Public);
	}

	const int32 SliceNum = RawData.Num();
	if (SliceNum > 0 && PixelSize > 0)
	{
		if (IsValid(Texture))
		{
			const int32 FormatDataSize = FTextureSource::GetBytesPerPixel(ETextureSourceFormat::TSF_BGRA8);
			Texture->UpdateSourceFromFunction([this, FormatDataSize, PixelSize, &RawData](int32 X, int32 Y, int32 Z, void* Data) {

				FMemory::Memcpy(Data, &RawData[Z][X + Y * PixelSize], FormatDataSize);

			}, PixelSize, PixelSize, SliceNum, ETextureSourceFormat::TSF_BGRA8);

			Texture->PostEditChange();
		}
		MarkPackageDirty();
	}
}

#endif
