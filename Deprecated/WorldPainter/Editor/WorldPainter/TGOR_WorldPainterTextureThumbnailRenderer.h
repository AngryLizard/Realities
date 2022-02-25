// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "ThumbnailRendering/DefaultSizedThumbnailRenderer.h"
#include "Materials/MaterialInstanceConstant.h"
#include "TGOR_WorldPainterTextureThumbnailRenderer.generated.h"

UCLASS(MinimalAPI)
class UTGOR_WorldPainterTextureThumbnailRenderer : public UDefaultSizedThumbnailRenderer
{
	GENERATED_UCLASS_BODY()

	//~ Begin UThumbnailRenderer Interface.
	virtual bool CanVisualizeAsset(UObject* Object) override;
	virtual void Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget*, FCanvas* Canvas, bool bAdditionalViewFamily) override;
	//~ EndUThumbnailRenderer Interface.

	// UObject implementation
	REALITIESPROCEDURALEDITOR_API virtual void BeginDestroy() override;

private:

	class FWorldPainterTextureThumbnailScene* ThumbnailScene;

	UPROPERTY()
		UMaterialInstanceConstant* MaterialInstance;
};
