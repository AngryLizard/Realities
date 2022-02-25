// The Gateway of Realities: Planes of Existence.


#include "TGOR_Colour.h"


UTGOR_Colour::UTGOR_Colour()
	: Super()
{
	Lights = FLinearColor();
	Medium = FLinearColor();
	Darks = FLinearColor();

	Display.UseCustomColor = true;
	Display.CustomColor = FLinearColor(1.0f, 1.0f, 1.0f);

	Roughness = 1.0f;
	Metallic = 0.0f;
	Glow = 0.0f;
	EffectControl = 0.0f;
}

