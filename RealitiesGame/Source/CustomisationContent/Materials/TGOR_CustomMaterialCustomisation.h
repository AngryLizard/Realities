// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CustomisationSystem/Content/Customisations/TGOR_MaterialCustomisation.h"
#include "TGOR_CustomMaterialCustomisation.generated.h"

/**
*
*/
UCLASS()
class CUSTOMISATIONCONTENT_API UTGOR_CustomMaterialCustomisation : public UTGOR_MaterialCustomisation
{
	GENERATED_BODY()

public:
	UTGOR_CustomMaterialCustomisation();

	virtual void Apply(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation) override;
	virtual void Query(const UTGOR_CustomisationComponent* Master, FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation) const override;

	virtual void Write(FTGOR_GroupWritePackage& Package, const FTGOR_CustomisationPayload& Payload) override;
	virtual bool Read(FTGOR_GroupReadPackage& Package, FTGOR_CustomisationPayload& Payload) const override;

	/** Apply material */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void ApplyMaterial(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, int32 NodeIndex, UMaterialInstanceDynamic* Instance);

	/** Query material */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void QueryMaterial(const UTGOR_CustomisationComponent* Master, FTGOR_CustomisationPayload& Payload, int32 NodeIndex, UMaterialInstanceDynamic* Instance) const;

	/** Write payload to package */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void WriteMaterial(UPARAM(ref) FTGOR_GroupWritePackage& Package, const FTGOR_CustomisationPayload& Payload);

	/** Read payload from package */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		bool ReadMaterial(UPARAM(ref) FTGOR_GroupReadPackage& Package, FTGOR_CustomisationPayload& Payload) const;
};
