// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "TGOR_ProceduralGradient.generated.h"

#define MAX_ARRAY_SLICES 512

class UTGOR_ProceduralGradient;

/** Represents a 2D Texture Array to the renderer. */
class FTGOR_ProceduralGradientResource : public FTextureResource
{
public:

	FTGOR_ProceduralGradientResource() : Owner(nullptr) {}
	FTGOR_ProceduralGradientResource(UTGOR_ProceduralGradient* InOwner);
	virtual ~FTGOR_ProceduralGradientResource();

	// Rendering thread functions

	/** Initializes the texture array resource if needed, and re-initializes if the texture array has been made dirty since the last init. */
	void UpdateResource();

	/**
	* Called when the resource is initialized. This is only called by the rendering thread.
	*/
	virtual void InitRHI() override;

	/** Returns the width of the texture in pixels. */
	virtual uint32 GetSizeX() const override;

	/** Returns the height of the texture in pixels. */
	virtual uint32 GetSizeY() const override;
private:

	UTGOR_ProceduralGradient* Owner;

	/** Copies data from DataEntry into Dest, taking stride into account. */
	//void GetData(const FTextureArrayDataEntry& DataEntry, int32 MipIndex, void* Dest, uint32 DestPitch);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

UCLASS()
class REALITIESPROCEDURAL_API UTGOR_ProceduralGradient : public UTexture
{
	GENERATED_UCLASS_BODY()

public:
	/** The derived data for this texture on this platform. */
	FTexturePlatformData* PlatformData;
#if WITH_EDITOR
	/* cooked platform data for this texture */
	TMap<FString, FTexturePlatformData*> CookedPlatformData;
#endif

	/*
	* Initialize texture source from textures in SourceArray.
	* @param bUpdateSourceSettings Set to false to prevent overriding current texture settings.
	*/
	/** Trivial accessors. */
	FORCEINLINE int32 GetSizeX() const
	{
		return 1 << FMath::Clamp(WidthPow2, 1, 11);
	}
	FORCEINLINE int32 GetSizeY() const
	{
		return 1 << FMath::Clamp(HeightPow2, 1, 11);
	}
	FORCEINLINE EPixelFormat GetPixelFormat() const
	{
		return EPixelFormat::PF_B8G8R8A8;
	}

	/**
	 * Generate gradient from input into pixel cache
	 */
	UFUNCTION()
		void GenerateTexture();

	/**
	 * Generate gradient from input into pixel cache
	 */
	UFUNCTION()
		virtual void GenerateGradient(int32 SizeX, int32 SizeY, TArray<FLinearColor>& Pixels);

	/** Pixel cache used by resource */
	UPROPERTY(Transient)
		TArray<uint32> DataCache;

	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural", meta = (ClampMin = 1, ClampMax = 10))
		int32 WidthPow2;

	UPROPERTY(EditAnywhere, Category = "!TGOR Procedural", meta = (ClampMin = 1, ClampMax = 10))
		int32 HeightPow2;

	/** Base color */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Texture, AssetRegistrySearchable, AdvancedDisplay)
	//	FLinearColor Base;

	/** Information for thumbnail rendering */
	UPROPERTY(VisibleAnywhere, Instanced, Category = Thumbnail)
		class UThumbnailInfo* ThumbnailInfo;

	//~ Begin UTexture Interface
	virtual void Serialize(FArchive& Ar) override;
	virtual void PostLoad() override;
	virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;
	virtual FString GetDesc() override;
	virtual void GetResourceSizeEx(FResourceSizeEx& CumulativeResourceSize) override;
	virtual float GetSurfaceWidth() const override { return GetSizeX(); }
	virtual float GetSurfaceHeight() const override { return GetSizeY(); }
	virtual FTextureResource* CreateResource() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

	virtual void UpdateResource() override;
	virtual EMaterialValueType GetMaterialType() const override { return MCT_Texture2D; }
	virtual FTexturePlatformData** GetRunningPlatformData() override { return &PlatformData; }
#if WITH_EDITOR
	virtual TMap<FString, FTexturePlatformData*>* GetCookedPlatformData() override { return &CookedPlatformData; }
#endif
	//~ End UTexture Interface


	/**
	 * Calculates the size of this texture in bytes if it had MipCount mip-levels streamed in.
	 *
	 * @param	MipCount Number of mips to calculate size for, counting from the smallest 1x1 mip-level and up.
	 * @return	Size of MipCount mips in bytes
	 */
	uint32 CalcTextureMemorySize(int32 MipCount) const;

	/**
	* Calculates the size of this texture if it had MipCount mip levels streamed in.
	*
	* @param	Enum which mips to calculate size for.
	* @return	Total size of all specified mips, in bytes
	*/
	virtual uint32 CalcTextureMemorySizeEnum(ETextureMipCount Enum) const override;

protected:
};
