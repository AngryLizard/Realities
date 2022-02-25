// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGOR_MaterialExpressionWorldPainterTexture.h"

#include "EditorSupportDelegates.h"
#include "Engine/Texture2D.h"
#include "MaterialCompiler.h"
#include "Materials/MaterialExpressionTextureBase.h"
#include "RealitiesProcedural/Textures/TGOR_WorldPainterTexture.h"

#define LOCTEXT_NAMESPACE "TGOR_MaterialExpressionWorldPainterTexture"

UTGOR_MaterialExpressionWorldPainterTexture::UTGOR_MaterialExpressionWorldPainterTexture(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	WorldPainterTexture(nullptr)
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		FText NAME_WorldPainterTexture;
		FConstructorStatics()
			: NAME_WorldPainterTexture(LOCTEXT("WorldPainterSampler", "WorldPainterSampler"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

#if WITH_EDITORONLY_DATA
	MenuCategories.Add(ConstructorStatics.NAME_WorldPainterTexture);
#endif

#if WITH_EDITORONLY_DATA
	Outputs.Reset();
	Outputs.Add(FExpressionOutput(TEXT("RGB"), 1, 1, 1, 1, 0));
	Outputs.Add(FExpressionOutput(TEXT("A"), 1, 0, 0, 0, 1));
#endif
}

#if WITH_EDITOR

void UTGOR_MaterialExpressionWorldPainterTexture::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetName() == TEXT("WorldPainterTexture"))
	{
		if (WorldPainterTexture != nullptr && WorldPainterTexture->Texture != nullptr)
		{
			SamplerType = UMaterialExpressionTextureBase::GetSamplerTypeForTexture(WorldPainterTexture->Texture);

			FEditorSupportDelegates::ForcePropertyWindowRebuild.Broadcast(this);
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

int32 UTGOR_MaterialExpressionWorldPainterTexture::Compile(class FMaterialCompiler* Compiler, int32 OutputIndex)
{
	if (WorldPainterTexture != nullptr && WorldPainterTexture->Texture != nullptr)
	{
		const int32 LocationIndex = Location.IsConnected() ? Location.Compile(Compiler) : Compiler->Constant2(0,0);
		const int32 SizeIndex = Size.IsConnected() ? Size.Compile(Compiler) : Compiler->Constant2(1, 1);

		const int32 WorldPositionIndex = Compiler->ComponentMask(Compiler->WorldPosition(EWorldPositionIncludedOffsets::WPT_Default), true, true, false, false);
		int32 RelativePositionIndex = Compiler->Sub(WorldPositionIndex, LocationIndex);
		
		if (Angle.IsConnected())
		{
			const int32 AngleIndex = Angle.Compile(Compiler);
			const int32 CosineIndex = Compiler->Cosine(AngleIndex);
			const int32 SineIndex = Compiler->Sine(AngleIndex);
			const int32 LocalXIndex = Compiler->Dot(Compiler->AppendVector(CosineIndex, SineIndex), RelativePositionIndex);
			const int32 LocalYIndex = Compiler->Dot(Compiler->AppendVector(Compiler->Mul(SineIndex, Compiler->Constant(-1.f)), CosineIndex), RelativePositionIndex);
			RelativePositionIndex = Compiler->AppendVector(LocalXIndex, LocalYIndex);
		}

		const int32 UVIndex = Compiler->Div(RelativePositionIndex, SizeIndex);

		int32 WIndex = High.IsConnected() ? High.Compile(Compiler) : Compiler->Constant(1);
		if (Low.IsConnected())
		{
			const int32 LowIndex = Low.Compile(Compiler);
			if (Shading.IsConnected())
			{
				const int32 ShadingIndex = Shading.Compile(Compiler);
				WIndex = Compiler->Add(LowIndex, Compiler->Mul(Compiler->Sub(WIndex, LowIndex), ShadingIndex));
			}
			else
			{
				const int32 XIndex = Compiler->ComponentMask(RelativePositionIndex, true, false, false, false);
				WIndex = Compiler->Add(LowIndex, Compiler->Mul(Compiler->Sub(WIndex, LowIndex), XIndex));
			}
		}
		
		const int32 CoordinateIndex = Compiler->AppendVector(UVIndex, WIndex);

		int32 TextureReferenceIndex = INDEX_NONE;
		int32 TextureCodeIndex = Compiler->Texture(WorldPainterTexture->Texture, TextureReferenceIndex, SamplerType, SSM_Wrap_WorldGroupSettings, TMVM_None);
		return Compiler->TextureSample(TextureCodeIndex, CoordinateIndex, SamplerType);
	}

	return CompilerError(Compiler, TEXT("Requires valid texture"));
}

void UTGOR_MaterialExpressionWorldPainterTexture::GetCaption(TArray<FString>& OutCaptions) const
{
	OutCaptions.Add(TEXT("WorldPainterSample"));
}

#endif // WITH_EDITOR

UObject* UTGOR_MaterialExpressionWorldPainterTexture::GetReferencedTexture() const
{
	return WorldPainterTexture != nullptr ? WorldPainterTexture->Texture : nullptr;
}

#undef LOCTEXT_NAMESPACE
