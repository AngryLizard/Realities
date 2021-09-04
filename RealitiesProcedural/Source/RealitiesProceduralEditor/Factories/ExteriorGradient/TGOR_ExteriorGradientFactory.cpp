// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGOR_ExteriorGradientFactory.h"
#include "RealitiesProcedural/Textures/TGOR_ExteriorGradient.h"


#define LOCTEXT_NAMESPACE "TGOR_ExteriorGradientFactory"

UTGOR_ExteriorGradientFactory::UTGOR_ExteriorGradientFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UTGOR_ExteriorGradient::StaticClass();
}

FText UTGOR_ExteriorGradientFactory::GetDisplayName() const
{
	return LOCTEXT("TGOR_ExteriorGradientFactoryDescription", "Exterior gradient texture");
}

bool UTGOR_ExteriorGradientFactory::ConfigureProperties()
{
	return true;
}

bool UTGOR_ExteriorGradientFactory::CanCreateNew() const
{
	return true;
}

UObject* UTGOR_ExteriorGradientFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UTGOR_ExteriorGradient* ExteriorGradient = CastChecked<UTGOR_ExteriorGradient>(CreateOrOverwriteAsset(UTGOR_ExteriorGradient::StaticClass(), InParent, Name, Flags));

	if (ExteriorGradient == nullptr)
	{
		UE_LOG(LogTexture, Warning, TEXT("ExteriorGradient creation failed.\n"));
	}

	return ExteriorGradient;
}

#undef LOCTEXT_NAMESPACE
