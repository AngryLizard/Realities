// The Gateway of Realities: Planes of Existence.

#pragma once


#include "Realities/Mod/Customisations/MeshMorphs/TGOR_MeshMorphCustomisation.h"
#include "TGOR_NegativeFloatCustomisation.generated.h"

/**
*
*/
UCLASS()
class REALITIES_API UTGOR_NegativeFloatCustomisation : public UTGOR_MeshMorphCustomisation
{
	GENERATED_BODY()

public:
	UTGOR_NegativeFloatCustomisation();

	virtual void Apply(UTGOR_ModularSkeletalMeshComponent* Master, const TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance) override;
	virtual void Query(UTGOR_ModularSkeletalMeshComponent* Master, TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance) const override;

	virtual void Write(FTGOR_GroupWritePackage& Package, const TArray<uint8>& Payload) override;
	virtual bool Read(FTGOR_GroupReadPackage& Package, TArray<uint8>& Payload) const override;
	
	/** Get payload */
	UFUNCTION(BlueprintPure, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		void ToPayload(TArray<uint8>& Payload, float Value) const;

	/** Set payload */
	UFUNCTION(BlueprintPure, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		void FromPayload(const TArray<uint8>& Payload, float& Value);


	/** Negative parameter name for material */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Customisation")
		FName NegativeParam;
};
