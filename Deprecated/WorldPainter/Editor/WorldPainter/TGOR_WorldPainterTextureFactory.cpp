// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGOR_WorldPainterTextureFactory.h"

#include "AssetTypeCategories.h"
#include "RealitiesProcedural/Textures/TGOR_WorldPainterTexture.h"

#define LOCTEXT_NAMESPACE "TGOR_WorldPainterTextureFactory"

UTGOR_WorldPainterTextureFactory::UTGOR_WorldPainterTextureFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = false;
	bEditorImport = false;
	SupportedClass = UTGOR_WorldPainterTexture::StaticClass();
}

FText UTGOR_WorldPainterTextureFactory::GetDisplayName() const
{
	return LOCTEXT("TGOR_WorldPainterTextureFactoryDescription", "World Painter Texture");
}

bool UTGOR_WorldPainterTextureFactory::ConfigureProperties()
{
	return true;
}

bool UTGOR_WorldPainterTextureFactory::CanCreateNew() const
{
	return true;
}

UObject* UTGOR_WorldPainterTextureFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UTGOR_WorldPainterTexture* WorldPainterTexture = NewObject<UTGOR_WorldPainterTexture>(InParent, Class, Name, Flags);
	if (WorldPainterTexture == nullptr)
	{
		UE_LOG(LogTexture, Warning, TEXT("WorldPainterTexture creation failed.\n"));
	}

	return WorldPainterTexture;
}

#undef LOCTEXT_NAMESPACE
