// The Gateway of Realities: Planes of Existence.

#include "TGOR_WorldPainterCanvas.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "RealitiesUtility/Utility/TGOR_Math.h"
#include "UObject/ConstructorHelpers.h"

#include "Engine/EngineTypes.h"
#include "TGOR_WorldPainterLayer.h"
#include "Components/BoxComponent.h"
#include "Math/BoxSphereBounds.h"
#include "EngineUtils.h"

#if WITH_EDITOR
#include "Editor.h"
#endif

ATGOR_WorldPainterCanvas::ATGOR_WorldPainterCanvas(const FObjectInitializer& ObjectInitializer)
:	Super(ObjectInitializer),
	BoundsActor(nullptr)
{
	bIsEditorOnlyActor = true;

	USceneComponent* Root = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, FName(TEXT("Root")));
	Root->SetMobility(EComponentMobility::Static);
	SetRootComponent(Root);

#if WITH_EDITORONLY_DATA
	// Add box for visualization of bounds
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Bounds"));
	Box->SetBoxExtent(FVector(1.f, 1.f, 1.f), false);
	Box->SetRelativeTransform(FTransform(FVector(100.f, 100.f, 100.f)));
	Box->bDrawOnlyIfSelected = true;
	Box->SetIsVisualizationComponent(true);
	Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Box->SetCanEverAffectNavigation(false);
	Box->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	Box->SetGenerateOverlapEvents(false);
	Box->SetupAttachment(Root);
#endif
}

void ATGOR_WorldPainterCanvas::CopyBounds()
{
#if WITH_EDITORONLY_DATA
	if (IsValid(BoundsActor))
	{
		FVector Origin, BoxExtent;
		BoundsActor->GetActorBounds(false, Origin, BoxExtent, false);

		// Apply final result and notify the parent actor
		Modify();
		SetActorTransform(FTransform(FQuat::Identity, Origin, BoxExtent));
		PostEditMove(true);

		GEditor->NoteSelectionChange(false);
	}
#endif
}


/*
#if WITH_EDITOR
#include "StaticMeshAttributes.h"
#include "Engine/StaticMeshActor.h"
#endif

#include "ProceduralMeshComponent/Public/ProceduralMeshComponent.h"

void ATGOR_WorldPainterCanvas::GenerateMesh()
{
	FMeshDescription MeshDescription;

	FStaticMeshAttributes AttributeGetter(MeshDescription);
	AttributeGetter.Register();

	TPolygonGroupAttributesRef<FName> PolygonGroupNames = AttributeGetter.GetPolygonGroupMaterialSlotNames();
	TVertexAttributesRef<FVector> VertexPositions = AttributeGetter.GetVertexPositions();
	TVertexInstanceAttributesRef<FVector> Tangents = AttributeGetter.GetVertexInstanceTangents();
	TVertexInstanceAttributesRef<float> BinormalSigns = AttributeGetter.GetVertexInstanceBinormalSigns();
	TVertexInstanceAttributesRef<FVector> Normals = AttributeGetter.GetVertexInstanceNormals();
	TVertexInstanceAttributesRef<FVector4> Colors = AttributeGetter.GetVertexInstanceColors();
	TVertexInstanceAttributesRef<FVector2D> UVs = AttributeGetter.GetVertexInstanceUVs();
	UVs.SetNumIndices(2);

	// Create the Polygon Groups
	FPolygonGroupID PolygonGroupID = MeshDescription.CreatePolygonGroup();
	PolygonGroupNames[PolygonGroupID] = "WorldPainter";

	const int32 VertexCount = Triangulation.Points.Num();
	MeshDescription.ReserveNewVertices(VertexCount);

	const int32 PolygonCount = Triangulation.Triangles.Num();
	MeshDescription.ReserveNewPolygons(PolygonCount);
	MeshDescription.ReserveNewEdges(PolygonCount * 2);


	TArray<int32> IndexBuffer;
	IndexBuffer.Reserve(PolygonCount * 3);
	for (const FTGOR_Triangle& Triangle : Triangulation.Triangles)
	{
		if (Triangle.Enabled)
		{
			IndexBuffer.Append({ Triangle.Verts[0], Triangle.Verts[1], Triangle.Verts[2] });
		}
	}

	const int32 VertexInstanceCount = IndexBuffer.Num();
	MeshDescription.ReserveNewVertexInstances(VertexInstanceCount);

	// Create the vertex
	TMap<int32, FVertexID> VertexIndexToVertexID;
	VertexIndexToVertexID.Reserve(VertexCount);
	for (int32 VertexIndex = 0; VertexIndex < VertexCount; ++VertexIndex)
	{
		const FVector2D& Origin = Triangulation.Points[VertexIndex];
		const FVertexID VertexID = MeshDescription.CreateVertex();
		VertexPositions[VertexID] = FVector(Origin * 2.f - FVector2D(1.f, 1.f), 0.f);
		VertexIndexToVertexID.Add(VertexIndex, VertexID);
	}

	// Create the VertexInstance
	int32 NumTri = VertexInstanceCount / 3;
	TMap<int32, FVertexInstanceID> IndiceIndexToVertexInstanceID;
	IndiceIndexToVertexInstanceID.Reserve(VertexCount);
	for (int32 IndiceIndex = 0; IndiceIndex < VertexInstanceCount; IndiceIndex++)
	{
		const int32 VertexIndex = IndexBuffer[IndiceIndex];
		const FVertexID VertexID = VertexIndexToVertexID[VertexIndex];
		const FVertexInstanceID VertexInstanceID = MeshDescription.CreateVertexInstance(VertexID);
		IndiceIndexToVertexInstanceID.Add(IndiceIndex, VertexInstanceID);

		const FTGOR_BrushData& Vertex = Vertices[VertexIndex];
		Colors[VertexInstanceID] = Vertex.Dark;
		Tangents[VertexInstanceID] = FVector::RightVector;//FVector(Vertex.Light) * 2.f - FVector::OneVector;
		Normals[VertexInstanceID] = FVector(Vertex.Mask).GetSafeNormal();
		BinormalSigns[VertexInstanceID] = 1.f;

		UVs.Set(VertexInstanceID, 1, FVector2D(Vertex.Light.R, Vertex.Light.G));
		UVs.Set(VertexInstanceID, 0, FVector2D(Vertex.Light.B, Vertex.Light.A));
	}

	// Create the polygons for this section
	for (int32 TriIdx = 0; TriIdx < NumTri; TriIdx++)
	{
		TArray<FVertexInstanceID> VertexInstanceIDs;
		VertexInstanceIDs.SetNum(3);

		for (int32 CornerIndex = 0; CornerIndex < 3; ++CornerIndex)
		{
			const int32 IndiceIndex = (TriIdx * 3) + CornerIndex;
			VertexInstanceIDs[CornerIndex] = IndiceIndexToVertexInstanceID[IndiceIndex];
		}

		// Insert a polygon into the mesh
		MeshDescription.CreatePolygon(PolygonGroupID, VertexInstanceIDs);
	}

	// Create static mesh
	if (!IsValid(PainterMesh))
	{
		PainterMesh = NewObject<UStaticMesh>(this, "PainterMesh", EObjectFlags::RF_Public);
	}

	PainterMesh->InitResources();
	PainterMesh->SetNumSourceModels(0);
	PainterMesh->LightingGuid = FGuid::NewGuid();

	// If we got some valid data.
	if (MeshDescription.Polygons().Num() > 0)
	{
		// Add source to new StaticMesh
		FStaticMeshSourceModel& SrcModel = PainterMesh->AddSourceModel();
		SrcModel.BuildSettings.bRecomputeNormals = false;
		SrcModel.BuildSettings.bRecomputeTangents = false;
		SrcModel.BuildSettings.bRemoveDegenerates = false;
		SrcModel.BuildSettings.bUseHighPrecisionTangentBasis = false;
		SrcModel.BuildSettings.bUseFullPrecisionUVs = false;
		SrcModel.BuildSettings.bGenerateLightmapUVs = true;
		SrcModel.BuildSettings.SrcLightmapIndex = 0;
		SrcModel.BuildSettings.DstLightmapIndex = 1;
		PainterMesh->CreateMeshDescription(0, MoveTemp(MeshDescription));
		PainterMesh->CommitMeshDescription(0);
	}

	PainterMesh->StaticMaterials.Reset(1);
	PainterMesh->StaticMaterials.Add(FStaticMaterial(0, "Painter", "Painter"));
	PainterMesh->ImportVersion = EImportStaticMeshVersion::LastVersion;

	// Build mesh from source
	PainterMesh->Build(true);
	PainterMesh->PostEditChange();

	StaticMeshComponent->SetStaticMesh(PainterMesh);
}
*/

#if WITH_EDITOR

void ATGOR_WorldPainterCanvas::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	FProperty* ChangedProperty = PropertyChangedEvent.Property;

	if (ChangedProperty)
	{
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void ATGOR_WorldPainterCanvas::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
}

#endif // WITH_EDITOR