// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGOR_RealitiesUGCEditorStyle.h"
#include "RealitiesUGCEditor/RealitiesUGCEditor.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr< FSlateStyleSet > FTGOR_RealitiesUGCEditorStyle::StyleInstance = NULL;

void FTGOR_RealitiesUGCEditorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FTGOR_RealitiesUGCEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FTGOR_RealitiesUGCEditorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("SimpleUGCEditorStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef< FSlateStyleSet > FTGOR_RealitiesUGCEditorStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("SimpleUGCEditorStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("RealitiesUGC")->GetBaseDir() / TEXT("Resources"));

	Style->Set("SimpleUGCEditor.PackageUGCAction", new IMAGE_BRUSH(TEXT("PackageUGC_64x"), Icon40x40));
	Style->Set("SimpleUGCEditor.CreateUGCAction", new IMAGE_BRUSH(TEXT("CreateUGC_64x"), Icon40x40));

	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FTGOR_RealitiesUGCEditorStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FTGOR_RealitiesUGCEditorStyle::Get()
{
	return *StyleInstance;
}
