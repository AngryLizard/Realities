// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Materials/MaterialExpression.h"
#include "TGOR_MaterialExpressionWorldPainterTexture.generated.h"

class UTGOR_WorldPainterTexture;


/** Node which outputs a texture object contained in a UHeightfieldMinMaxTexture. */
UCLASS(collapsecategories, hidecategories = Object, MinimalAPI)
class UTGOR_MaterialExpressionWorldPainterTexture : public UMaterialExpression
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(meta = (RequiredInput = "true", ToolTip = "2D location of canvas"))
		FExpressionInput Location;

	UPROPERTY(meta = (RequiredInput = "true", ToolTip = "2D size of canvas"))
		FExpressionInput Size;

	UPROPERTY(meta = (RequiredInput = "true", ToolTip = "Rotation of canvas in radians"))
		FExpressionInput Angle;

	UPROPERTY(meta = (RequiredInput = "true", ToolTip = "Shading hi"))
		FExpressionInput High;

	UPROPERTY(meta = (RequiredInput = "false", ToolTip = "Shading lo"))
		FExpressionInput Low;

	UPROPERTY(meta = (RequiredInput = "false", ToolTip = "Shading hi"))
		FExpressionInput Shading;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MaterialExpressionTexture)
		UTGOR_WorldPainterTexture* WorldPainterTexture;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MaterialExpressionTexture)
		TEnumAsByte<enum EMaterialSamplerType> SamplerType;
#endif

protected:
#if WITH_EDITOR
	//~ Begin UObject Interface
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End UObject Interface
#endif

	//~ Begin UMaterialExpression Interface
#if WITH_EDITOR
	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
#endif
	virtual UObject* GetReferencedTexture() const override;
	virtual bool CanReferenceTexture() const override { return true; }
	//~ End UMaterialExpression Interface
};
