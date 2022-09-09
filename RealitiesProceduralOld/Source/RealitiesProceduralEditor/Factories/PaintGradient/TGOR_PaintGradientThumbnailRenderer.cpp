// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGOR_PaintGradientThumbnailRenderer.h"
#include "RealitiesProcedural/Textures/TGOR_PaintGradient.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"

//////////////////////////////////////////////////////////////////////////
// UPaperSpriteThumbnailRenderer

UTGOR_PaintGradientThumbnailRenderer::UTGOR_PaintGradientThumbnailRenderer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTGOR_PaintGradientThumbnailRenderer::Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* RenderTarget, FCanvas* Canvas, bool bAdditionalViewFamily)
{
	UTGOR_PaintGradient* PaintGradient = Cast<UTGOR_PaintGradient>(Object);

	if (IsValid(PaintGradient) && PaintGradient->GetResource() != nullptr)
	{
		// Use A canvas tile item to draw
		FCanvasTileItem CanvasTile(FVector2D(X, Y), PaintGradient->GetResource(), FVector2D(Width, Height), FLinearColor::White);
		CanvasTile.BlendMode = SE_BLEND_Opaque;
		CanvasTile.Draw(Canvas);
	}
}
