// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGOR_ProceduralGradientExporter.h"
#include "HAL/UnrealMemory.h"

#include "Factories.h"
#include "RealitiesProcedural/Textures/TGOR_ExteriorGradient.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"

#define LOCTEXT_NAMESPACE "ProceduralGradientExporter"

UTGOR_ProceduralGradientExporterTGA::UTGOR_ProceduralGradientExporterTGA(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	SupportedClass = UTGOR_ProceduralGradient::StaticClass();
	PreferredFormatIndex = 0;
	FormatExtension.Add(TEXT("TGA"));
	FormatDescription.Add(TEXT("Targa"));
}

bool UTGOR_ProceduralGradientExporterTGA::SupportsObject(UObject* Object) const
{
	if (Super::SupportsObject(Object))
	{
		UTGOR_ProceduralGradient* Texture = Cast<UTGOR_ProceduralGradient>(Object);
		if (Texture)
		{
			return true;
		}
	}
	return false;
}

struct FTGAFileFooter
{
	uint32 ExtensionAreaOffset;
	uint32 DeveloperDirectoryOffset;
	uint8 Signature[16];
	uint8 TrailingPeriod;
	uint8 NullTerminator;
};

bool UTGOR_ProceduralGradientExporterTGA::ExportBinary(UObject* Object, const TCHAR* Type, FArchive& Ar, FFeedbackContext* Warn, int32 FileIndex, uint32 PortFlags)
{
	UTGOR_ProceduralGradient* Texture = CastChecked<UTGOR_ProceduralGradient>(Object);
	if (!Texture->Source.IsValid() || (Texture->Source.GetFormat() != TSF_BGRA8))
	{
		return false;
	}

	const int32 BytesPerPixel = 4;

	int32 SizeX = Texture->Source.GetSizeX();
	int32 SizeY = Texture->Source.GetSizeY();
	TArray64<uint8> RawData;
	Texture->Source.GetMipData(RawData, 0);

	// If we should export the file with no alpha info.  
	// If the texture is compressed with no alpha we should definitely not export an alpha channel
	bool bExportWithAlpha = !Texture->CompressionNoAlpha;
	if (bExportWithAlpha)
	{
		// If the texture isn't compressed with no alpha scan the texture to see if the alpha values are all 255 which means we can skip exporting it.
		// This is a relatively slow process but we are just exporting textures 
		bExportWithAlpha = false;
		const int32 AlphaOffset = 3;
		for (int32 Y = SizeY - 1; Y >= 0; --Y)
		{
			uint8* Color = &RawData[Y * SizeX * BytesPerPixel];
			for (int32 X = SizeX; X > 0; --X)
			{
				// Skip color info
				Color += AlphaOffset;
				// Get Alpha value then increment the pointer past it for the next pixel
				uint8 Alpha = *Color++;
				if (Alpha != 255)
				{
					// When a texture is imported with no alpha, the alpha bits are set to 255
					// So if the texture has non 255 alpha values, the texture is a valid alpha channel
					bExportWithAlpha = true;
					break;
				}
			}
			if (bExportWithAlpha)
			{
				break;
			}
		}
	}

	const int32 OriginalWidth = SizeX;
	const int32 OriginalHeight = SizeY;

	FTGAFileHeader TGA;
	FMemory::Memzero(&TGA, sizeof(TGA));
	TGA.ImageTypeCode = 2;
	TGA.BitsPerPixel = bExportWithAlpha ? 32 : 24;
	TGA.Height = OriginalHeight;
	TGA.Width = OriginalWidth;
	Ar.Serialize(&TGA, sizeof(TGA));

	if (bExportWithAlpha)
	{
		for (int32 Y = 0; Y < OriginalHeight; Y++)
		{
			// If we aren't skipping alpha channels we can serialize each line
			Ar.Serialize(&RawData[(OriginalHeight - Y - 1) * OriginalWidth * 4], OriginalWidth * 4);
		}
	}
	else
	{
		// Serialize each pixel
		for (int32 Y = OriginalHeight - 1; Y >= 0; --Y)
		{
			uint8* Color = &RawData[Y * OriginalWidth * BytesPerPixel];
			for (int32 X = OriginalWidth; X > 0; --X)
			{
				Ar << *Color++;
				Ar << *Color++;
				Ar << *Color++;
				// Skip alpha channel since we are exporting with no alpha
				Color++;
			}
		}
	}

	FTGAFileFooter Ftr;
	FMemory::Memzero(&Ftr, sizeof(Ftr));
	FMemory::Memcpy(Ftr.Signature, "TRUEVISION-XFILE", 16);
	Ftr.TrailingPeriod = '.';
	Ar.Serialize(&Ftr, sizeof(Ftr));
	return true;
}

#undef LOCTEXT_NAMESPACE