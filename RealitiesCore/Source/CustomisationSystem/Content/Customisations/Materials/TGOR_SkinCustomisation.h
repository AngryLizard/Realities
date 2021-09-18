// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_SurfaceCustomisation.h"
#include "TGOR_SkinCustomisation.generated.h"

class UTGOR_Skin;
class UTGOR_Mask;
class UTGOR_Palette;
class UTGOR_Canvas;

/**
*
*/
UCLASS()
class CUSTOMISATIONSYSTEM_API UTGOR_SkinCustomisation : public UTGOR_SurfaceCustomisation
{
	GENERATED_BODY()

public:
	UTGOR_SkinCustomisation();

	virtual bool Check(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart) override;
	virtual void Apply(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation) override;
	virtual void Query(const UTGOR_CustomisationComponent* Master, FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation) const override;

	virtual void Write(FTGOR_GroupWritePackage& Package, const FTGOR_CustomisationPayload& Payload) override;
	virtual bool Read(FTGOR_GroupReadPackage& Package, FTGOR_CustomisationPayload& Payload) const override;

	/** Get payload */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void ToPayload(FTGOR_CustomisationPayload& Payload, UTGOR_Skin* Skin, UTGOR_Mask* Mask, UTGOR_Palette* Palette) const;

	/** Set payload */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void FromPayload(const FTGOR_CustomisationPayload& Payload, UTGOR_Skin*& Skin, UTGOR_Mask*& Mask, UTGOR_Palette*& Palette);

};
