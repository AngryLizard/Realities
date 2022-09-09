// Copyright Epic Games, Inc. All Rights Reserved.

#include "RealitiesProceduralDetails.h"
#include "Modules/ModuleManager.h"
#include "Misc/PackageName.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Engine/StaticMesh.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "DetailCategoryBuilder.h"
#include "AssetRegistryModule.h"
#include "StaticMeshAttributes.h"
#include "Engine/StaticMeshActor.h"
#include "PhysicsEngine/BodySetup.h"
#include "Dialogs/DlgPickAssetPath.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"
#include "ProceduralMeshComponent/Public/ProceduralMeshComponent.h"
#include "RealitiesProcedural/Actors/TGOR_ProceduralActor.h"
#include "EditorScriptingUtilities/Public/EditorLevelLibrary.h"
#include "Subsystems/EditorActorSubsystem.h"

#define LOCTEXT_NAMESPACE "RealitiesProceduralDetails"

TSharedRef<IDetailCustomization> FRealitiesProceduralDetails::MakeInstance()
{
	return MakeShareable(new FRealitiesProceduralDetails);
}

void FRealitiesProceduralDetails::CustomizeDetails( IDetailLayoutBuilder& DetailBuilder )
{
	IDetailCategoryBuilder& ProcMeshCategory = DetailBuilder.EditCategory("ProceduralMesh");

	const FText ConvertToStaticMeshText = LOCTEXT("ConvertToStaticMesh", "Create StaticMesh");
	SelectedObjectsList = DetailBuilder.GetSelectedObjects();

	ProcMeshCategory.AddCustomRow(ConvertToStaticMeshText, false)
	.NameContent()
	[
		SNullWidget::NullWidget
	]
	.ValueContent()
	.VAlign(VAlign_Center)
	.MaxDesiredWidth(250)
	[
		SNew(SButton)
		.VAlign(VAlign_Center)
		.ToolTipText(LOCTEXT("ConvertToStaticMeshTooltip", "Create a new StaticMesh asset using current geometry from this ProceduralMeshComponent. Does not modify instance."))
		.OnClicked(this, &FRealitiesProceduralDetails::ClickedOnConvertToStaticMesh)
		.IsEnabled(this, &FRealitiesProceduralDetails::StaticMeshEnabled)
		.Content()
		[
			SNew(STextBlock)
			.Text(ConvertToStaticMeshText)
		]
	];

	const FText ReplaceToStaticMeshText = LOCTEXT("ReplaceToStaticMesh", "Replace StaticMesh");
	SelectedObjectsList = DetailBuilder.GetSelectedObjects();

	ProcMeshCategory.AddCustomRow(ReplaceToStaticMeshText, false)
		.NameContent()
		[
			SNullWidget::NullWidget
		]
	.ValueContent()
		.VAlign(VAlign_Center)
		.MaxDesiredWidth(250)
		[
			SNew(SButton)
			.VAlign(VAlign_Center)
		.ToolTipText(LOCTEXT("ReplaceToStaticMeshTooltip", "Create a new StaticMesh asset using current geometry from this ProceduralMeshComponent and replaces it with a static mesh."))
		.OnClicked(this, &FRealitiesProceduralDetails::ClickedOnReplaceToStaticMesh)
		.IsEnabled(this, &FRealitiesProceduralDetails::StaticMeshEnabled)
		.Content()
		[
			SNew(STextBlock)
			.Text(ReplaceToStaticMeshText)
		]
		];
}

ATGOR_ProceduralActor* FRealitiesProceduralDetails::GetFirstSelectedProceduralActor() const
{
	// Find first selected valid ProceduralActor
	for (const TWeakObjectPtr<UObject>& Object : SelectedObjectsList)
	{
		ATGOR_ProceduralActor* ProceduralActor = Cast<ATGOR_ProceduralActor>(Object.Get());
		// See if this one is good
		if (ProceduralActor != nullptr && !ProceduralActor->IsTemplate())
		{
			return ProceduralActor;
		}
	}
	return nullptr;
}

bool FRealitiesProceduralDetails::StaticMeshEnabled() const
{
	return GetFirstSelectedProceduralActor() != nullptr;
}



// From here mostly copied from ProceduralMesh

TMap<UMaterialInterface*, FPolygonGroupID> BuildMaterialMap(UProceduralMeshComponent* ProcMeshComp, FMeshDescription& MeshDescription)
{
	TMap<UMaterialInterface*, FPolygonGroupID> UniqueMaterials;
	const int32 NumSections = ProcMeshComp->GetNumSections();
	UniqueMaterials.Reserve(NumSections);

	FStaticMeshAttributes AttributeGetter(MeshDescription);
	TPolygonGroupAttributesRef<FName> PolygonGroupNames = AttributeGetter.GetPolygonGroupMaterialSlotNames();
	for (int32 SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
	{
		FProcMeshSection* ProcSection = ProcMeshComp->GetProcMeshSection(SectionIdx);
		UMaterialInterface* Material = ProcMeshComp->GetMaterial(SectionIdx);
		if (IsValid(Material) && !UniqueMaterials.Contains(Material))
		{
			FPolygonGroupID NewPolygonGroup = MeshDescription.CreatePolygonGroup();
			UniqueMaterials.Add(Material, NewPolygonGroup);
			PolygonGroupNames[NewPolygonGroup] = Material->GetFName();
		}
	}
	return UniqueMaterials;
}


FMeshDescription BuildMeshDescription(UProceduralMeshComponent* ProcMeshComp)
{
	FMeshDescription MeshDescription;

	FStaticMeshAttributes AttributeGetter(MeshDescription);
	AttributeGetter.Register();

	TPolygonGroupAttributesRef<FName> PolygonGroupNames = AttributeGetter.GetPolygonGroupMaterialSlotNames();
	TVertexAttributesRef<FVector3f> VertexPositions = AttributeGetter.GetVertexPositions();
	TVertexInstanceAttributesRef<FVector3f> Tangents = AttributeGetter.GetVertexInstanceTangents();
	TVertexInstanceAttributesRef<float> BinormalSigns = AttributeGetter.GetVertexInstanceBinormalSigns();
	TVertexInstanceAttributesRef<FVector3f> Normals = AttributeGetter.GetVertexInstanceNormals();
	TVertexInstanceAttributesRef<FVector4f> Colors = AttributeGetter.GetVertexInstanceColors();
	TVertexInstanceAttributesRef<FVector2f> UVs = AttributeGetter.GetVertexInstanceUVs();

	// Materials to apply to new mesh
	const int32 NumSections = ProcMeshComp->GetNumSections();
	int32 VertexCount = 0;
	int32 VertexInstanceCount = 0;
	int32 PolygonCount = 0;

	TMap<UMaterialInterface*, FPolygonGroupID> UniqueMaterials = BuildMaterialMap(ProcMeshComp, MeshDescription);
	TArray<FPolygonGroupID> PolygonGroupForSection;
	PolygonGroupForSection.Reserve(NumSections);

	// Calculate the totals for each ProcMesh element type
	for (int32 SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
	{
		FProcMeshSection* ProcSection =
			ProcMeshComp->GetProcMeshSection(SectionIdx);
		VertexCount += ProcSection->ProcVertexBuffer.Num();
		VertexInstanceCount += ProcSection->ProcIndexBuffer.Num();
		PolygonCount += ProcSection->ProcIndexBuffer.Num() / 3;
	}
	MeshDescription.ReserveNewVertices(VertexCount);
	MeshDescription.ReserveNewVertexInstances(VertexInstanceCount);
	MeshDescription.ReserveNewPolygons(PolygonCount);
	MeshDescription.ReserveNewEdges(PolygonCount * 2);
	UVs.SetNumChannels(4);

	// Create the Polygon Groups
	for (int32 SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
	{
		FProcMeshSection* ProcSection =
			ProcMeshComp->GetProcMeshSection(SectionIdx);
		UMaterialInterface* Material = ProcMeshComp->GetMaterial(SectionIdx);
		if (IsValid(Material))
		{
			FPolygonGroupID* PolygonGroupID = UniqueMaterials.Find(Material);
			check(PolygonGroupID != nullptr);
			PolygonGroupForSection.Add(*PolygonGroupID);
		}
	}


	// Add Vertex and VertexInstance and polygon for each section
	for (int32 SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
	{
		FProcMeshSection* ProcSection =
			ProcMeshComp->GetProcMeshSection(SectionIdx);
		FPolygonGroupID PolygonGroupID = PolygonGroupForSection[SectionIdx];
		// Create the vertex
		int32 NumVertex = ProcSection->ProcVertexBuffer.Num();
		TMap<int32, FVertexID> VertexIndexToVertexID;
		VertexIndexToVertexID.Reserve(NumVertex);
		for (int32 VertexIndex = 0; VertexIndex < NumVertex; ++VertexIndex)
		{
			FProcMeshVertex& Vert = ProcSection->ProcVertexBuffer[VertexIndex];
			const FVertexID VertexID = MeshDescription.CreateVertex();
			VertexPositions[VertexID] = FVector3f(Vert.Position);
			VertexIndexToVertexID.Add(VertexIndex, VertexID);
		}
		// Create the VertexInstance
		int32 NumIndices = ProcSection->ProcIndexBuffer.Num();
		int32 NumTri = NumIndices / 3;
		TMap<int32, FVertexInstanceID> IndiceIndexToVertexInstanceID;
		IndiceIndexToVertexInstanceID.Reserve(NumVertex);
		for (int32 IndiceIndex = 0; IndiceIndex < NumIndices; IndiceIndex++)
		{
			const int32 VertexIndex = ProcSection->ProcIndexBuffer[IndiceIndex];
			const FVertexID VertexID = VertexIndexToVertexID[VertexIndex];
			const FVertexInstanceID VertexInstanceID =
				MeshDescription.CreateVertexInstance(VertexID);
			IndiceIndexToVertexInstanceID.Add(IndiceIndex, VertexInstanceID);

			FProcMeshVertex& ProcVertex = ProcSection->ProcVertexBuffer[VertexIndex];

			Tangents[VertexInstanceID] = FVector3f(ProcVertex.Tangent.TangentX);
			Normals[VertexInstanceID] = FVector3f(ProcVertex.Normal);
			BinormalSigns[VertexInstanceID] =
				ProcVertex.Tangent.bFlipTangentY ? -1.f : 1.f;

			Colors[VertexInstanceID] = FLinearColor(ProcVertex.Color);

			UVs.Set(VertexInstanceID, 0, FVector2f(ProcVertex.UV0));
			UVs.Set(VertexInstanceID, 1, FVector2f(ProcVertex.UV1));
			UVs.Set(VertexInstanceID, 2, FVector2f(ProcVertex.UV2));
			UVs.Set(VertexInstanceID, 3, FVector2f(ProcVertex.UV3));
		}

		// Create the polygons for this section
		for (int32 TriIdx = 0; TriIdx < NumTri; TriIdx++)
		{
			FVertexID VertexIndexes[3];
			TArray<FVertexInstanceID> VertexInstanceIDs;
			VertexInstanceIDs.SetNum(3);

			for (int32 CornerIndex = 0; CornerIndex < 3; ++CornerIndex)
			{
				const int32 IndiceIndex = (TriIdx * 3) + CornerIndex;
				const int32 VertexIndex = ProcSection->ProcIndexBuffer[IndiceIndex];
				VertexIndexes[CornerIndex] = VertexIndexToVertexID[VertexIndex];
				VertexInstanceIDs[CornerIndex] =
					IndiceIndexToVertexInstanceID[IndiceIndex];
			}

			// Insert a polygon into the mesh
			MeshDescription.CreatePolygon(PolygonGroupID, VertexInstanceIDs);
		}
	}
	return MeshDescription;
}

UStaticMesh* FRealitiesProceduralDetails::CreateStaticMesh(ATGOR_ProceduralActor* ProceduralActor)
{
	VPORT(ProceduralActor);
	UProceduralMeshComponent* ProceduralMesh = ProceduralActor->GetMesh();
	if (ProceduralMesh != nullptr)
	{
		// Make sure mesh can be generated
		if (ProceduralActor->Generate(ProceduralActor->PreviewLOD))
		{
			FString NewNameSuggestion = FString(TEXT("ProcMesh"));
			FString PackageName = FString(TEXT("/Game/Meshes/")) + NewNameSuggestion;
			FString Name;
			FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
			AssetToolsModule.Get().CreateUniqueAssetName(PackageName, TEXT(""), PackageName, Name);

			TSharedPtr<SDlgPickAssetPath> PickAssetPathWidget =
				SNew(SDlgPickAssetPath)
				.Title(LOCTEXT("ConvertToStaticMeshPickName", "Choose New StaticMesh Location"))
				.DefaultAssetPath(FText::FromString(PackageName));

			if (PickAssetPathWidget->ShowModal() == EAppReturnType::Ok)
			{
				// Get the full name of where we want to create the physics asset.
				FString UserPackageName = PickAssetPathWidget->GetFullAssetPath().ToString();
				FName MeshName(*FPackageName::GetLongPackageAssetName(UserPackageName));

				// Check if the user inputed a valid asset name, if they did not, give it the generated default name
				if (MeshName == NAME_None)
				{
					// Use the defaults that were already generated.
					UserPackageName = PackageName;
					MeshName = *Name;
				}

				// Then find/create it.
				UPackage* Package = CreatePackage(*UserPackageName);
				check(Package);

				// Create StaticMesh object
				UStaticMesh* StaticMesh = NewObject<UStaticMesh>(Package, MeshName, RF_Public | RF_Standalone);
				StaticMesh->InitResources();

				StaticMesh->SetLightingGuid(FGuid::NewGuid());

				int32 Count = 0;
				for (int32 LOD = 0; LOD <= ProceduralActor->MaxLOD; LOD++)
				{
					if (!ProceduralActor->Generate(LOD))
					{
						return nullptr;
					}
					FMeshDescription MeshDescription = BuildMeshDescription(ProceduralMesh);

					// If we got some valid data.
					if (MeshDescription.Polygons().Num() > 0)
					{
						// Add source to new StaticMesh
						FStaticMeshSourceModel& SrcModel = StaticMesh->AddSourceModel();
						SrcModel.BuildSettings.bRecomputeNormals = false;
						SrcModel.BuildSettings.bRecomputeTangents = false;
						SrcModel.BuildSettings.bRemoveDegenerates = false;
						SrcModel.BuildSettings.bUseHighPrecisionTangentBasis = false;
						SrcModel.BuildSettings.bUseFullPrecisionUVs = false;
						SrcModel.BuildSettings.bGenerateLightmapUVs = true;
						SrcModel.BuildSettings.SrcLightmapIndex = 0;
						SrcModel.BuildSettings.DstLightmapIndex = 1;
						StaticMesh->CreateMeshDescription(LOD, MoveTemp(MeshDescription));
						StaticMesh->CommitMeshDescription(LOD);
						Count++;
					}
				}

				if (Count > 0)
				{
					//// SIMPLE COLLISION
					if (ProceduralMesh->bUseComplexAsSimpleCollision)
					{
						StaticMesh->CreateBodySetup();
						UBodySetup* NewBodySetup = StaticMesh->GetBodySetup();
						NewBodySetup->BodySetupGuid = FGuid::NewGuid();
						NewBodySetup->bGenerateMirroredCollision = false;
						NewBodySetup->bDoubleSidedGeometry = true;
						NewBodySetup->CollisionTraceFlag = CTF_UseComplexAsSimple;
						StaticMesh->LODForCollision = FMath::Min(ProceduralActor->CollisionLOD, Count);
					}
					else
					{
						StaticMesh->CreateBodySetup();
						UBodySetup* NewBodySetup = StaticMesh->GetBodySetup();
						NewBodySetup->BodySetupGuid = FGuid::NewGuid();
						NewBodySetup->AggGeom.ConvexElems = ProceduralMesh->ProcMeshBodySetup->AggGeom.ConvexElems;
						NewBodySetup->bGenerateMirroredCollision = false;
						NewBodySetup->bDoubleSidedGeometry = true;
						NewBodySetup->CollisionTraceFlag = CTF_UseDefault;
						NewBodySetup->CreatePhysicsMeshes();
					}

					//// MATERIALS
					TSet<UMaterialInterface*> UniqueMaterials;
					const int32 NumSections = ProceduralMesh->GetNumSections();
					for (int32 SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
					{
						FProcMeshSection* ProcSection = ProceduralMesh->GetProcMeshSection(SectionIdx);
						UMaterialInterface* Material = ProceduralMesh->GetMaterial(SectionIdx);
						UniqueMaterials.Add(Material);
					}
					// Copy materials to new mesh
					TArray<FStaticMaterial>& StaticMaterials = StaticMesh->GetStaticMaterials();
					for (auto* Material : UniqueMaterials)
					{
						StaticMaterials.Add(FStaticMaterial(Material, Material->GetFName(), Material->GetFName()));
					}

					//Set the Imported version before calling the build
					StaticMesh->ImportVersion = EImportStaticMeshVersion::LastVersion;

					// Build mesh from source
					StaticMesh->Build(false);
					StaticMesh->PostEditChange();

					ProceduralActor->Generate(ProceduralActor->PreviewLOD);

					// Notify asset registry of new asset
					FAssetRegistryModule::AssetCreated(StaticMesh);
					return StaticMesh;
				}
			}
		}
		else
		{
			ERRET("Cannot generate mesh", Error, nullptr);
		}
	}
	return nullptr;
}

FReply FRealitiesProceduralDetails::ClickedOnConvertToStaticMesh()
{
	ATGOR_ProceduralActor* ProceduralActor = GetFirstSelectedProceduralActor();
	if (ProceduralActor != nullptr)
	{
		CreateStaticMesh(ProceduralActor);
	}
	return FReply::Handled();
}

FReply FRealitiesProceduralDetails::ClickedOnReplaceToStaticMesh()
{
	ATGOR_ProceduralActor* ProceduralActor = GetFirstSelectedProceduralActor();
	if (ProceduralActor != nullptr)
	{
		UProceduralMeshComponent* Procedural = ProceduralActor->GetMesh();
		UStaticMesh* StaticMesh = CreateStaticMesh(ProceduralActor);
		if (Procedural != nullptr && StaticMesh != nullptr)
		{
			const FTransform ActorTransform = ProceduralActor->GetTransform();
			const FTransform ComponentTransform = Procedural->GetComponentTransform();

			UEditorActorSubsystem* ActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
			if (ActorSubsystem)
			{
				AActor* MeshActor = ActorSubsystem->SpawnActorFromClass(AStaticMeshActor::StaticClass(), ActorTransform.GetLocation(), ActorTransform.GetRotation().Rotator());
				if (MeshActor != nullptr)
				{
					MeshActor->SetActorTransform(ActorTransform);
					UStaticMeshComponent* MeshComponent = MeshActor->FindComponentByClass<UStaticMeshComponent>();
					if (MeshComponent != nullptr)
					{
						MeshComponent->SetStaticMesh(StaticMesh);
						MeshComponent->SetWorldTransform(ComponentTransform);
					}
					TArray<AActor*> Selected;
					Selected.Emplace(MeshActor);
					ActorSubsystem->SetSelectedLevelActors(Selected);
					ActorSubsystem->DestroyActor(ProceduralActor);
				}
			}
		}
	}
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
