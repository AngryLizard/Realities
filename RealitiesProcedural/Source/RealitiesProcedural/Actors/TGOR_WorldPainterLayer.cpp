// The Gateway of Realities: Planes of Existence.

#include "TGOR_WorldPainterLayer.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/ArrowComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "TGOR_WorldPainterBrush.h"

#include "Engine/Canvas.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Engine/TextureRenderTargetVolume.h"
#include "Engine/VolumeTexture.h"

#define INV_SQRTWOPI 0.398942280401f

FTGOR_BrushData::FTGOR_BrushData()
	: FTGOR_BrushData(2, FLinearColor::Black)
{
}

FTGOR_BrushData::FTGOR_BrushData(int32 Num, const FLinearColor& Color)
{
	for (int32 Index = 0; Index < Num; Index++)
	{
		Slices.Emplace(Color);
	}
}

FLinearColor FTGOR_BrushData::Get(int32 Index) const
{
	if (Slices.IsValidIndex(Index)) return Slices[Index];
	return FLinearColor(0.f, 0.f, 0.f, 0.f);
}

void FTGOR_BrushData::AddWeighted(const FTGOR_BrushData& Data, float Weight)
{
	const int32 Num = FMath::Min(Data.Slices.Num(), Slices.Num());
	for (int32 Index = 0; Index < Num; Index++)
	{
		const float Intensity = Data.Slices[Index].A * Weight;
		Slices[Index].R += Data.Slices[Index].R * Intensity;
		Slices[Index].G += Data.Slices[Index].G * Intensity;
		Slices[Index].B += Data.Slices[Index].B * Intensity;
		Slices[Index].A += Intensity;
	}
}

void FTGOR_BrushData::Normalize()
{
	const int32 Num = Slices.Num();
	for (int32 Index = 0; Index < Num; Index++)
	{
		if (!FMath::IsNearlyZero(Slices[Index].A))
		{
			Slices[Index].R /= Slices[Index].A;
			Slices[Index].G /= Slices[Index].A;
			Slices[Index].B /= Slices[Index].A;
			Slices[Index].A = 1.0f;
		}
	}
}



ATGOR_WorldPainterLayer::ATGOR_WorldPainterLayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	TargetTexture(nullptr), 
	TargetTexture2D(nullptr),
	RenderMaterial(nullptr)
{
	bIsEditorOnlyActor = true;

	USceneComponent* Root = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, FName(TEXT("Root")));
	Root->SetMobility(EComponentMobility::Static);
	SetRootComponent(Root);

	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UMaterialInterface> WPCanvasMaterial;

		FConstructorStatics()
			: WPCanvasMaterial(TEXT("/RealitiesProcedural/Resources/M_WorldPainter_Bake.M_WorldPainter_Bake"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;
	RenderMaterial = ConstructorStatics.WPCanvasMaterial.Get();
}

#if WITH_EDITOR

void ATGOR_WorldPainterLayer::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	FProperty* ChangedProperty = PropertyChangedEvent.Property;

	if (ChangedProperty)
	{
		if (ChangedProperty->GetFName() == GET_MEMBER_NAME_CHECKED(ATGOR_WorldPainterLayer, GeneratorSeed) ||
			ChangedProperty->GetFName() == GET_MEMBER_NAME_CHECKED(ATGOR_WorldPainterLayer, VertexDisturbance) ||
			ChangedProperty->GetFName() == GET_MEMBER_NAME_CHECKED(ATGOR_WorldPainterLayer, VertexSamples))
		{
			GenerateTriangulationDone = false;
			GenerateVerticesDone = false;
			GenerateRendertargetDone = false;
			GenerateTextureDone = false;
		}

		if (ChangedProperty->GetFName() == GET_MEMBER_NAME_CHECKED(ATGOR_WorldPainterLayer, Dispersion))
		{
			GenerateVerticesDone = false;
			GenerateRendertargetDone = false;
			GenerateTextureDone = false;
		}

		if (ChangedProperty->GetFName() == GET_MEMBER_NAME_CHECKED(ATGOR_WorldPainterLayer, TargetTexture) ||
			ChangedProperty->GetFName() == GET_MEMBER_NAME_CHECKED(ATGOR_WorldPainterLayer, TargetTexture2D) ||
			ChangedProperty->GetFName() == GET_MEMBER_NAME_CHECKED(ATGOR_WorldPainterLayer, RenderMaterial) ||
			ChangedProperty->GetFName() == GET_MEMBER_NAME_CHECKED(ATGOR_WorldPainterLayer, TextureSize) ||
			ChangedProperty->GetFName() == GET_MEMBER_NAME_CHECKED(ATGOR_WorldPainterLayer, Blending))
		{
			GenerateRendertargetDone = false;
			GenerateTextureDone = false;
		}
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void ATGOR_WorldPainterLayer::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent)
{
	FProperty* ChangedProperty = PropertyChangedEvent.Property;

	if (ChangedProperty->GetFName() == GET_MEMBER_NAME_CHECKED(ATGOR_WorldPainterLayer, Bias))
	{
		GenerateVerticesDone = false;
		GenerateRendertargetDone = false;
		GenerateTextureDone = false;
	}
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
}

void ATGOR_WorldPainterLayer::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);

	OnBrushUpdate(bFinished);
}

void ATGOR_WorldPainterLayer::Bake()
{
	if (!GenerateTriangulationDone) GenerateTriangulation();
	if (!GeneratePointsDone) GeneratePoints();
	if (!GenerateVerticesDone) GenerateVertices();
	if (!GenerateRendertargetDone) GenerateRendertarget();
	if (!GenerateTextureDone) GenerateTexture();
	//GenerateMesh();
}

void ATGOR_WorldPainterLayer::OnBrushUpdate(bool bFinished)
{
	GeneratePointsDone = false;
	GenerateVerticesDone = false;
	GenerateRendertargetDone = false;
	GenerateTextureDone = false;

	if (bFinished)
	{
		Bake();
	}
}
#endif

ATGOR_WorldPainterCanvas* ATGOR_WorldPainterLayer::GetPainterCanvas() const
{
	return Cast<ATGOR_WorldPainterCanvas>(GetAttachParentActor());
}

#if WITH_EDITOR
void ATGOR_WorldPainterLayer::GenerateTriangulation()
{
	FRandomStream Random(GeneratorSeed);

	TArray<FVector2D> Samples;
	Samples.Reserve(VertexSamples + 4);
	for (int32 SampleX = -1; SampleX <= VertexSamples; SampleX++)
	{
		for (int32 SampleY = -1; SampleY <= VertexSamples; SampleY++)
		{
			const float X = FMath::Clamp((((float)SampleX) + 0.5f + Random.FRandRange(-0.5f, 0.5f) * VertexDisturbance) / VertexSamples, 0.0f, 1.0f);
			const float Y = FMath::Clamp((((float)SampleY) + 0.5f + Random.FRandRange(-0.5f, 0.5f) * VertexDisturbance) / VertexSamples, 0.0f, 1.0f);
			Samples.Emplace(FVector2D(X, Y));
		}
	}

	// Create triangulation
	Triangulation.QHull(Samples, -1);
	Triangulation.FixTriangles(-1);

	GenerateTriangulationDone = true;
}

void ATGOR_WorldPainterLayer::GeneratePoints()
{
	Points.Empty();

	ATGOR_WorldPainterCanvas* Canvas = GetPainterCanvas();
	if (IsValid(Canvas))
	{
		FTransform Transform = Canvas->GetActorTransform();

		TArray<AActor*> Actors;
		GetAttachedActors(Actors, true);
		for (AActor* Actor : Actors)
		{
			ATGOR_WorldPainterBrush* Brush = Cast<ATGOR_WorldPainterBrush>(Actor);
			if (IsValid(Brush))
			{
				const FVector Local = Transform.InverseTransformPosition(Brush->GetActorLocation());

				FTGOR_BrushPoint Point;
				Point.Variance = Brush->Size;
				Point.Position = FVector2D(Local + FVector::OneVector) / 2;
				Point.Data = Brush->Brush;
				Points.Emplace(Point);
			}
		}
	}

	GeneratePointsDone = true;
}

void ATGOR_WorldPainterLayer::GenerateVertices()
{
	const int32 PointCount = Points.Num();
	const int32 SliceNum = Bias.Slices.Num();
	const int32 VertexCount = Triangulation.Points.Num();

	Vertices.SetNum(VertexCount);

	const float BiasWeight = FMath::Exp(-Dispersion);
	for (int32 VertexIndex = 0; VertexIndex < VertexCount; VertexIndex++)
	{
		const FVector2D& Origin = Triangulation.Points[VertexIndex];

		FTGOR_BrushData& Vertex = Vertices[VertexIndex];
		Vertex = FTGOR_BrushData(SliceNum, FLinearColor(0.f, 0.f, 0.f, 0.f));
		Vertex.AddWeighted(Bias, BiasWeight);
		for (int32 PointIndex = 0; PointIndex < PointCount; PointIndex++)
		{
			const FTGOR_BrushPoint& Point = Points[PointIndex];

			const float DS = (Point.Position - Origin).SizeSquared();
			const float Weight = INV_SQRTWOPI / Point.Variance * FMath::Exp(DS / (Point.Variance * Point.Variance * -2));
			Vertex.AddWeighted(Point.Data, Weight);
		}
		Vertex.Normalize();
	}

	GenerateVerticesDone = true;
}

void ATGOR_WorldPainterLayer::GenerateRendertarget()
{
	const int32 SliceNum = Bias.Slices.Num();
	const int32 PixelSize = (1 << TextureSize);

	// Generate image data for each slice
	RawData.SetNumZeroed(SliceNum);
	for (int32 Slice = 0; Slice < SliceNum; Slice++)
	{
		RawData[Slice].SetNumZeroed(PixelSize * PixelSize);
	}

	CacheRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, PixelSize, PixelSize, ETextureRenderTargetFormat::RTF_RGBA8, FLinearColor::Black, false);
	GenerateRendertargetDone = true;
}

void ATGOR_WorldPainterLayer::GenerateTexture()
{
	const int32 TriangleCount = Triangulation.Triangles.Num();
	if (IsValid(RenderMaterial))
	{
		TArray<FCanvasUVTri> Tris;
		Tris.SetNum(TriangleCount);

		const int32 SliceNum = Bias.Slices.Num();
		for (int32 Slice = 0; Slice < SliceNum; Slice++)
		{
			UCanvas* Canvas;
			FVector2D Size;
			BeginDrawCanvasToRenderTargetVolume(Canvas, Size);

			if (Canvas)
			{
				for (int32 TriangleIndex = 0; TriangleIndex < TriangleCount; TriangleIndex++)
				{
					const FTGOR_Triangle& Triangle = Triangulation.Triangles[TriangleIndex];

					FCanvasUVTri& Tri = Tris[TriangleIndex];

					const FVector2D& P0 = Triangulation.Points[Triangle.Verts[0]];
					const FVector2D& P1 = Triangulation.Points[Triangle.Verts[1]];
					const FVector2D& P2 = Triangulation.Points[Triangle.Verts[2]];

					const FLinearColor& V0 = Vertices[Triangle.Verts[0]].Get(Slice);
					const FLinearColor& V1 = Vertices[Triangle.Verts[1]].Get(Slice);
					const FLinearColor& V2 = Vertices[Triangle.Verts[2]].Get(Slice);

					const FLinearColor Mean = (V0 + V1 + V2) / 3;

					Tri.V0_Pos = P0 * Size;
					Tri.V0_UV = P0;
					Tri.V0_Color = FMath::Lerp(Mean, V0, Blending);

					Tri.V1_Pos = P1 * Size;
					Tri.V1_UV = P1;
					Tri.V1_Color = FMath::Lerp(Mean, V1, Blending);

					Tri.V2_Pos = P2 * Size;
					Tri.V2_UV = P2;
					Tri.V2_Color = FMath::Lerp(Mean, V2, Blending);
				}

				Canvas->K2_DrawMaterialTriangle(RenderMaterial, Tris);
				EndDrawCanvasToRenderTargetVolume();

				FTextureRenderTargetResource* RenderCacheResource = CacheRenderTarget->GameThread_GetRenderTargetResource();
				RenderCacheResource->ReadPixels(RawData[Slice]);
			}
		}

		const int32 PixelSize = (1 << TextureSize);
		if (PixelSize > 0)
		{
			if (IsValid(TargetTexture))
			{
				const int32 FormatDataSize = FTextureSource::GetBytesPerPixel(ETextureSourceFormat::TSF_BGRA8);
				TargetTexture->UpdateSourceFromFunction([this, FormatDataSize, PixelSize](int32 X, int32 Y, int32 Z, void* Data) {

					FMemory::Memcpy(Data, &RawData[Z][X + Y * PixelSize], FormatDataSize);

				}, PixelSize, PixelSize, SliceNum, ETextureSourceFormat::TSF_BGRA8);

				TargetTexture->PostEditChange();
				TargetTexture->MarkPackageDirty();
			}

			if (IsValid(TargetTexture2D))
			{
				CacheRenderTarget->UpdateTexture2D(TargetTexture2D, ETextureSourceFormat::TSF_BGRA8);
			}
		}
		//TargetTexture->BuildTexture(RawData, PixelSize);


		/*
		@todo: Somehow we can directly render to a volume RT via GPU, the following just does nothing
		if (TargetTexture && TargetTexture->Resource)
		{
			FTextureRenderTargetResource* RenderCacheResource = CacheRenderTargets[0]->GameThread_GetRenderTargetResource();
			ENQUEUE_RENDER_COMMAND(CopyCommand)([RenderCacheResource, this](FRHICommandList& RHICmdList) {

				RHICmdList.CopyTexture(RenderCacheResource->GetRenderTargetTexture(), TargetTexture->Resource->TextureRHI, FRHICopyTextureInfo());
			});

			FTextureRenderTargetResource* RenderVolumeResource = VolumeRenderTarget->GameThread_GetRenderTargetResource();
			ENQUEUE_RENDER_COMMAND(VolumeCopyCommand)([RenderCacheResource, RenderVolumeResource](FRHICommandListImmediate& RHICmdList) {

				RenderVolumeResource->FlushDeferredResourceUpdate(RHICmdList);
				RHICmdList.CopyTexture(RenderCacheResource->TextureRHI, RenderVolumeResource->TextureRHI, FRHICopyTextureInfo());
			});
		}
		*/
	}
	GenerateTextureDone = true;
}
#endif // WITH_EDITOR


void ATGOR_WorldPainterLayer::BeginDrawCanvasToRenderTargetVolume(UCanvas*& Canvas, FVector2D& Size)
{
	Canvas = NULL;
	Size = FVector2D(0, 0);

	if (!FApp::CanEverRender())
	{
		// Returning early to avoid warnings about missing resources that are expected when CanEverRender is false.
		return;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
	if (World && CacheRenderTarget && CacheRenderTarget->Resource)
	{
		World->FlushDeferredParameterCollectionInstanceUpdates();

		Canvas = World->GetCanvasForRenderingToTarget();

		Size = FVector2D(CacheRenderTarget->SizeX, CacheRenderTarget->SizeY);

		FTextureRenderTargetResource* RenderCacheResource = CacheRenderTarget->GameThread_GetRenderTargetResource();
		FCanvas* NewCanvas = new FCanvas(
			RenderCacheResource,
			nullptr,
			World,
			World->FeatureLevel,
			// Draw immediately so that interleaved SetVectorParameter (etc) function calls work as expected
			FCanvas::CDM_ImmediateDrawing);
		Canvas->Init(CacheRenderTarget->SizeX, CacheRenderTarget->SizeY, nullptr, NewCanvas);
		Canvas->Update();

		ContextDrawEvent = new FDrawEvent();

		FName RTName = CacheRenderTarget->GetFName();
		FDrawEvent* DrawEvent = ContextDrawEvent;
		ENQUEUE_RENDER_COMMAND(BeginDrawEventCommand)(
			[RTName, DrawEvent, RenderCacheResource](FRHICommandListImmediate& RHICmdList)
		{
			RenderCacheResource->FlushDeferredResourceUpdate(RHICmdList);

			BEGIN_DRAW_EVENTF(
				RHICmdList,
				DrawCanvasToTarget,
				(*DrawEvent),
				*RTName.ToString());
		});
	}
}

void ATGOR_WorldPainterLayer::EndDrawCanvasToRenderTargetVolume()
{
	if (!FApp::CanEverRender())
	{
		// Returning early to avoid warnings about missing resources that are expected when CanEverRender is false.
		return;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		UCanvas* WorldCanvas = World->GetCanvasForRenderingToTarget();
		if (WorldCanvas->Canvas)
		{
			WorldCanvas->Canvas->Flush_GameThread();
			delete WorldCanvas->Canvas;
			WorldCanvas->Canvas = nullptr;
		}

		if (CacheRenderTarget)
		{
			FTextureRenderTargetResource* RenderCacheResource = CacheRenderTarget->GameThread_GetRenderTargetResource();

			FDrawEvent* DrawEvent = ContextDrawEvent;
			ENQUEUE_RENDER_COMMAND(CanvasRenderTargetResolveCommand)([RenderCacheResource, DrawEvent](FRHICommandList& RHICmdList) {

				RHICmdList.CopyToResolveTarget(RenderCacheResource->GetRenderTargetTexture(), RenderCacheResource->TextureRHI, FResolveParams());

				STOP_DRAW_EVENT((*DrawEvent));
				delete DrawEvent;
			});
		}
	}
}


