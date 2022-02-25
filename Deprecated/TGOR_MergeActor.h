// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"

#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "RealitiesUtility/Structures/TGOR_Time.h"
#include "Realities/Base/Creature/TGOR_GeometryUserData.h"

#include "Realities/Actors/TGOR_Actor.h"
#include "TGOR_MergeActor.generated.h"


// ...
USTRUCT(BlueprintType)
struct FTGOR_MeshMergePackage
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		class USkeletalMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		class UTexture2D* Curvature;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		class UTexture2D* Normals;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		class UTexture2D* Occlusion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		int32 PosX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		int32 PosY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		TArray<int32> Sections;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		int32 Parent = -1;
};

// ...
USTRUCT(BlueprintType)
struct FTGOR_MeshMergePackages
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		class USkeleton* Skeleton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		TArray<FTGOR_MeshMergePackage> Packages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		class UMaterialInterface* CurvatureMerger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		class UMaterialInterface* NormalsMerger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		class UMaterialInterface* OcclusionMerger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		int32 SizeX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		int32 SizeY;
};

// ...
USTRUCT(BlueprintType)
struct FTGOR_MeshMergeSectors
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		class UMaterialInterface* MaterialInterface;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
		bool DoesUVTransform;
};

/**
* 
*/
UCLASS()
class REALITIES_API ATGOR_MergeActor : public ATGOR_Actor
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_MergeActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		void OnMeshMergeDone();

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* TargeComponent;


	UFUNCTION(BlueprintCallable, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		void DrawToTexture(UTextureRenderTarget2D* Target, UTexture2D* Texture, int32 PosX, int32 PosY);

	UFUNCTION(BlueprintCallable, Category = "!TGOR Pawn", Meta = (Keywords = "C++"))
		void DrawToTriangles(UTextureRenderTarget2D* Target, UTGOR_GeometryUserData* Geometry, int32 SectionIndex, const FTransform& PointTransform, UTexture2D* Mask, UTexture2D* Texture, UTexture2D* Parent, UMaterialInstanceDynamic* Material);

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Mesh")
		TArray<UMaterialInstanceDynamic*> Materials;

	UPROPERTY(BlueprintReadOnly, Category = "Mesh")
		UMaterialInstanceDynamic* CurvatureMerger;

	UPROPERTY(BlueprintReadOnly, Category = "Mesh")
		UMaterialInstanceDynamic* NormalsMerger;

	UPROPERTY(BlueprintReadOnly, Category = "Mesh")
		UMaterialInstanceDynamic* OcclusionMerger;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
		bool DrawDebugBases;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
		FTGOR_MeshMergePackages Packages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
		TArray<FTGOR_MeshMergeSectors> Sectors;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
		UTextureRenderTarget2D* CurvatureRenderTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
		UTextureRenderTarget2D* NormalsRenderTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
		UTextureRenderTarget2D* OcclusionRenderTarget;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
		UTextureRenderTarget2D* ManualCurvatureRenderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
		UTextureRenderTarget2D* ManualNormalsRenderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
		UTextureRenderTarget2D* ManualOcclusionRenderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
		UTextureRenderTarget2D* ManualReferenceRenderTarget;


	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Mesh", Meta = (Keywords = "C++"))
		void MergeTest();


private:
};
