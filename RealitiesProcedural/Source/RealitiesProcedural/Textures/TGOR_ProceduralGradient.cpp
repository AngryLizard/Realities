// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGOR_ProceduralGradient.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "TextureResource.h"
#include "EngineUtils.h"
#include "DeviceProfiles/DeviceProfile.h"


FTGOR_ProceduralGradientResource::FTGOR_ProceduralGradientResource(UTGOR_ProceduralGradient* InOwner)
{
	Owner = InOwner;
	bSRGB = InOwner->SRGB;
}

FTGOR_ProceduralGradientResource::~FTGOR_ProceduralGradientResource()
{
}

uint32 FTGOR_ProceduralGradientResource::GetSizeX() const
{
	if(Owner) return Owner->GetSizeX();
	return 256;
}

uint32 FTGOR_ProceduralGradientResource::GetSizeY() const
{
	if (Owner) return Owner->GetSizeY();
	return 256;
}

void FTGOR_ProceduralGradientResource::InitRHI()
{
	const uint32 Width = GetSizeX();
	const uint32 Height = GetSizeY();

	// Create the RHI texture.
	const ETextureCreateFlags TexCreateFlags = TexCreate_SRGB | TexCreate_OfflineProcessed | TexCreate_Streamable;

	//const ETextureCreateFlags TexCreateFlags = ;

	const EPixelFormat EffectiveFormat = Owner->GetPixelFormat();

	const FSamplerStateInitializerRHI SamplerStateInitializer(ESamplerFilter::SF_Bilinear, AM_Mirror, AM_Mirror);
	SamplerStateRHI = RHICreateSamplerState(SamplerStateInitializer);

	// Set the greyscale format flag appropriately.
	bGreyScaleFormat = (EffectiveFormat == PF_G8) || (EffectiveFormat == PF_BC4);

	FRHIResourceCreateInfo CreateInfo(TEXT("ProceduralGradientResource"));
	//CreateInfo.ExtData = Owner->PlatformData->GetExtData();
	const FTexture2DRHIRef Texture2DRHI = RHICreateTexture2D(Width, Height, EffectiveFormat, 1, 1, TexCreateFlags, CreateInfo);
	TextureRHI = Texture2DRHI;
	TextureRHI->SetName(Owner->GetFName());
	RHIBindDebugLabelName(TextureRHI, *Owner->GetName());
	RHIUpdateTextureReference(Owner->TextureReference.TextureReferenceRHI, TextureRHI);

	uint32 DestPitch;
	void* Data = RHILockTexture2D(Texture2DRHI, 0, RLM_WriteOnly, DestPitch, false);

	FMemory::Memcpy(Data, Owner->DataCache.GetData(), Owner->DataCache.Num() * sizeof(uint32));

	RHIUnlockTexture2D(Texture2DRHI, 0, false);
}

/** Initializes the texture array resource if needed. */
void FTGOR_ProceduralGradientResource::UpdateResource()
{
	if (IsInitialized())
	{
		ReleaseResource();
	}

	InitResource();
}


////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ProceduralGradient::UTGOR_ProceduralGradient(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WidthPow2 = 8;
	HeightPow2 = 8;

#if WITH_EDITORONLY_DATA
	SRGB = true;
	MipGenSettings = TMGS_NoMipmaps;
#endif
}

FTextureResource* UTGOR_ProceduralGradient::CreateResource()
{
	return new FTGOR_ProceduralGradientResource(this);
}


void UTGOR_ProceduralGradient::GenerateTexture()
{
	const int32 SizeX = GetSizeX();
	const int32 SizeY = GetSizeY();
	const int32 Num = SizeX * SizeY;

	TArray<FLinearColor> Pixels;
	Pixels.SetNumZeroed(Num);

	GenerateGradient(SizeX, SizeY, Pixels);

	DataCache.SetNumUninitialized(Num);
	for (int32 Index = 0; Index < Num; Index++)
	{
		Pixels[Index].A = 1.0f;
		DataCache[Index] = Pixels[Index].ToFColor(SRGB).ToPackedARGB();
	}
}


FColor GetIterationColor(uint32 Index)
{
	return FColor(
		(30 * Index) & 0xFF,
		(4 * Index) & 0xFF,
		(0xFF - (30 * Index)) & 0xFF,
		0xFF);
}

uint32 Mandelbrot(const FVector2D& c, uint32 MaxIterations, float R)
{

	FVector2D Z = FVector2D::ZeroVector;
	for (uint32 Index = 0; Index < MaxIterations; Index++)
	{
		Z = FVector2D(Z.X * Z.X - Z.Y * Z.Y, 2 * Z.X * Z.Y) + c;

		if (Z.Size() > R) return Index;
	}
	return MaxIterations;
}

void UTGOR_ProceduralGradient::GenerateGradient(int32 SizeX, int32 SizeY, TArray<FLinearColor>& Pixels)
{
	const float MinX = -2.0f;
	const float MaxX = 1.0f;
	const float MinY = -1.5f;
	const float MaxY = 1.5f;
	const uint32 MaxIterations = 64;
	for (int32 X = 0; X < SizeX; X++)
	{
		const float ValX = MinX + (MaxX - MinX) * X / SizeX;
		for (int32 Y = 0; Y < SizeY; Y++)
		{
			const float ValY = MinY + (MaxY - MinY) * Y / SizeY;

			const uint32 Index = Mandelbrot(FVector2D(ValX, ValY), MaxIterations, 2.0f);
			Pixels[SizeY * X + Y] = ((Index == MaxIterations) ? FLinearColor::Black : FLinearColor(GetIterationColor(Index)));
		}
	}
}

void UTGOR_ProceduralGradient::UpdateResource()
{
	GenerateTexture();

	if (PlatformData)
	{
		PlatformData->Mips.Empty();
	}
#if WITH_EDITOR

	const uint32 SizeX = GetSizeX();
	const uint32 SizeY = GetSizeY();
	Source.Init(SizeX, SizeY, 1, 1, ETextureSourceFormat::TSF_BGRA8, (const uint8*)DataCache.GetData());

	// Re-cache platform data if the source has changed.
	CookedPlatformData.Empty();
#endif // #if WITH_EDITOR

	Super::UpdateResource();
}

uint32 UTGOR_ProceduralGradient::CalcTextureMemorySizeEnum(ETextureMipCount Enum) const
{

	return GetSizeX() * GetSizeY() * sizeof(uint32);
}


void UTGOR_ProceduralGradient::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	FStripDataFlags StripFlags(Ar);
	bool bCooked = Ar.IsCooking();
	Ar << bCooked;

	if (bCooked || Ar.IsCooking())
	{
		//SerializeCookedPlatformData(Ar);
	}

#if WITH_EDITOR
	if (Ar.IsLoading() && !Ar.IsTransacting() && !bCooked)
	{
		//BeginCachePlatformData();
	}
#endif
}

void UTGOR_ProceduralGradient::PostLoad()
{
#if WITH_EDITOR
	//FinishCachePlatformData();

#endif // #if WITH_EDITOR
	Super::PostLoad();
};

void UTGOR_ProceduralGradient::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
#if WITH_EDITOR
	int32 SizeX = GetSizeX();
	int32 SizeY = GetSizeY();
#else
	int32 SizeX = 0;
	int32 SizeY = 0;
	int32 SizeZ = 0;
#endif
	const FString Dimensions = FString::Printf(TEXT("%dx%d"), SizeX, SizeY);
	OutTags.Add(FAssetRegistryTag("Dimensions", Dimensions, FAssetRegistryTag::TT_Dimensional));
	OutTags.Add(FAssetRegistryTag("Format", GPixelFormats[GetPixelFormat()].Name, FAssetRegistryTag::TT_Alphabetical));

	Super::GetAssetRegistryTags(OutTags);
}

FString UTGOR_ProceduralGradient::GetDesc()
{
	return FString::Printf(TEXT("Array: %dx%dy [%s]"),
		GetSizeX(),
		GetSizeY(),
		GPixelFormats[GetPixelFormat()].Name
	);
}

void UTGOR_ProceduralGradient::GetResourceSizeEx(FResourceSizeEx& CumulativeResourceSize)
{
	Super::GetResourceSizeEx(CumulativeResourceSize);
	CumulativeResourceSize.AddUnknownMemoryBytes(CalcTextureMemorySizeEnum(TMC_ResidentMips));
}


#if WITH_EDITOR

REALITIESPROCEDURAL_API void UTGOR_ProceduralGradient::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropertyName = PropertyChangedEvent.GetPropertyName();

	if (PowerOfTwoMode == ETexturePowerOfTwoSetting::None && (!Source.IsPowerOfTwo()))
	{
		// Force NPT textures to have no mip maps.
		if (PropertyName == GET_MEMBER_NAME_CHECKED(UTGOR_ProceduralGradient, MipGenSettings))
		{
			UE_LOG(LogTexture, Warning, TEXT("Cannot use mip maps for non-power of two textures."));
		}

		MipGenSettings = TMGS_NoMipmaps;
		NeverStream = true;
	}

	UpdateResource();

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif