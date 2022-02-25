// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGOR_AssetTypeActions_WorldPainterTexture.h"

#include "RealitiesProcedural/Textures/TGOR_WorldPainterTexture.h"

#define LOCTEXT_NAMESPACE "VirtualHeightfieldMeshEditorModule"

UClass* FTGOR_AssetTypeActions_WorldPainterTexture::GetSupportedClass() const
{
	return UTGOR_WorldPainterTexture::StaticClass();
}

FText FTGOR_AssetTypeActions_WorldPainterTexture::GetName() const
{
	return LOCTEXT("AssetTypeActions_WorldPainterTextureBuilder", "WorldPainter Texture"); 
}

FColor FTGOR_AssetTypeActions_WorldPainterTexture::GetTypeColor() const
{
	return FColor(128, 128, 128); 
}

uint32 FTGOR_AssetTypeActions_WorldPainterTexture::GetCategories()
{
	return EAssetTypeCategories::MaterialsAndTextures; 
}

#undef LOCTEXT_NAMESPACE
