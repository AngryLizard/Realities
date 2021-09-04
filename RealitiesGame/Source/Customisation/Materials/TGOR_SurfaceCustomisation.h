// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CustomisationSystem/Content/Customisations/TGOR_MaterialCustomisation.h"
#include "TGOR_SurfaceCustomisation.generated.h"

class UTGOR_Skin;
class UTGOR_Mask;
class UTGOR_Palette;
class UTGOR_Canvas;

/**
*
*/
UCLASS()
class CUSTOMISATION_API UTGOR_SurfaceCustomisation : public UTGOR_MaterialCustomisation
{
	GENERATED_BODY()

public:
	UTGOR_SurfaceCustomisation();
	virtual void BuildResource() override;


	/** Checks whether this customisation can have this given mask */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		bool ValidateMask(UTGOR_Mask* Mask, UTGOR_Bodypart* Bodypart) const;

	/** Get mask from input to make sure output is non-zero (except on wrong bodypart configuration) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		UTGOR_Mask* GetMandatoryMask(UTGOR_Mask* Mask, UTGOR_Bodypart* Bodypart) const;

	/** Checks whether this customisation can have this given skin */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		bool ValidateSkin(UTGOR_Skin* Skin, UTGOR_Bodypart* Bodypart) const;

	/** Get skin from input to make sure output is non-zero (except on wrong bodypart configuration) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		UTGOR_Skin* GetMandatorySkin(UTGOR_Skin* Skin, UTGOR_Bodypart* Bodypart) const;

	/** Get palette from input to make sure output is non-zero (except on wrong bodypart configuration) */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		UTGOR_Palette* GetMandatoryPalette(UTGOR_Palette* Palette, UTGOR_Bodypart* Bodypart) const;

	// /////////////////////////////////////////////////////////////////////

	/** Write/Merge materials for each texture type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Customisation")
		TMap<TSubclassOf<UTGOR_Canvas>, UMaterialInterface*> Materials;

	// /////////////////////////////////////////////////////////////////////

	/** Write/Merge material parameter name for normalmap */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Customisation")
		FName NormalName;

	/** Write/Merge material parameter name for surface */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Customisation")
		FName SurfaceName;

	/** Write/Merge material parameter name for mask */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Customisation")
		FName MaskName;

	/** Write/Merge skin parameter name for normalmap */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Customisation")
		FName ColorName;

	/** Write/Merge skin parameter name for surface */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Customisation")
		FName MaterialName;

	// /////////////////////////////////////////////////////////////////////

	/** Filter for skintype that can be used for this customisation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Customisation")
		TSubclassOf<UTGOR_Skin> SkinFilter;

	/** Filter for masktype that can be used for this customisation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Customisation")
		TSubclassOf<UTGOR_Mask> MaskFilter;

protected:

	/** */
	UPROPERTY()
		TMap<TSubclassOf<UTGOR_Canvas>, UMaterialInstanceDynamic*> DynamicMaterials;

};
