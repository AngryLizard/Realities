// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGOR_PaintGradientFactory.h"
#include "RealitiesProcedural/Textures/TGOR_PaintGradient.h"


#define LOCTEXT_NAMESPACE "TGOR_PaintGradientFactory"

UTGOR_PaintGradientFactory::UTGOR_PaintGradientFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UTGOR_PaintGradient::StaticClass();
}

FText UTGOR_PaintGradientFactory::GetDisplayName() const
{
	return LOCTEXT("TGOR_PaintGradientFactoryDescription", "Paint Gradient Texture");
}

bool UTGOR_PaintGradientFactory::ConfigureProperties()
{
	return true;
}

bool UTGOR_PaintGradientFactory::CanCreateNew() const
{
	return true;
}

UObject* UTGOR_PaintGradientFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UTGOR_PaintGradient* PaintGradient = CastChecked<UTGOR_PaintGradient>(CreateOrOverwriteAsset(UTGOR_PaintGradient::StaticClass(), InParent, Name, Flags));

	if (PaintGradient == nullptr)
	{
		UE_LOG(LogTexture, Warning, TEXT("PaintGradient creation failed.\n"));
	}

	return PaintGradient;
}

#undef LOCTEXT_NAMESPACE
