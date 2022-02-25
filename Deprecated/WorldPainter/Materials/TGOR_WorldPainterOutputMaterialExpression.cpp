// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGOR_WorldPainterOutputMaterialExpression.h"

#include "EditorSupportDelegates.h"
#include "MaterialCompiler.h"
#include "Materials/MaterialExpressionTextureBase.h"

#define LOCTEXT_NAMESPACE "TGOR_WorldPainterMaterialExpression"
UTGOR_WorldPainterOutputMaterialExpression::UTGOR_WorldPainterOutputMaterialExpression(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		FText NAME_VirtualTexture;
		FConstructorStatics()
			: NAME_VirtualTexture(LOCTEXT("VirtualTexture", "VirtualTexture"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

#if WITH_EDITORONLY_DATA
	MenuCategories.Add(ConstructorStatics.NAME_VirtualTexture);
#endif

#if WITH_EDITOR
	Outputs.Reset();
#endif
}

#if WITH_EDITOR

int32 UTGOR_WorldPainterOutputMaterialExpression::Compile(class FMaterialCompiler* Compiler, int32 OutputIndex)
{
	int32 CodeInput = INDEX_NONE;
	uint8 OutputAttributeMask = 0;

	// Order of outputs generates function names GetVirtualTextureOutput{index}
	// These must match the function names called in VirtualTextureMaterial.usf
	if (OutputIndex == 0)
	{
		CodeInput = BaseColor.IsConnected() ? BaseColor.Compile(Compiler) : Compiler->Constant3(0.f, 0.f, 0.f);
		OutputAttributeMask |= BaseColor.IsConnected() ? (1 << (uint8)ERuntimeVirtualTextureAttributeType::BaseColor) : 0;
	}
	else if (OutputIndex == 1)
	{
		CodeInput = Specular.IsConnected() ? Specular.Compile(Compiler) : Compiler->Constant(0.5f);
		OutputAttributeMask |= Specular.IsConnected() ? (1 << (uint8)ERuntimeVirtualTextureAttributeType::Specular) : 0;
	}
	else if (OutputIndex == 2)
	{
		CodeInput = Roughness.IsConnected() ? Roughness.Compile(Compiler) : Compiler->Constant(0.5f);
		OutputAttributeMask |= Roughness.IsConnected() ? (1 << (uint8)ERuntimeVirtualTextureAttributeType::Roughness) : 0;
	}
	else if (OutputIndex == 3)
	{
		CodeInput = Normal.IsConnected() ? Normal.Compile(Compiler) : Compiler->Constant3(0.f, 0.f, 1.f);
		OutputAttributeMask |= Normal.IsConnected() ? (1 << (uint8)ERuntimeVirtualTextureAttributeType::Normal) : 0;
	}
	else if (OutputIndex == 4)
	{
		CodeInput = WorldHeight.IsConnected() ? WorldHeight.Compile(Compiler) : Compiler->Constant(0.f);
		OutputAttributeMask |= WorldHeight.IsConnected() ? (1 << (uint8)ERuntimeVirtualTextureAttributeType::WorldHeight) : 0;
	}
	else if (OutputIndex == 5)
	{
		CodeInput = Opacity.IsConnected() ? Opacity.Compile(Compiler) : Compiler->Constant(1.f);
	}
	else if (OutputIndex == 6)
	{
		CodeInput = Mask.IsConnected() ? Mask.Compile(Compiler) : Compiler->Constant(1.f);
		OutputAttributeMask |= Mask.IsConnected() ? (1 << (uint8)ERuntimeVirtualTextureAttributeType::Mask) : 0;
	}

	Compiler->VirtualTextureOutput(OutputAttributeMask);
	return Compiler->CustomOutput(this, OutputIndex, CodeInput);
}

void UTGOR_WorldPainterOutputMaterialExpression::GetCaption(TArray<FString>& OutCaptions) const
{
	OutCaptions.Add(FString(TEXT("Runtime Virtual Texture Output")));
}

#endif // WITH_EDITOR

int32 UTGOR_WorldPainterOutputMaterialExpression::GetNumOutputs() const
{
	return 7;
}

FString UTGOR_WorldPainterOutputMaterialExpression::GetFunctionName() const
{
	return TEXT("GetVirtualTextureOutput");
}

FString UTGOR_WorldPainterOutputMaterialExpression::GetDisplayName() const
{
	return TEXT("Runtime Virtual Texture");
}


/*

UTGOR_WorldPainterOutputMaterialExpression::UTGOR_WorldPainterOutputMaterialExpression(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		FText NAME_WorldPainter;
		FConstructorStatics()
			//: NAME_WorldPainter(LOCTEXT("WorldPainter", "WorldPainter"))
			: NAME_WorldPainter(LOCTEXT("VirtualTexture", "VirtualTexture"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

#if WITH_EDITORONLY_DATA
	MenuCategories.Add(ConstructorStatics.NAME_WorldPainter);
#endif

#if WITH_EDITOR
	Outputs.Reset();
#endif
}

#if WITH_EDITOR

int32 UTGOR_WorldPainterOutputMaterialExpression::Compile(class FMaterialCompiler* Compiler, int32 OutputIndex)
{
	int32 CodeInput = INDEX_NONE;
	uint8 OutputAttributeMask = 0;

	// Order of outputs generates function names GetVirtualTextureOutput{index}
	// These must match the function names called in VirtualTextureMaterial.usf
	if (OutputIndex == 0)
	{
		const int32 PrimaryInput = Primary.Compile(Compiler);

		const int32 SpecularInput = Primary.IsConnected() ? Compiler->ComponentMask(PrimaryInput, true, false, false, false) : Compiler->Constant(0.f);
		Compiler->VirtualTextureOutput(Primary.IsConnected() ? (1 << (uint8)ERuntimeVirtualTextureAttributeType::Specular) : 0);
		Compiler->CustomOutput(this, 1, SpecularInput);

		const int32 RoughnessInput = Primary.IsConnected() ? Compiler->ComponentMask(PrimaryInput, true, false, false, false) : Compiler->Constant(0.f);
		Compiler->VirtualTextureOutput(Primary.IsConnected() ? (1 << (uint8)ERuntimeVirtualTextureAttributeType::Roughness) : 0);
		Compiler->CustomOutput(this, 2, RoughnessInput);

		const int32 MaskInput = Primary.IsConnected() ? Compiler->ComponentMask(PrimaryInput, true, false, false, false) : Compiler->Constant(0.f);
		Compiler->VirtualTextureOutput(Primary.IsConnected() ? (1 << (uint8)ERuntimeVirtualTextureAttributeType::Mask) : 0);
		Compiler->CustomOutput(this, 6, MaskInput);
	}
	else if (OutputIndex == 1)
	{
		const int32 ColorInput = Secondary.IsConnected() ? Secondary.Compile(Compiler) : Compiler->Constant3(0.f, 0.f, 0.f);
		Compiler->VirtualTextureOutput(Secondary.IsConnected() ? (1 << (uint8)ERuntimeVirtualTextureAttributeType::BaseColor) : 0);
		Compiler->CustomOutput(this, 0, ColorInput);
	}
	else if (OutputIndex == 2)
	{
		const int32 NormalInput = Mask.IsConnected() ? Mask.Compile(Compiler) : Compiler->Constant3(0.f, 0.f, 1.f);
		Compiler->VirtualTextureOutput(Mask.IsConnected() ? (1 << (uint8)ERuntimeVirtualTextureAttributeType::Normal) : 0);
		Compiler->CustomOutput(this, 3, NormalInput);
	}

	return INDEX_NONE;
}

void UTGOR_WorldPainterOutputMaterialExpression::GetCaption(TArray<FString>& OutCaptions) const
{
	OutCaptions.Add(FString(TEXT("Runtime Virtual Texture Output")));
}

#endif // WITH_EDITOR

int32 UTGOR_WorldPainterOutputMaterialExpression::GetNumOutputs() const
{
	return 7;
}

FString UTGOR_WorldPainterOutputMaterialExpression::GetFunctionName() const
{
	return TEXT("GetVirtualTextureOutput");
}

FString UTGOR_WorldPainterOutputMaterialExpression::GetDisplayName() const
{
	return TEXT("Runtime Virtual Texture");
	//return TEXT("World Painter");
}
*/
#undef LOCTEXT_NAMESPACE
