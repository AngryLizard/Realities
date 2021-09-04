// The Gateway of Realities: Planes of Existence.

#include "TGOR_BakeLibrary.h"

/*
void CreateTexture()
{
	UTexture;
	// Texture Information
	FString FileName = FString("MyTexture");
	int width = 1024;
	int height = 1024;
	uint8* pixels = (uint8*)malloc(height * width * 4); // x4 because it's RGBA. 4 integers, one for Red, one for Green, one for Blue, one for Alpha

	// filling the pixels with dummy data (4 boxes: red, green, blue and white)
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (x < width / 2) {
				if (y < height / 2) {
					pixels[y * 4 * width + x * 4 + 0] = 255; // R
					pixels[y * 4 * width + x * 4 + 1] = 0;   // G
					pixels[y * 4 * width + x * 4 + 2] = 0;   // B
					pixels[y * 4 * width + x * 4 + 3] = 255; // A
				}
				else {
					pixels[y * 4 * width + x * 4 + 0] = 0;   // R
					pixels[y * 4 * width + x * 4 + 1] = 255; // G
					pixels[y * 4 * width + x * 4 + 2] = 0;   // B
					pixels[y * 4 * width + x * 4 + 3] = 255; // A
				}
			}
			else {
				if (y < height / 2) {
					pixels[y * 4 * width + x * 4 + 0] = 0;   // R
					pixels[y * 4 * width + x * 4 + 1] = 0;   // G
					pixels[y * 4 * width + x * 4 + 2] = 255; // B
					pixels[y * 4 * width + x * 4 + 3] = 255; // A
				}
				else {
					pixels[y * 4 * width + x * 4 + 0] = 255; // R
					pixels[y * 4 * width + x * 4 + 1] = 255; // G
					pixels[y * 4 * width + x * 4 + 2] = 255; // B
					pixels[y * 4 * width + x * 4 + 3] = 255; // A
				}
			}
		}
	}


	// Create Package
	FString pathPackage = FString("/Game/MyTextures/");
	FString absolutePathPackage = FPaths::GameContentDir() + "/MyTextures/";

	FPackageName::RegisterMountPoint(*pathPackage, *absolutePathPackage);

	UPackage* Package = CreatePackage(nullptr, *pathPackage);

	// Create the Texture
	FName TextureName = MakeUniqueObjectName(Package, UTexture2D::StaticClass(), FName(*FileName));
	UTexture2D* Texture = NewObject(Package, TextureName, RF_Public | RF_Standalone);

	// Texture Settings
	Texture->PlatformData = new FTexturePlatformData();
	Texture->PlatformData->SizeX = width;
	Texture->PlatformData->SizeY = height;
	Texture->PlatformData->PixelFormat = PF_R8G8B8A8;

	// Passing the pixels information to the texture
	FTexture2DMipMap* Mip = new(Texture->PlatformData->Mips) FTexture2DMipMap();
	Mip->SizeX = width;
	Mip->SizeY = height;
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	uint8* TextureData = (uint8*)Mip->BulkData.Realloc(height * width * sizeof(uint8) * 4);
	FMemory::Memcpy(TextureData, pixels, sizeof(uint8) * height * width * 4);
	Mip->BulkData.Unlock();

	// Updating Texture & mark it as unsaved
	Texture->AddToRoot();
	Texture->UpdateResource();
	Package->MarkPackageDirty();

	UE_LOG(LogTemp, Log, TEXT("Texture created: %s"), &FileName);

	free(pixels);
	pixels = NULL;










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
		UPackage* Package = CreatePackage(NULL, *UserPackageName);
		check(Package);

		// Create StaticMesh object
		UStaticMesh* StaticMesh = NewObject<UStaticMesh>(Package, MeshName, RF_Public | RF_Standalone);
		StaticMesh->InitResources();

		StaticMesh->LightingGuid = FGuid::NewGuid();

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
				UBodySetup* NewBodySetup = StaticMesh->BodySetup;
				NewBodySetup->BodySetupGuid = FGuid::NewGuid();
				NewBodySetup->bGenerateMirroredCollision = false;
				NewBodySetup->bDoubleSidedGeometry = true;
				NewBodySetup->CollisionTraceFlag = CTF_UseComplexAsSimple;
				StaticMesh->LODForCollision = FMath::Min(ProceduralActor->CollisionLOD, Count);
			}
			else
			{
				StaticMesh->CreateBodySetup();
				UBodySetup* NewBodySetup = StaticMesh->BodySetup;
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
			for (auto* Material : UniqueMaterials)
			{
				StaticMesh->StaticMaterials.Add(FStaticMaterial(Material, Material->GetFName(), Material->GetFName()));
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
*/
