// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Components/BillboardComponent.h"
#include "TGOR_WorldPainterCanvas.h"

#include "GameFramework/Actor.h"
#include "TGOR_WorldPainterLayer.generated.h"

//////////////////////////////////////////// DECL /////////////////////////////////////////////////

class UProceduralMeshComponent;
class UTGOR_WorldPainterTexture;
class UTextureRenderTarget2D;

//////////////////////////////////////////// STRUCTS /////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FTGOR_BrushData
{
	GENERATED_USTRUCT_BODY()

	FTGOR_BrushData();
	FTGOR_BrushData(int32 Num, const FLinearColor& Color);
	FLinearColor Get(int32 Index) const;
	void AddWeighted(const FTGOR_BrushData& Data, float Weight);
	void Normalize();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ShowOnlyInnerProperties))
		TArray<FLinearColor> Slices;
};

USTRUCT(BlueprintType)
struct FTGOR_BrushPoint
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Variance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D Position = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_BrushData Data;
};

/**
* ATGOR_WorldPainterLayer
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIESPROCEDURAL_API ATGOR_WorldPainterLayer : public AActor
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_WorldPainterLayer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Painter Texture")
		UVolumeTexture* TargetTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Painter Texture")
		UTexture2D* TargetTexture2D;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Painter Texture")
		UMaterialInterface* RenderMaterial;

	/** Base color to be applied where there is no brush */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Painter Texture", meta = (ShowOnlyInnerProperties))
		FTGOR_BrushData Bias;

	/** Power of two size in pixels */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Painter Texture", Meta = (ClampMin = 0, ClampMax = 12))
		int32 TextureSize = 8;

	/** How much brush colors disperse into bias value */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Painter Texture", Meta = (ClampMin = 0, ClampMax = 20))
		float Dispersion = 5.0f;

	/** How much blending there is between triangles */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Painter Texture", Meta = (ClampMin = 0, ClampMax = 1))
		float Blending = 1.0f;

	/** Seed for the randome vertex generator */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Painter Texture")
		int32 GeneratorSeed = 69;

	/** Amount of disturbance for each vertex */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Painter Texture", Meta = (ClampMin = 0, ClampMax = 1))
		float VertexDisturbance = 0.5f;

	/** Number of vertices to draw per side */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Painter Texture", Meta = (ClampMin = 0, ClampMax = 256))
		int32 VertexSamples = 32;

	/** ID forwarded to the billboard material */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Painter Texture", Meta = (ClampMin = 0, ClampMax = 256))
		int32 MaterialID = 0;

	/** Draw contained brushes to Target Texture */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "World Painter Texture", Meta = (Keywords = "C++"))
		void Bake();


#if WITH_EDITOR
	/** Called when a brush updated (currently unused) */
	void OnBrushUpdate(bool bFinished);
#endif // WITH_EDITOR

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY()
		UTextureRenderTarget2D* CacheRenderTarget;

	// Raw pixel buffer
	TArray<TArray<FColor>> RawData;

	/** Get canvas this layer resides in */
	UFUNCTION()
		ATGOR_WorldPainterCanvas* GetPainterCanvas() const;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual void PostEditMove(bool bFinished) override;
#endif // WITH_EDITOR

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	// These procedures were split-up in case we ever want to update this at runtime (only some of the last few steps need to be repeated on brush/setting changes)
	bool GenerateTriangulationDone = false;
	void GenerateTriangulation();

	bool GeneratePointsDone = false;
	void GeneratePoints();

	bool GenerateVerticesDone = false;
	void GenerateVertices();

	bool GenerateRendertargetDone = false;
	void GenerateRendertarget();

	bool GenerateTextureDone = false;
	void GenerateTexture();

	FTGOR_Triangulation2D Triangulation;
	TArray<FTGOR_BrushPoint> Points;
	TArray<FTGOR_BrushData> Vertices;

	void BeginDrawCanvasToRenderTargetVolume(UCanvas*& Canvas, FVector2D& Size);
	void EndDrawCanvasToRenderTargetVolume();

	FDrawEvent* ContextDrawEvent;
};
