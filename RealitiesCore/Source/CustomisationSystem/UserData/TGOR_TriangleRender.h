// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "TGOR_GeometryUserData.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"


class FMaterialRenderProxy;


/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class FTGOR_TriangleVertexFactory : public FLocalVertexFactory
{
public:
	FTGOR_TriangleVertexFactory(const FStaticMeshVertexBuffers* VertexBuffers, ERHIFeatureLevel::Type InFeatureLevel);
	void InitResource() override;

private:
	const FStaticMeshVertexBuffers* VertexBuffers;
};

/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class FTGOR_MergeFTriangleMesh : public FRenderResource
{
public:
	FTGOR_MergeFTriangleMesh(const FRawIndexBuffer* IndexBuffer, const FTGOR_TriangleVertexFactory* VertexFactory);

	FMeshBatch MeshBatch;
	virtual void InitRHI() override;
private:
	const FRawIndexBuffer* IndexBuffer;
	const FTGOR_TriangleVertexFactory* VertexFactory;
};

/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class FTGOR_TriangleRenderData
{
public:
	FTGOR_TriangleRenderData(ERHIFeatureLevel::Type InFeatureLevel)
		: VertexFactory(&StaticMeshVertexBuffers, InFeatureLevel)
		, TriMesh(&IndexBuffer, &VertexFactory)
	{}

	FORCEINLINE int32 AddTriangle(const FTGOR_MergeUVTri& Tri)
	{
		return Triangles.Add(Tri);
	};

	FORCEINLINE void AddReserveTriangles(int32 NumTriangles)
	{
		Triangles.Reserve(Triangles.Num() + NumTriangles);
	}

	FORCEINLINE void ReserveTriangles(int32 NumTriangles)
	{
		Triangles.Reserve(NumTriangles);
	}

	void RenderTriangles(
		const FMaterialRenderProxy* InMaterialRenderProxy,
		const FHitProxyId& HitProxyId,
		const FCanvas::FTransformEntry& InTransform,
		FRHICommandListImmediate& RHICmdList,
		FMeshPassProcessorRenderState& DrawRenderState,
		const FSceneView& View,
		bool bIsHitTesting,
		bool bNeedsToSwitchVerticalAxis);

private:
	void InitTriangleMesh(const FSceneView& View, bool bNeedsToSwitchVerticalAxis);
	void ReleaseTriangleMesh();

	FRawIndexBuffer IndexBuffer;
	FStaticMeshVertexBuffers StaticMeshVertexBuffers;
	FTGOR_TriangleVertexFactory VertexFactory;
	FTGOR_MergeFTriangleMesh TriMesh;

	TArray<FTGOR_MergeUVTri> Triangles;
};

/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Info needed to render a single FTriangleRenderer
*/
class FTGOR_TriangleRendererItem : public FCanvasBaseRenderItem
{
public:
	/**
	* Init constructor
	*/
	FTGOR_TriangleRendererItem(ERHIFeatureLevel::Type InFeatureLevel,
		const FMaterialRenderProxy* InMaterialRenderProxy,
		const FCanvas::FTransformEntry& InTransform,
		const FHitProxyId& HitProxyId,
		TSharedRef<FTGOR_TriangleRenderData> Data,
		bool bInFreezeTime = false)
		// this data is deleted after rendering has completed
		: Data(Data)
		, MaterialRenderProxy(InMaterialRenderProxy)
		, Transform(InTransform)
		, HitProxyId(HitProxyId)
		, bFreezeTime(bInFreezeTime)
	{}

	/**
	 * FCanvasTriangleRendererItem instance accessor
	 *
	 * @return this instance
	 */
	virtual class FCanvasTriangleRendererItem* GetCanvasTriangleRendererItem() override
	{
		return nullptr;
	}

	/**
	* Renders the canvas item.
	* Iterates over each triangle to be rendered and draws it with its own transforms
	*
	* @param Canvas - canvas currently being rendered
	* @param RHICmdList - command list to use
	* @return true if anything rendered
	*/
	virtual bool Render_RenderThread(FRHICommandListImmediate& RHICmdList, FMeshPassProcessorRenderState& DrawRenderState, const FCanvas* Canvas) override;

	/**
	* Renders the canvas item.
	* Iterates over each triangle to be rendered and draws it with its own transforms
	*
	* @param Canvas - canvas currently being rendered
	* @return true if anything rendered
	*/
	virtual bool Render_GameThread(const FCanvas* Canvas, FRenderThreadScope& RenderScope) override;

	/**
	* Determine if this is a matching set by comparing material,transform. All must match
	*
	* @param IInMaterialRenderProxy - material proxy resource for the item being rendered
	* @param InTransform - the transform for the item being rendered
	* @return true if the parameters match this render item
	*/
	bool IsMatch(const FMaterialRenderProxy* InMaterialRenderProxy, const FCanvas::FTransformEntry& InTransform)
	{
		return(MaterialRenderProxy == InMaterialRenderProxy);
	};

private:
	

	/**
	 * Render data which is allocated when a new FCanvasTriangleRendererItem is added for rendering.
	 * This data is only freed on the rendering thread once the item has finished rendering
	 */
	TSharedRef<FTGOR_TriangleRenderData> Data;
	const FMaterialRenderProxy* MaterialRenderProxy;
	const FCanvas::FTransformEntry Transform;
	FHitProxyId HitProxyId;

	const bool bFreezeTime;
};


/// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CUSTOMISATIONSYSTEM_API FTGOR_TriangleItem : public FCanvasItem
{
public:

	FTGOR_TriangleItem(UTGOR_GeometryUserData* Geometry, int32 SectionIndex, const FTransform& PointTransform, const FMaterialRenderProxy* MaterialRenderProxy);
	virtual ~FTGOR_TriangleItem();

	virtual void Draw(FCanvas* InCanvas) override;

	/* Expose the functions defined in the base class. */
	using FCanvasItem::Draw;

	/* Material proxy for rendering. */
	const FMaterialRenderProxy* MaterialRenderProxy;

	FBatchedElementParameters* BatchedElementParameters;

	/* List of triangles. */
	UTGOR_GeometryUserData* Geometry;

	int32 SectionIndex;

	/* Point transform for rendering */
	FTransform PointTransform;

	TSharedPtr<class FTGOR_TriangleRenderData> Data;
};
