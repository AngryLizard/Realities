// The Gateway of Realities: Planes of Existence.

#pragma once

#include "../TGOR_BodypartInstance.h"

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_Canvas.generated.h"

class UTGOR_Section;

class UTGOR_SkeletalMergeUserData;
class UTGOR_BodypartUserData;
class UTGOR_Bodypart;
class UTGOR_Section;

UENUM(BlueprintType)
enum class ETGOR_PBRExport : uint8
{
	Emissive,
	Occlusion,
	Normal
};

/**
 * 
 */
UCLASS(Blueprintable)
class CUSTOMISATIONSYSTEM_API UTGOR_Canvas : public UTGOR_CoreContent
{
	GENERATED_BODY()
	
public:
	UTGOR_Canvas();
	virtual void BuildResource() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Format used for the rendertarget */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Canvas")
		TEnumAsByte<ETextureRenderTargetFormat> Format;

	/** Compression settings to use when baking the texture. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Canvas")
		TEnumAsByte<enum TextureCompressionSettings> CompressionSettings;

	/** Power of two scale applied to the texture */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Canvas")
		FTGOR_TextureScale Scale;

	/** Name used in material texture parameter */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Canvas")
		FName ParameterName;

	/** Type to be assigned when exporting to file */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Canvas")
		ETGOR_PBRExport ExportType;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Name used in merge material texture parameter */
	UPROPERTY(EditAnywhere, Category = "!TGOR Canvas")
		FName BlendTextureName;

	/** Name used in merge material tangents parameter */
	UPROPERTY(EditAnywhere, Category = "!TGOR Canvas")
		FName BlendTangentsName;

	/** Name used in merge material normals parameter */
	UPROPERTY(EditAnywhere, Category = "!TGOR Canvas")
		FName BlendNormalsName;

	/** Name used in merge material reference parameter */
	UPROPERTY(EditAnywhere, Category = "!TGOR Canvas")
		FName BlendReferenceName;

	/** Name used in merge target material convert parameter */
	UPROPERTY(EditAnywhere, Category = "!TGOR Canvas")
		FName BlendConvertName;

	/** Name used in merge material mask parameter */
	UPROPERTY(EditAnywhere, Category = "!TGOR Canvas")
		FName BlendMaskName;

	/** Material used for texture blending */
	UPROPERTY(EditAnywhere, Category = "!TGOR Canvas")
		UMaterialInterface* BlendMaterial;

	/** Material used for texture blending */
	UPROPERTY(EditAnywhere, Category = "!TGOR Canvas")
		UMaterialInterface* BlendTextureMaterial;

	/** Render the merge geometry between two bodyparts */
	void RenderBlend(UTGOR_SkeletalMergeUserData* MergeData, UTGOR_BodypartUserData* BodypartData, UTGOR_Bodypart* Bodypart, UTGOR_Section* Section, UTexture* Texture, const FTransform& Transform, FCanvas* Target);

	/** Render the merge geometry between two bodyparts using the baked texture */
	void RenderBlendTexture(UTGOR_BodypartUserData* BodypartData, UTGOR_BodypartUserData* ParentData, UTGOR_Bodypart* Bodypart, UTGOR_Section* Section, UTexture* Texture, FCanvas* Target, const FVector2D& Pos, const FVector2D& Size);

private:

	/** */
	UPROPERTY()
		UMaterialInstanceDynamic* DynamicBlendMaterial;

	/** */
	UPROPERTY()
		UMaterialInstanceDynamic* DynamicBlendTextureMaterial;
};
