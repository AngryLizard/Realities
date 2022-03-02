// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGOR_ExteriorGradientThumbnailRenderer.h"
#include "RealitiesProcedural/Textures/TGOR_ExteriorGradient.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"

//////////////////////////////////////////////////////////////////////////
// UPaperSpriteThumbnailRenderer

UTGOR_ExteriorGradientThumbnailRenderer::UTGOR_ExteriorGradientThumbnailRenderer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTGOR_ExteriorGradientThumbnailRenderer::Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* RenderTarget, FCanvas* Canvas, bool bAdditionalViewFamily)
{
	UTGOR_ExteriorGradient* ExteriorGradient = Cast<UTGOR_ExteriorGradient>(Object);

	if (IsValid(ExteriorGradient) && ExteriorGradient->GetResource() != nullptr)
	{
		// Use A canvas tile item to draw
		FCanvasTileItem CanvasTile(FVector2D(X, Y), ExteriorGradient->GetResource(), FVector2D(Width, Height), FLinearColor::White);
		CanvasTile.BlendMode = SE_BLEND_Opaque;
		CanvasTile.Draw(Canvas);
	}
}
