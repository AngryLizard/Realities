// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CustomisationSystem/Content/Customisations/TGOR_MeshMorphCustomisation.h"
#include "TGOR_SingleFloatCustomisation.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMISATIONCONTENT_API UTGOR_SingleFloatCustomisation : public UTGOR_MeshMorphCustomisation
{
	GENERATED_BODY()
	
public:
	UTGOR_SingleFloatCustomisation();

	virtual void Apply(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation) override;
	virtual void Query(const UTGOR_CustomisationComponent* Master, FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation) const override;

	virtual void Write(FTGOR_GroupWritePackage& Package, const FTGOR_CustomisationPayload& Payload) override;
	virtual bool Read(FTGOR_GroupReadPackage& Package, FTGOR_CustomisationPayload& Payload) const override;

	virtual bool MergePayloads(const TArray<FTGOR_CustomisationPayload>& Payloads, FTGOR_CustomisationPayload& Merge) override;

	/** Get payload */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void ToPayload(FTGOR_CustomisationPayload& Payload, float Value) const;

	/** Set payload */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void FromPayload(const FTGOR_CustomisationPayload& Payload, float& Value);

	/** Parameter name for material */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Customisation")
		FName PositiveParam;
};
