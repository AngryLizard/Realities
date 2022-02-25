// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Mod/Customisations/Materials/TGOR_MaterialCustomisation.h"
#include "TGOR_SliderCustomisation.generated.h"

/**
*
*/
UCLASS()
class REALITIES_API UTGOR_SliderCustomisation : public UTGOR_MaterialCustomisation
{
	GENERATED_BODY()

public:
	UTGOR_SliderCustomisation();

	virtual void Apply(UTGOR_ModularSkeletalMeshComponent* Master, const TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance) override;
	virtual void Query(UTGOR_ModularSkeletalMeshComponent* Master, TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance) const override;

	virtual void Write(FTGOR_GroupWritePackage& Package, const TArray<uint8>& Payload) override;
	virtual bool Read(FTGOR_GroupReadPackage& Package, TArray<uint8>& Payload) const override;

	/** Get payload from slider */
	UFUNCTION(BlueprintPure, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		void ToPayload(TArray<uint8>& Payload, float Value) const;

	/** Set payload to slider */
	UFUNCTION(BlueprintPure, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		void FromPayload(const TArray<uint8>& Payload, float& Value);

	/** Slider parameter name for material */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Customisation")
		FName SliderParam;
};
