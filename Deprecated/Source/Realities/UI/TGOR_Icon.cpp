// The Gateway of Realities: Planes of Existence.


#include "TGOR_Icon.h"
#include "PaperSprite.h"


void UTGOR_Icon::ClearDisplay()
{
	ResetVisuals();
}

void UTGOR_Icon::Display(const FTGOR_Display& Display)
{
	Visualise(Display);
}

FVector2D UTGOR_Icon::GetSourceSize(UPaperSprite* Sprite)
{
	UTexture2D* Texture = Sprite->GetBakedTexture();
	if (IsValid(Texture))
	{
		float factor = Sprite->GetPixelsPerUnrealUnit() * 2;
		FVector Extend = Sprite->GetRenderBounds().BoxExtent * factor;

		return FVector2D(Extend.X, Extend.Z);
	}

	return FVector2D(0.0f, 0.0f);
}