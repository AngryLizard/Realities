// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGOR_WorldPainterSamplerMaterialExpression.h"

#include "EditorSupportDelegates.h"
#include "MaterialCompiler.h"
#include "Materials/MaterialExpressionTextureBase.h"

#define LOCTEXT_NAMESPACE "TGOR_WorldPainterSamplerMaterialExpression"

UTGOR_WorldPainterSamplerMaterialExpression::UTGOR_WorldPainterSamplerMaterialExpression(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		FText NAME_VirtualTexture;
		FText NAME_Parameters;
		FConstructorStatics()
			: NAME_VirtualTexture(LOCTEXT("WorldPainter", "WorldPainter"))
			, NAME_Parameters(LOCTEXT("Parameters", "Parameters"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	bIsParameterExpression = true;

#if WITH_EDITORONLY_DATA
	MenuCategories.Add(ConstructorStatics.NAME_VirtualTexture);
	MenuCategories.Add(ConstructorStatics.NAME_Parameters);
#endif

#if WITH_EDITOR
	InitOutputs();
	bShowOutputNameOnPin = true;
	bShowMaskColorsOnPin = false;
#endif
}

bool UTGOR_WorldPainterSamplerMaterialExpression::InitVirtualTextureDependentSettings()
{
	bool bChanged = false;
	if (VirtualTexture != nullptr)
	{
		bChanged |= (ERuntimeVirtualTextureMaterialType::BaseColor_Normal_Specular_Mask_YCoCg != VirtualTexture->GetMaterialType());
		bChanged |= bSinglePhysicalSpace != VirtualTexture->GetSinglePhysicalSpace();
		bSinglePhysicalSpace = VirtualTexture->GetSinglePhysicalSpace();
		bChanged |= bAdaptive != VirtualTexture->GetAdaptivePageTable();
		bAdaptive = VirtualTexture->GetAdaptivePageTable();
	}
	return bChanged;
}

void UTGOR_WorldPainterSamplerMaterialExpression::InitOutputs()
{
#if WITH_EDITORONLY_DATA
	Outputs.Reset();

	Outputs.Add(FExpressionOutput(TEXT("Dark"), 1, 1, 1, 1, 0));
	Outputs.Add(FExpressionOutput(TEXT("Light"), 1, 1, 1, 1, 0));
	Outputs.Add(FExpressionOutput(TEXT("Normal"), 1, 1, 1, 1, 0));
#endif // WITH_EDITORONLY_DATA
}

UObject* UTGOR_WorldPainterSamplerMaterialExpression::GetReferencedTexture() const
{
	return VirtualTexture;
}

bool UTGOR_WorldPainterSamplerMaterialExpression::IsNamedParameter(const FHashedMaterialParameterInfo& ParameterInfo, URuntimeVirtualTexture*& OutValue) const
{
	if (ParameterInfo.Name == ParameterName)
	{
		OutValue = VirtualTexture;
		return true;
	}

	return false;
}

void UTGOR_WorldPainterSamplerMaterialExpression::GetAllParameterInfo(TArray<FMaterialParameterInfo>& OutParameterInfo, TArray<FGuid>& OutParameterIds, const FMaterialParameterInfo& InBaseParameterInfo) const
{
	int32 CurrentSize = OutParameterInfo.Num();
	FMaterialParameterInfo NewParameter(ParameterName, InBaseParameterInfo.Association, InBaseParameterInfo.Index);
#if WITH_EDITOR
	NewParameter.ParameterLocation = Material;
	if (Function != nullptr)
	{
		NewParameter.ParameterLocation = Function;
	}
	if (HasConnectedOutputs())
#endif
	{
		OutParameterInfo.AddUnique(NewParameter);
		if (CurrentSize != OutParameterInfo.Num())
		{
			OutParameterIds.Add(ExpressionGUID);
		}
	}
}

#if WITH_EDITOR

bool UTGOR_WorldPainterSamplerMaterialExpression::SetParameterValue(FName InParameterName, URuntimeVirtualTexture* InValue)
{
	if (InParameterName == ParameterName)
	{
		VirtualTexture = InValue;
		return true;
	}

	return false;
}

void UTGOR_WorldPainterSamplerMaterialExpression::SetEditableName(const FString& NewName)
{
	ParameterName = *NewName;
}

FString UTGOR_WorldPainterSamplerMaterialExpression::GetEditableName() const
{
	return ParameterName.ToString();
}

void UTGOR_WorldPainterSamplerMaterialExpression::ValidateParameterName(const bool bAllowDuplicateName)
{
	if (Material != nullptr)
	{
		int32 NameIndex = 1;
		bool bFoundValidName = false;
		FName PotentialName;

		// Find an available unique name
		while (!bFoundValidName)
		{
			PotentialName = GetParameterName();

			// Parameters cannot be named Name_None, use the default name instead
			if (PotentialName == NAME_None)
			{
				PotentialName = TEXT("Param");
			}

			if (!bAllowDuplicateName)
			{
				if (NameIndex != 1)
				{
					PotentialName.SetNumber(NameIndex);
				}

				bFoundValidName = true;

				for (const UMaterialExpression* Expression : Material->Expressions)
				{
					if (Expression != nullptr && Expression->HasAParameterName())
					{
						// Validate that the new name doesn't violate the expression's rules (by default, same name as another of the same class)
						if (Expression != this && Expression->GetParameterName() == PotentialName && Expression->HasClassAndNameCollision(this))
						{
							bFoundValidName = false;
							break;
						}
					}
				}

				++NameIndex;
			}
			else
			{
				bFoundValidName = true;
			}
		}

		if (bAllowDuplicateName)
		{
			// Check for any matching values
			for (UMaterialExpression* Expression : Material->Expressions)
			{
				if (Expression != nullptr && Expression->HasAParameterName())
				{
					// Name are unique per class type
					if (Expression != this && Expression->GetParameterName() == PotentialName && Expression->GetClass() == GetClass())
					{
						SetValueToMatchingExpression(Expression);
						break;
					}
				}
			}
		}

		SetParameterName(PotentialName);
	}

}

bool UTGOR_WorldPainterSamplerMaterialExpression::MatchesSearchQuery(const TCHAR* SearchQuery)
{
	if (ParameterName.ToString().Contains(SearchQuery))
	{
		return true;
	}

	return UMaterialExpression::MatchesSearchQuery(SearchQuery);
}

void UTGOR_WorldPainterSamplerMaterialExpression::SetValueToMatchingExpression(UMaterialExpression* OtherExpression)
{
	URuntimeVirtualTexture* Value = nullptr;
	if (Material->GetRuntimeVirtualTextureParameterValue(FMaterialParameterInfo(OtherExpression->GetParameterName()), Value))
	{
		VirtualTexture = Value;
		FProperty* ParamProperty = FindFProperty<FProperty>(UTGOR_WorldPainterSamplerMaterialExpression::StaticClass(), GET_MEMBER_NAME_STRING_CHECKED(UTGOR_WorldPainterSamplerMaterialExpression, VirtualTexture));
		FPropertyChangedEvent PropertyChangedEvent(ParamProperty);
		PostEditChangeProperty(PropertyChangedEvent);
	}
}

void UTGOR_WorldPainterSamplerMaterialExpression::PostLoad()
{
	Super::PostLoad();

	InitOutputs();
}

void UTGOR_WorldPainterSamplerMaterialExpression::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// Update MaterialType setting to match VirtualTexture
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetName() == TEXT("VirtualTexture"))
	{
		if (VirtualTexture != nullptr)
		{
			InitVirtualTextureDependentSettings();
			FEditorSupportDelegates::ForcePropertyWindowRebuild.Broadcast(this);
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

int32 UTGOR_WorldPainterSamplerMaterialExpression::Compile(class FMaterialCompiler* Compiler, int32 OutputIndex)
{
	// Is this a valid UTGOR_WorldPainterSamplerMaterialExpression?
	const bool bIsParameter = HasAParameterName() && GetParameterName().IsValid() && !GetParameterName().IsNone();

	// Check validity of current virtual texture
	bool bIsVirtualTextureValid = VirtualTexture != nullptr;
	if (!bIsVirtualTextureValid)
	{
		if (!bIsParameter)
		{
			Compiler->Error(TEXT("Missing input Virtual Texture"));
		}
	}
	else if (VirtualTexture->GetMaterialType() != ERuntimeVirtualTextureMaterialType::BaseColor_Normal_Specular_Mask_YCoCg)
	{
		UEnum const* Enum = StaticEnum<ERuntimeVirtualTextureMaterialType>();
		FString MaterialTypeDisplayName = Enum->GetDisplayNameTextByValue((int64)ERuntimeVirtualTextureMaterialType::BaseColor_Normal_Specular_Mask_YCoCg).ToString();
		FString TextureTypeDisplayName = Enum->GetDisplayNameTextByValue((int64)VirtualTexture->GetMaterialType()).ToString();

		Compiler->Errorf(TEXT("%Material type is '%s', should be '%s' to match %s"),
			*MaterialTypeDisplayName,
			*TextureTypeDisplayName,
			*VirtualTexture->GetName());

		bIsVirtualTextureValid = false;
	}
	else if (VirtualTexture->GetSinglePhysicalSpace() != bSinglePhysicalSpace)
	{
		Compiler->Errorf(TEXT("%Page table packing is '%d', should be '%d' to match %s"),
			bSinglePhysicalSpace ? 1 : 0,
			VirtualTexture->GetSinglePhysicalSpace() ? 1 : 0,
			*VirtualTexture->GetName());

		bIsVirtualTextureValid = false;
	}
	else if ((VirtualTexture->GetAdaptivePageTable()) != bAdaptive)
	{
		Compiler->Errorf(TEXT("Adaptive page table is '%d', should be '%d' to match %s"),
			bAdaptive ? 1 : 0,
			VirtualTexture->GetAdaptivePageTable() ? 1 : 0,
			*VirtualTexture->GetName());

		bIsVirtualTextureValid = false;
	}

	// Calculate the virtual texture layer and sampling/unpacking functions for this output
	// Fallback to a sensible default value if the output isn't valid for the bound virtual texture
	uint32 UnpackTarget = 0;
	uint32 UnpackMask = 0;
	EVirtualTextureUnpackType UnpackType = EVirtualTextureUnpackType::None;

	bool bIsBaseColorValid = false;
	bool bIsSpecularValid = false;
	bool bIsNormalValid = false;
	bool bIsWorldHeightValid = false;
	bool bIsMaskValid = false;
	bIsBaseColorValid = bIsNormalValid = bIsSpecularValid = bIsMaskValid = true;

	switch (OutputIndex)
	{
	case 0:
		if ((bIsParameter || bIsVirtualTextureValid) && bIsSpecularValid)
		{
			UnpackTarget = 2; UnpackMask = 0x1 | 0x2 | 0x8;
		}
		else
		{
			return Compiler->Constant3(0.f, 0.f, 0.f);
		}
		break;
	case 1:
		if ((bIsParameter || bIsVirtualTextureValid) && bIsBaseColorValid)
		{
			UnpackType = EVirtualTextureUnpackType::BaseColorYCoCg;
		}
		else
		{
			return Compiler->Constant3(0.f, 0.f, 0.f);
		}
		break;
	case 2:
		if ((bIsParameter || bIsVirtualTextureValid) && bIsNormalValid)
		{
			UnpackType = EVirtualTextureUnpackType::NormalBC5BC1;
		}
		else
		{
			return Compiler->Constant3(0.f, 0.f, 1.f);
		}
		break;
	}

	// Compile the texture object references
	const int32 TextureLayerCount = URuntimeVirtualTexture::GetLayerCount(ERuntimeVirtualTextureMaterialType::BaseColor_Normal_Specular_Mask_YCoCg);
	check(TextureLayerCount <= RuntimeVirtualTexture::MaxTextureLayers);

	int32 TextureCodeIndex[RuntimeVirtualTexture::MaxTextureLayers] = { INDEX_NONE };
	int32 TextureReferenceIndex[RuntimeVirtualTexture::MaxTextureLayers] = { INDEX_NONE };
	for (int32 TexureLayerIndex = 0; TexureLayerIndex < TextureLayerCount; TexureLayerIndex++)
	{
		const int32 PageTableLayerIndex = bSinglePhysicalSpace ? 0 : TexureLayerIndex;

		if (bIsParameter)
		{
			TextureCodeIndex[TexureLayerIndex] = Compiler->VirtualTextureParameter(GetParameterName(), VirtualTexture, TexureLayerIndex, PageTableLayerIndex, TextureReferenceIndex[TexureLayerIndex], SAMPLERTYPE_VirtualMasks);
		}
		else
		{
			TextureCodeIndex[TexureLayerIndex] = Compiler->VirtualTexture(VirtualTexture, TexureLayerIndex, PageTableLayerIndex, TextureReferenceIndex[TexureLayerIndex], SAMPLERTYPE_VirtualMasks);
		}
	}

	// Compile the runtime virtual texture uniforms
	int32 Uniforms[ERuntimeVirtualTextureShaderUniform_Count];
	for (int32 UniformIndex = 0; UniformIndex < ERuntimeVirtualTextureShaderUniform_Count; ++UniformIndex)
	{
		if (bIsParameter)
		{
			Uniforms[UniformIndex] = Compiler->VirtualTextureUniform(GetParameterName(), TextureReferenceIndex[0], UniformIndex);
		}
		else
		{
			Uniforms[UniformIndex] = Compiler->VirtualTextureUniform(TextureReferenceIndex[0], UniformIndex);
		}
	}

	// Compile the coordinates
	// We use the virtual texture world space transform by default
	int32 CoordinateIndex = INDEX_NONE;

	if (Coordinates.GetTracedInput().Expression != nullptr && WorldPosition.GetTracedInput().Expression != nullptr)
	{
		Compiler->Errorf(TEXT("Only one of 'Coordinates' and 'WorldPosition' can be used"));
	}

	if (Coordinates.GetTracedInput().Expression != nullptr)
	{
		CoordinateIndex = Coordinates.Compile(Compiler);
	}
	else
	{
		int32 WorldPositionIndex = INDEX_NONE;
		if (WorldPosition.GetTracedInput().Expression != nullptr)
		{
			WorldPositionIndex = WorldPosition.Compile(Compiler);
		}
		else
		{
			WorldPositionIndex = Compiler->WorldPosition(WPT_Default);
		}

		const int32 P0 = Uniforms[ERuntimeVirtualTextureShaderUniform_WorldToUVTransform0];
		const int32 P1 = Uniforms[ERuntimeVirtualTextureShaderUniform_WorldToUVTransform1];
		const int32 P2 = Uniforms[ERuntimeVirtualTextureShaderUniform_WorldToUVTransform2];
		CoordinateIndex = Compiler->VirtualTextureWorldToUV(WorldPositionIndex, P0, P1, P2);
	}

	// Compile the texture sample code
	const bool bAutomaticMipViewBias = true;
	int32 SampleCodeIndex[RuntimeVirtualTexture::MaxTextureLayers] = { INDEX_NONE };
	for (int32 TexureLayerIndex = 0; TexureLayerIndex < TextureLayerCount; TexureLayerIndex++)
	{
		SampleCodeIndex[TexureLayerIndex] = Compiler->TextureSample(
			TextureCodeIndex[TexureLayerIndex],
			CoordinateIndex,
			SAMPLERTYPE_VirtualMasks,
			INDEX_NONE, INDEX_NONE, TMVM_None, SSM_Clamp_WorldGroupSettings,
			TextureReferenceIndex[TexureLayerIndex],
			bAutomaticMipViewBias, bAdaptive);
	}

	// Compile any unpacking code
	int32 UnpackCodeIndex = INDEX_NONE;
	if (UnpackType != EVirtualTextureUnpackType::None)
	{
		int32 P0 = Uniforms[ERuntimeVirtualTextureShaderUniform_WorldHeightUnpack];
		UnpackCodeIndex = Compiler->VirtualTextureUnpack(SampleCodeIndex[0], SampleCodeIndex[1], SampleCodeIndex[2], P0, UnpackType);
	}
	else
	{
		UnpackCodeIndex = SampleCodeIndex[UnpackTarget] == INDEX_NONE ? INDEX_NONE : Compiler->ComponentMask(SampleCodeIndex[UnpackTarget], UnpackMask & 1, (UnpackMask >> 1) & 1, (UnpackMask >> 2) & 1, (UnpackMask >> 3) & 1);
	}
	return UnpackCodeIndex;
}

void UTGOR_WorldPainterSamplerMaterialExpression::GetCaption(TArray<FString>& OutCaptions) const
{
	OutCaptions.Add(FString(TEXT("World Painter Sample Param ")));
	OutCaptions.Add(FString::Printf(TEXT("'%s'"), *ParameterName.ToString()));
}

#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE