// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CustomisationSystem/Content/Customisations/TGOR_MaterialCustomisation.h"
#include "TGOR_ColorCustomisation.generated.h"

/**
*
*/
UCLASS()
class CUSTOMISATION_API UTGOR_ColorCustomisation : public UTGOR_MaterialCustomisation
{
	GENERATED_BODY()

public:
	UTGOR_ColorCustomisation();

	virtual void Apply(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation) override;
	virtual void Query(const UTGOR_CustomisationComponent* Master, FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation) const override;

	virtual void Write(FTGOR_GroupWritePackage& Package, const FTGOR_CustomisationPayload& Payload) override;
	virtual bool Read(FTGOR_GroupReadPackage& Package, FTGOR_CustomisationPayload& Payload) const override;

	/** Get payload */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void ToPayload(FTGOR_CustomisationPayload& Payload, const FColor& Color) const;

	/** Set payload */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void FromPayload(const FTGOR_CustomisationPayload& Payload, FColor& Color);

	/** Color parameter name for material */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Customisation")
		FName ColorParam;
};
