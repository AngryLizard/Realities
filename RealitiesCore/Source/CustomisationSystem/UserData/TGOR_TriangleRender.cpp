// The Gateway of Realities: Planes of Existence.

#include "TGOR_TriangleRender.h"

#include "Engine/Canvas.h"

#include "ShowFlags.h"
#include "RHI.h"
#include "RenderResource.h"
#include "RenderingThread.h"
#include "VertexFactory.h"
#include "LocalVertexFactory.h"
#include "SceneView.h"
#include "CanvasTypes.h"
#include "MeshBatch.h"
#include "RendererInterface.h"
#include "EngineModule.h"
#include "MeshPassProcessor.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_TriangleVertexFactory::FTGOR_TriangleVertexFactory(
	const FStaticMeshVertexBuffers* InVertexBuffers,
	ERHIFeatureLevel::Type InFeatureLevel)
	: FLocalVertexFactory(InFeatureLevel, "FTriangleVertexFactory")
	, VertexBuffers(InVertexBuffers)
{}

void FTGOR_TriangleVertexFactory::InitResource()
{
	FLocalVertexFactory::FDataType VertexData;
	VertexBuffers->PositionVertexBuffer.BindPositionVertexBuffer(this, VertexData);
	VertexBuffers->StaticMeshVertexBuffer.BindTangentVertexBuffer(this, VertexData);
	VertexBuffers->StaticMeshVertexBuffer.BindPackedTexCoordVertexBuffer(this, VertexData);
	VertexBuffers->StaticMeshVertexBuffer.BindLightMapVertexBuffer(this, VertexData, 0);
	VertexBuffers->ColorVertexBuffer.BindColorVertexBuffer(this, VertexData);
	SetData(VertexData);

	FLocalVertexFactory::InitResource();
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_MergeFTriangleMesh::FTGOR_MergeFTriangleMesh(
	const FRawIndexBuffer* InIndexBuffer,
	const FTGOR_TriangleVertexFactory* InVertexFactory)
	: IndexBuffer(InIndexBuffer)
	, VertexFactory(InVertexFactory)
{}


void FTGOR_MergeFTriangleMesh::InitRHI()
{
	MeshBatch.VertexFactory = VertexFactory;
	MeshBatch.ReverseCulling = false;
	MeshBatch.bDisableBackfaceCulling = true;
	MeshBatch.Type = PT_TriangleList;
	MeshBatch.DepthPriorityGroup = SDPG_Foreground;

	FMeshBatchElement& MeshBatchElement = MeshBatch.Elements[0];
	MeshBatchElement.IndexBuffer = IndexBuffer;
	MeshBatchElement.FirstIndex = 0;
	MeshBatchElement.NumPrimitives = 1;
	MeshBatchElement.MinVertexIndex = 0;
	MeshBatchElement.MaxVertexIndex = 2;
	MeshBatchElement.PrimitiveUniformBufferResource = &GIdentityPrimitiveUniformBuffer;
}

void FTGOR_TriangleRenderData::InitTriangleMesh(const FSceneView& View, bool bNeedsToSwitchVerticalAxis)
{
	StaticMeshVertexBuffers.PositionVertexBuffer.Init(Triangles.Num() * 3);
	StaticMeshVertexBuffers.StaticMeshVertexBuffer.Init(Triangles.Num() * 3, 3);
	StaticMeshVertexBuffers.ColorVertexBuffer.Init(Triangles.Num() * 3);

	IndexBuffer.Indices.SetNum(Triangles.Num() * 3);

	for (int32 i = 0; i < Triangles.Num(); i++)
	{
		const uint32 StartIndex = i * 3;

		/** The use of an index buffer here is actually necessary to workaround an issue with BaseVertexIndex, DrawPrimitive, and manual vertex fetch.
		 *  In short, DrawIndexedPrimitive with StartIndex map SV_VertexId to the correct location, but DrawPrimitive with BaseVertexIndex will not.
		 */
		IndexBuffer.Indices[StartIndex + 0] = StartIndex + 0;
		IndexBuffer.Indices[StartIndex + 1] = StartIndex + 1;
		IndexBuffer.Indices[StartIndex + 2] = StartIndex + 2;

		const FTGOR_MergeUVTri& Tri = Triangles[i];

		// create verts. Notice the order is (1, 0, 2)
		if (bNeedsToSwitchVerticalAxis)
		{
			StaticMeshVertexBuffers.PositionVertexBuffer.VertexPosition(StartIndex + 0) = FVector(Tri.V1.Pos.X, View.UnscaledViewRect.Height() - Tri.V1.Pos.Y, Tri.V1.Pos.Z);
			StaticMeshVertexBuffers.PositionVertexBuffer.VertexPosition(StartIndex + 1) = FVector(Tri.V0.Pos.X, View.UnscaledViewRect.Height() - Tri.V0.Pos.Y, Tri.V0.Pos.Z);
			StaticMeshVertexBuffers.PositionVertexBuffer.VertexPosition(StartIndex + 2) = FVector(Tri.V2.Pos.X, View.UnscaledViewRect.Height() - Tri.V2.Pos.Y, Tri.V2.Pos.Z);
		}
		else
		{
			StaticMeshVertexBuffers.PositionVertexBuffer.VertexPosition(StartIndex + 0) = FVector(Tri.V1.Pos.X, Tri.V1.Pos.Y, Tri.V1.Pos.Z);
			StaticMeshVertexBuffers.PositionVertexBuffer.VertexPosition(StartIndex + 1) = FVector(Tri.V0.Pos.X, Tri.V0.Pos.Y, Tri.V0.Pos.Z);
			StaticMeshVertexBuffers.PositionVertexBuffer.VertexPosition(StartIndex + 2) = FVector(Tri.V2.Pos.X, Tri.V2.Pos.Y, Tri.V2.Pos.Z);
		}

		StaticMeshVertexBuffers.StaticMeshVertexBuffer.SetVertexTangents(StartIndex + 0, FVector(Tri.V1.Tangent), Tri.V1.Bitangent, Tri.V1.Normal());
		StaticMeshVertexBuffers.StaticMeshVertexBuffer.SetVertexTangents(StartIndex + 1, FVector(Tri.V0.Tangent), Tri.V0.Bitangent, Tri.V0.Normal());
		StaticMeshVertexBuffers.StaticMeshVertexBuffer.SetVertexTangents(StartIndex + 2, FVector(Tri.V2.Tangent), Tri.V2.Bitangent, Tri.V2.Normal());

		StaticMeshVertexBuffers.StaticMeshVertexBuffer.SetVertexUV(StartIndex + 0, 0, FVector2D(Tri.V1.UV[0].X, Tri.V1.UV[0].Y));
		StaticMeshVertexBuffers.StaticMeshVertexBuffer.SetVertexUV(StartIndex + 1, 0, FVector2D(Tri.V0.UV[0].X, Tri.V0.UV[0].Y));
		StaticMeshVertexBuffers.StaticMeshVertexBuffer.SetVertexUV(StartIndex + 2, 0, FVector2D(Tri.V2.UV[0].X, Tri.V2.UV[0].Y));

		StaticMeshVertexBuffers.StaticMeshVertexBuffer.SetVertexUV(StartIndex + 0, 1, FVector2D(Tri.V1.UV[1].X, Tri.V1.UV[1].Y));
		StaticMeshVertexBuffers.StaticMeshVertexBuffer.SetVertexUV(StartIndex + 1, 1, FVector2D(Tri.V0.UV[1].X, Tri.V0.UV[1].Y));
		StaticMeshVertexBuffers.StaticMeshVertexBuffer.SetVertexUV(StartIndex + 2, 1, FVector2D(Tri.V2.UV[1].X, Tri.V2.UV[1].Y));

		StaticMeshVertexBuffers.StaticMeshVertexBuffer.SetVertexUV(StartIndex + 0, 2, FVector2D(Tri.V1.UV[2].X, Tri.V1.UV[2].Y));
		StaticMeshVertexBuffers.StaticMeshVertexBuffer.SetVertexUV(StartIndex + 1, 2, FVector2D(Tri.V0.UV[2].X, Tri.V0.UV[2].Y));
		StaticMeshVertexBuffers.StaticMeshVertexBuffer.SetVertexUV(StartIndex + 2, 2, FVector2D(Tri.V2.UV[2].X, Tri.V2.UV[2].Y));

		StaticMeshVertexBuffers.ColorVertexBuffer.VertexColor(StartIndex + 0) = Tri.V1.Color.ToFColor(false);
		StaticMeshVertexBuffers.ColorVertexBuffer.VertexColor(StartIndex + 1) = Tri.V0.Color.ToFColor(false);
		StaticMeshVertexBuffers.ColorVertexBuffer.VertexColor(StartIndex + 2) = Tri.V2.Color.ToFColor(false);
	}

	StaticMeshVertexBuffers.PositionVertexBuffer.InitResource();
	StaticMeshVertexBuffers.StaticMeshVertexBuffer.InitResource();
	StaticMeshVertexBuffers.ColorVertexBuffer.InitResource();
	IndexBuffer.InitResource();
	VertexFactory.InitResource();
	TriMesh.InitResource();
};

#if WITH_EDITOR
void FRawIndexBuffer::InitRHI()
{
	uint32 Size = Indices.Num() * sizeof(uint16);
	if (Size > 0)
	{
		// Create the index buffer.
		FRHIResourceCreateInfo CreateInfo(TEXT("RawIndexBuffer"));
		void* Buffer = nullptr;
		IndexBufferRHI = RHICreateAndLockIndexBuffer(sizeof(uint16), Size, BUF_Static, CreateInfo, Buffer);

		// Initialize the buffer.		
		FMemory::Memcpy(Buffer, Indices.GetData(), Size);
		RHIUnlockBuffer(IndexBufferRHI);
	}
}
#endif

/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FTGOR_TriangleRenderData::ReleaseTriangleMesh()
{
	TriMesh.ReleaseResource();
	VertexFactory.ReleaseResource();
	IndexBuffer.ReleaseResource();
	StaticMeshVertexBuffers.PositionVertexBuffer.ReleaseResource();
	StaticMeshVertexBuffers.StaticMeshVertexBuffer.ReleaseResource();
	StaticMeshVertexBuffers.ColorVertexBuffer.ReleaseResource();
}

void FTGOR_TriangleRenderData::RenderTriangles(
	const FMaterialRenderProxy* InMaterialRenderProxy,
	const FHitProxyId& HitProxyId,
	const FCanvas::FTransformEntry& InTransform,
	FRHICommandListImmediate& RHICmdList,
	FMeshPassProcessorRenderState& DrawRenderState,
	const FSceneView& View,
	bool bIsHitTesting,
	bool bNeedsToSwitchVerticalAxis)
{
	check(IsInRenderingThread());

	IRendererModule& RendererModule = GetRendererModule();

	InitTriangleMesh(View, bNeedsToSwitchVerticalAxis);

	const FMaterial& Material = InMaterialRenderProxy->GetIncompleteMaterialWithFallback(GMaxRHIFeatureLevel);
	SCOPED_DRAW_EVENTF(RHICmdList, CanvasDrawTriangles, *Material.GetFriendlyName());

	
	FMeshBatch& MeshBatch = TriMesh.MeshBatch;
	MeshBatch.VertexFactory = &VertexFactory;
	MeshBatch.MaterialRenderProxy = InMaterialRenderProxy;
	MeshBatch.Elements[0].FirstIndex = 0;
	MeshBatch.Elements[0].NumPrimitives = Triangles.Num();

	//GetRendererModule().DrawTileMesh(RHICmdList, DrawRenderState, View, MeshBatch, bIsHitTesting, HitProxyId);

	ReleaseTriangleMesh();
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
bool FTGOR_TriangleRendererItem::Render_RenderThread(FRHICommandListImmediate& RHICmdList, FMeshPassProcessorRenderState& DrawRenderState, const FCanvas* Canvas)
{
	float CurrentRealTime = 0.f;
	float CurrentWorldTime = 0.f;
	float DeltaWorldTime = 0.f;

	if (!bFreezeTime)
	{
		CurrentRealTime = Canvas->GetCurrentRealTime();
		CurrentWorldTime = Canvas->GetCurrentWorldTime();
		DeltaWorldTime = Canvas->GetCurrentDeltaWorldTime();
	}

	checkSlow(Data);

	const FRenderTarget* CanvasRenderTarget = Canvas->GetRenderTarget();

	TUniquePtr<const FSceneViewFamily> ViewFamily = MakeUnique<const FSceneViewFamily>(FSceneViewFamily::ConstructionValues(
		CanvasRenderTarget,
		nullptr,
		FEngineShowFlags(ESFIM_Game))
		.SetWorldTimes(CurrentWorldTime, DeltaWorldTime, CurrentRealTime)
		.SetGammaCorrection(CanvasRenderTarget->GetDisplayGamma()));

	const FIntRect ViewRect(FIntPoint(0, 0), CanvasRenderTarget->GetSizeXY());

	// make a temporary view
	FSceneViewInitOptions ViewInitOptions;
	ViewInitOptions.ViewFamily = ViewFamily.Get();
	ViewInitOptions.SetViewRectangle(ViewRect);
	ViewInitOptions.ViewOrigin = FVector::ZeroVector;
	ViewInitOptions.ViewRotationMatrix = FMatrix::Identity;
	ViewInitOptions.ProjectionMatrix = Transform.GetMatrix();
	ViewInitOptions.BackgroundColor = FLinearColor::Black;
	ViewInitOptions.OverlayColor = FLinearColor::White;

	TUniquePtr<const FSceneView> View = MakeUnique<const FSceneView>(ViewInitOptions);

	const bool bNeedsToSwitchVerticalAxis = RHINeedsToSwitchVerticalAxis(Canvas->GetShaderPlatform()) && Canvas->GetAllowSwitchVerticalAxis();

	Data->RenderTriangles(MaterialRenderProxy, HitProxyId, Transform, RHICmdList, DrawRenderState, *View, Canvas->IsHitTesting(), bNeedsToSwitchVerticalAxis);

	if (Canvas->GetAllowedModes() & FCanvas::Allow_DeleteOnRender)
	{
		//Data = nullptr;
	}

	return true;
}
*/

/*
bool FTGOR_TriangleRendererItem::Render_GameThread(const FCanvas* Canvas, FRenderThreadScope& RenderScope)
{
	float CurrentRealTime = 0.f;
	float CurrentWorldTime = 0.f;
	float DeltaWorldTime = 0.f;

	if (!bFreezeTime)
	{
		CurrentRealTime = Canvas->GetCurrentRealTime();
		CurrentWorldTime = Canvas->GetCurrentWorldTime();
		DeltaWorldTime = Canvas->GetCurrentDeltaWorldTime();
	}

	checkSlow(Data);

	const FRenderTarget* CanvasRenderTarget = Canvas->GetRenderTarget();

	const FSceneViewFamily* ViewFamily = new FSceneViewFamily(FSceneViewFamily::ConstructionValues(
		CanvasRenderTarget,
		Canvas->GetScene(),
		FEngineShowFlags(ESFIM_Game))
		.SetWorldTimes(CurrentWorldTime, DeltaWorldTime, CurrentRealTime)
		.SetGammaCorrection(CanvasRenderTarget->GetDisplayGamma()));

	const FIntRect ViewRect(FIntPoint(0, 0), CanvasRenderTarget->GetSizeXY());

	// make a temporary view
	FSceneViewInitOptions ViewInitOptions;
	ViewInitOptions.ViewFamily = ViewFamily;
	ViewInitOptions.SetViewRectangle(ViewRect);
	ViewInitOptions.ViewOrigin = FVector::ZeroVector;
	ViewInitOptions.ViewRotationMatrix = FMatrix::Identity;
	ViewInitOptions.ProjectionMatrix = Transform.GetMatrix();
	ViewInitOptions.BackgroundColor = FLinearColor::Black;
	ViewInitOptions.OverlayColor = FLinearColor::White;

	const FSceneView* View = new FSceneView(ViewInitOptions);

	const bool bNeedsToSwitchVerticalAxis = RHINeedsToSwitchVerticalAxis(Canvas->GetShaderPlatform()) && Canvas->GetAllowSwitchVerticalAxis();
	const bool bIsHitTesting = Canvas->IsHitTesting();
	const bool bDeleteOnRender = Canvas->GetAllowedModes() & FCanvas::Allow_DeleteOnRender;

	const FMaterialRenderProxy* InMaterialRenderProxy = MaterialRenderProxy;
	const FCanvas::FTransformEntry InTransform = Transform;
	FHitProxyId InHitProxyId = HitProxyId;

	RenderScope.EnqueueRenderCommand(
		[LocalData = Data, View, bIsHitTesting, bNeedsToSwitchVerticalAxis, InMaterialRenderProxy, InTransform, InHitProxyId](FRHICommandListImmediate& RHICmdList)
	{
		FMeshPassProcessorRenderState DrawRenderState(*View);

		// disable depth test & writes
		DrawRenderState.SetDepthStencilState(TStaticDepthStencilState<false, CF_Always>::GetRHI());

		LocalData->RenderTriangles(InMaterialRenderProxy, InHitProxyId, InTransform, RHICmdList, DrawRenderState, *View, bIsHitTesting, bNeedsToSwitchVerticalAxis);

		delete View->Family;
		delete View;
	});

	if (Canvas->GetAllowedModes() & FCanvas::Allow_DeleteOnRender)
	{
		//Data = nullptr;
	}

	return true;
}
*/

/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_TriangleItem::FTGOR_TriangleItem(UTGOR_GeometryUserData* Geometry, int32 SectionIndex, const FTransform& PointTransform, const FMaterialRenderProxy* MaterialRenderProxy)
	:FCanvasItem(FVector2D::ZeroVector),
	MaterialRenderProxy(MaterialRenderProxy),
	BatchedElementParameters(nullptr),
	Geometry(Geometry),
	SectionIndex(SectionIndex),
	PointTransform(PointTransform)
{
};

FTGOR_TriangleItem::~FTGOR_TriangleItem()
{
	Data = nullptr;
}

void FTGOR_TriangleItem::Draw(FCanvas* InCanvas)
{
	if (IsValid(Geometry) && Geometry->RenderSections.IsValidIndex(SectionIndex))
	{
		FTGOR_RenderGeometrySection& RenderSection = Geometry->RenderSections[SectionIndex];
		const int32 NumTriangles = RenderSection.Triangles.Num();
		if (NumTriangles > 0)
		{
			// get sort element based on the current sort key from top of sort key stack
			FCanvas::FCanvasSortElement& SortElement = InCanvas->GetSortElement(InCanvas->TopDepthSortKey());

			// find a batch to use 
			FTGOR_TriangleRendererItem* RenderBatch = nullptr;

			// get the current transform entry from top of transform stack
			FCanvas::FTransformEntry TopTransformEntry = InCanvas->GetTransformStack().Top();

			FMatrix Matrix(PointTransform.TransformVector(FVector::ForwardVector), PointTransform.TransformVector(FVector::RightVector), PointTransform.TransformVector(FVector::UpVector), PointTransform.TransformPosition(FVector::ZeroVector));
			TopTransformEntry.SetMatrix(Matrix * TopTransformEntry.GetMatrix());

			// Create and cache data
			if (!Data.IsValid())
			{
				Data = MakeShared<FTGOR_TriangleRenderData>(InCanvas->GetFeatureLevel());

				// add the triangles to the triangle render batch
				Data->ReserveTriangles(NumTriangles);
				for (int32 i = 0; i < NumTriangles; i++)
				{
					Data->AddTriangle(RenderSection.Triangles[i]);
				}
			}

			// TODO: Look for top batch entry

			// if a matching entry for this batch doesn't exist then allocate a new entry
			if (RenderBatch == nullptr || !RenderBatch->IsMatch(MaterialRenderProxy, TopTransformEntry))
			{
				FHitProxyId HitProxyId = InCanvas->GetHitProxyId();
				//RenderBatch = new FTGOR_TriangleRendererItem(InCanvas->GetFeatureLevel(), MaterialRenderProxy, TopTransformEntry, HitProxyId, Data.ToSharedRef(), bFreezeTime);
				//SortElement.RenderBatchArray.Add(RenderBatch);
			}
		}
	}
}