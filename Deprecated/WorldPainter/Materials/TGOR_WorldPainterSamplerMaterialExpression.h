// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Materials/MaterialExpression.h"
#include "VT/RuntimeVirtualTexture.h"
#include "Materials/MaterialExpressionRuntimeVirtualTextureSampleParameter.h"
#include "TGOR_WorldPainterSamplerMaterialExpression.generated.h"
//


/** Sample RVTs from world painter */
UCLASS(collapsecategories, hidecategories = Object, MinimalAPI)
class UTGOR_WorldPainterSamplerMaterialExpression : public UMaterialExpressionRuntimeVirtualTextureSampleParameter
{
	GENERATED_UCLASS_BODY()

	/** Init settings that affect shader compilation and need to match the current VirtualTexture */
	bool InitVirtualTextureDependentSettings();
		
protected:
	/** Initialize the output pins. */
	void InitOutputs();
		
		
#if WITH_EDITOR
	/** If this is the named parameter from this material expression, then set its value. */
	bool SetParameterValue(FName InParameterName, URuntimeVirtualTexture* InValue);
#endif
		
	//~ Begin UMaterialExpression Interface
	virtual UObject* GetReferencedTexture() const override;
	virtual bool CanReferenceTexture() const { return true; }
		
	/** Return whether this is the named parameter from this material expression, and if it is then return its value. */
	bool IsNamedParameter(const FHashedMaterialParameterInfo& ParameterInfo, URuntimeVirtualTexture*& OutValue) const;
	/** Adds to arrays of parameter info and id with the values used by this material expression. */
	void GetAllParameterInfo(TArray<FMaterialParameterInfo>& OutParameterInfo, TArray<FGuid>& OutParameterIds, const FMaterialParameterInfo& InBaseParameterInfo) const;
		
#if WITH_EDITOR
	virtual bool CanRenameNode() const override { return true; }
	virtual void SetEditableName(const FString& NewName) override;
	virtual FString GetEditableName() const override;
	virtual bool HasAParameterName() const override { return true; }
	virtual void SetParameterName(const FName& Name) override { ParameterName = Name; }
	virtual FName GetParameterName() const override { return ParameterName; }
	virtual void ValidateParameterName(const bool bAllowDuplicateName) override;
	virtual bool MatchesSearchQuery(const TCHAR* SearchQuery) override;
	virtual void SetValueToMatchingExpression(UMaterialExpression* OtherExpression) override;
		
	virtual void PostLoad() override;
	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
public:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
		
	virtual FGuid& GetParameterExpressionId() override { return ExpressionGUID; }
	//~ End UMaterialExpression Interface
};


///** Sample RVTs from world painter */
//UCLASS(collapsecategories, hidecategories = Object, MinimalAPI)
//class UTGOR_WorldPainterSamplerMaterialExpression : public UMaterialExpression
//{
//	GENERATED_UCLASS_BODY()
//
//	/** Name to be referenced when we want to find and set this parameter */
//	UPROPERTY(EditAnywhere, Category = MaterialParameter)
//		FName ParameterName;
//
//	/** GUID that should be unique within the material, this is used for parameter renaming. */
//	UPROPERTY()
//		FGuid ExpressionGUID;
//
//	/** The name of the parameter Group to display in MaterialInstance Editor. Default is None group */
//	UPROPERTY(EditAnywhere, Category = MaterialParameter)
//		FName Group;
//
//#if WITH_EDITORONLY_DATA
//	/** Controls where the this parameter is displayed in a material instance parameter list. The lower the number the higher up in the parameter list. */
//	UPROPERTY(EditAnywhere, Category = MaterialParameter)
//		int32 SortPriority = 32;
//#endif
//
//
//	/** Optional UV coordinates input if we want to override standard world position based coordinates. */
//	UPROPERTY(meta = (RequiredInput = "false"))
//		FExpressionInput Coordinates;
//
//	/** Optional world position input to override the default world position. */
//	UPROPERTY(meta = (RequiredInput = "false"))
//		FExpressionInput WorldPosition;
//
//	/** Meaning depends on MipValueMode. A single unit is one mip level.  */
//	UPROPERTY(meta = (RequiredInput = "false"))
//		FExpressionInput MipValue;
//
//	/** The virtual texture object to sample. */
//	UPROPERTY(EditAnywhere, Category = VirtualTexture)
//		class URuntimeVirtualTexture* VirtualTexture;
//
//	/** Enable page table channel packing. Note that the bound Virtual Texture should have the same setting for sampling to work correctly. */
//	UPROPERTY(EditAnywhere, Category = VirtualTexture, meta = (DisplayName = "Enable packed page table"))
//		bool bSinglePhysicalSpace = true;
//
//	/** Enable sparse adaptive page tables. Note that the bound Virtual Texture should have valid adaptive virtual texture settings for sampling to work correctly. */
//	UPROPERTY(EditAnywhere, Category = VirtualTexture, meta = (DisplayName = "Enable adaptive page table"))
//		bool bAdaptive = false;
//
//	/** Init settings that affect shader compilation and need to match the current VirtualTexture */
//	bool InitVirtualTextureDependentSettings();
//
//protected:
//	/** Initialize the output pins. */
//	void InitOutputs();
//
//
//#if WITH_EDITOR
//	/** If this is the named parameter from this material expression, then set its value. */
//	bool SetParameterValue(FName InParameterName, URuntimeVirtualTexture* InValue);
//#endif
//
//	//~ Begin UMaterialExpression Interface
//	virtual UObject* GetReferencedTexture() const override;
//	virtual bool CanReferenceTexture() const { return true; }
//
//	/** Return whether this is the named parameter from this material expression, and if it is then return its value. */
//	bool IsNamedParameter(const FHashedMaterialParameterInfo& ParameterInfo, URuntimeVirtualTexture*& OutValue) const;
//	/** Adds to arrays of parameter info and id with the values used by this material expression. */
//	void GetAllParameterInfo(TArray<FMaterialParameterInfo>& OutParameterInfo, TArray<FGuid>& OutParameterIds, const FMaterialParameterInfo& InBaseParameterInfo) const;
//
//#if WITH_EDITOR
//	virtual bool CanRenameNode() const override { return true; }
//	virtual void SetEditableName(const FString& NewName) override;
//	virtual FString GetEditableName() const override;
//	virtual bool HasAParameterName() const override { return true; }
//	virtual void SetParameterName(const FName& Name) override { ParameterName = Name; }
//	virtual FName GetParameterName() const override { return ParameterName; }
//	virtual void ValidateParameterName(const bool bAllowDuplicateName) override;
//	virtual bool MatchesSearchQuery(const TCHAR* SearchQuery) override;
//	virtual void SetValueToMatchingExpression(UMaterialExpression* OtherExpression) override;
//
//	virtual void PostLoad() override;
//	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;
//	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
//public:
//	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
//#endif
//
//	virtual FGuid& GetParameterExpressionId() override { return ExpressionGUID; }
//	//~ End UMaterialExpression Interface
//};
