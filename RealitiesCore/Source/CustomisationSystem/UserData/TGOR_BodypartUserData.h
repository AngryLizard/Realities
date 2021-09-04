// The Gateway of Realities: Planes of Existence.
#pragma once

#include "CoreMinimal.h"

#include "TGOR_GeometryUserData.h"

#include "TGOR_BodypartUserData.generated.h"

USTRUCT(BlueprintType)
struct FTGOR_BodypartPatchExtension
{
	GENERATED_USTRUCT_BODY();

	// Skeletal mesh to extend with
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		USkeletalMesh* Mesh = nullptr;

	// Section index to be extended with
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		uint32 SectionIndex = 0;

	// UV channel pointing to source mesh texture
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		uint32 UVChannel = 0;
};

USTRUCT(BlueprintType)
struct FTGOR_BodypartPatchSection
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		int32 SizeX = 9;

	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		int32 SizeY = 9;

	// Extensions to be included in baking
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		TArray<FTGOR_BodypartPatchExtension> Extensions;

	// Mask to be used for bodypart merge
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		UTexture* BlendMask = nullptr;
};

/** Stores bodypart information for mesh holes */
UCLASS(Blueprintable)
class CUSTOMISATIONSYSTEM_API UTGOR_BodypartUserData : public UTGOR_GeometryUserData
{
	GENERATED_UCLASS_BODY()

public:
	virtual void Draw(class FPrimitiveDrawInterface* PDI, const class FSceneView* View) const override;
	virtual FIntPoint GetTextureSize(int32 SectionIndex) const override;
	virtual FString GetTextureName(int32 SectionIndex) const override;

	virtual void BakeData(USkeletalMesh* SourceMesh) override;

	/** Distance threshold for vertices to match */
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		float MatchThreshold;

	/** Amount of vertices that got matched */
	UPROPERTY(VisibleAnywhere, Category = "MeshMerge")
		uint32 MatchCount = 0;

	// Section-specific options
	UPROPERTY(EditAnywhere, Category = "MeshMerge")
		TArray<FTGOR_BodypartPatchSection> Sections;
};

