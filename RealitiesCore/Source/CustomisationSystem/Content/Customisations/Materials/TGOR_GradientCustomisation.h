// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CustomisationSystem/Content/Customisations/Materials/TGOR_MaterialCustomisation.h"
#include "TGOR_GradientCustomisation.generated.h"

class UTGOR_Palette;

/**
*
*/
UCLASS()
class CUSTOMISATIONSYSTEM_API UTGOR_GradientCustomisation : public UTGOR_MaterialCustomisation
{
	GENERATED_BODY()

public:
	UTGOR_GradientCustomisation();

	virtual bool Check(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart) override;
	virtual void Apply(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation) override;
	virtual void Query(const UTGOR_CustomisationComponent* Master, FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation) const override;

	virtual void Write(FTGOR_GroupWritePackage& Package, const FTGOR_CustomisationPayload& Payload) override;
	virtual bool Read(FTGOR_GroupReadPackage& Package, FTGOR_CustomisationPayload& Payload) const override;

	/** Get payload */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void ToPayload(FTGOR_CustomisationPayload& Payload, UTGOR_Palette* Palette, float ValueX, float ValueY) const;

	/** Set payload */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void FromPayload(const FTGOR_CustomisationPayload& Payload, UTGOR_Palette*& Palette, float& ValueX, float& ValueY);


	/** Palette parameter name for material */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Customisation")
		FName PaletteParam;

	/** Material parameter name for material */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Customisation")
		FName MaterialParam;

	/** X Bias parameter name for material */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Customisation")
		FName ValueXParam;

	/** Y Bias parameter name for material */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Customisation")
		FName ValueYParam;
};
