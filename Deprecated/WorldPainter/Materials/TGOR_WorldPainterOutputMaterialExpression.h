// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Materials/MaterialExpressionCustomOutput.h"
#include "TGOR_WorldPainterOutputMaterialExpression.generated.h"


/** Node which outputs a texture object contained in a UHeightfieldMinMaxTexture. */
UCLASS(collapsecategories, hidecategories = Object, MinimalAPI)
class UTGOR_WorldPainterOutputMaterialExpression : public UMaterialExpressionCustomOutput
{
	GENERATED_UCLASS_BODY()

	/** Input for Base Color to output to virtual texture. */
	UPROPERTY()
		FExpressionInput BaseColor;

	/** Input for Specular to output to virtual texture. */
	UPROPERTY()
		FExpressionInput Specular;

	/** Input for Roughness to output to virtual texture. */
	UPROPERTY()
		FExpressionInput Roughness;

	/** Input for Surface Normal to output to virtual texture. */
	UPROPERTY()
		FExpressionInput Normal;

	/** Input for World Height to output to virtual texture. */
	UPROPERTY()
		FExpressionInput WorldHeight;

	/** Input for Opacity value used for blending to virtual texture. */
	UPROPERTY()
		FExpressionInput Opacity;

	/** Input for Mask to output to virtual texture. */
	UPROPERTY()
		FExpressionInput Mask;

public:
#if WITH_EDITOR
	//~ Begin UMaterialExpression Interface
	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
	//~ End UMaterialExpression Interface
#endif

	//~ Begin UMaterialExpressionCustomOutput Interface
	virtual int32 GetNumOutputs() const override;
	virtual FString GetFunctionName() const override;
	virtual FString GetDisplayName() const override;
	//~ End UMaterialExpressionCustomOutput Interface
};

	/** Primary color input. */
	//UPROPERTY()
	//	FExpressionInput Primary;

	/** Secondary color input. */
	//UPROPERTY()
	//	FExpressionInput Secondary;

	/** Normal input for masking layer. */
	//UPROPERTY()
	//	FExpressionInput Mask;