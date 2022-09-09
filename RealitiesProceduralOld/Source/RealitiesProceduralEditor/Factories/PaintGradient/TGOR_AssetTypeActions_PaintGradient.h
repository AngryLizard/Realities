// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

class UTGOR_PaintGradient;

class FTGOR_AssetTypeActions_PaintGradient : public FAssetTypeActions_Base
{
public:
	// IAssetTypeActions Implementation
	virtual FText GetName() const override { return NSLOCTEXT("PaintGradient", "TGOR_AssetTypeActions_PaintGradient", "Procedural Gradient"); }
	virtual FColor GetTypeColor() const override { return FColor((uint8)0xFF, (uint8)0xAA, (uint8)0xAA); }
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
	virtual uint32 GetCategories() override { return EAssetTypeCategories::Type::Textures; }
	virtual UClass* GetSupportedClass() const override;
	virtual class UThumbnailInfo* GetThumbnailInfo(UObject* Asset) const override;

};
