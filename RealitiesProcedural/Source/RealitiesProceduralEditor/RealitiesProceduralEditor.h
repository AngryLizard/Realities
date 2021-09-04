// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

/**
 * The public interface to this module
 */
class FRealitiesProceduralEditor : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TSharedPtr<class FTGOR_AssetTypeActions_PaintGradient> AssetTypeActions_PaintGradient;
	TSharedPtr<class FTGOR_AssetTypeActions_ExteriorGradient> AssetTypeActions_ExteriorGradient;
	TSharedPtr<class FTGOR_AssetTypeActions_WorldPainterTexture> AssetTypeActions_WorldPainterTexture;

	void OnPlacementModeRefresh(FName CategoryName);
};