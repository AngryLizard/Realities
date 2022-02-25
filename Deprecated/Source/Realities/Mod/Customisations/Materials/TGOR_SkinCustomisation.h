// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Mod/Customisations/Materials/TGOR_MaterialCustomisation.h"
#include "TGOR_SkinCustomisation.generated.h"

class UTGOR_Skin;

/**
*
*/
UCLASS()
class REALITIES_API UTGOR_SkinCustomisation : public UTGOR_MaterialCustomisation
{
	GENERATED_BODY()

public:
	UTGOR_SkinCustomisation();

	virtual bool Validate(UTGOR_ModularSkeletalMeshComponent* Master, const TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart) override;
	virtual void Apply(UTGOR_ModularSkeletalMeshComponent* Master, const TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance) override;
	virtual void Query(UTGOR_ModularSkeletalMeshComponent* Master, TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance) const override;

	virtual void Write(FTGOR_GroupWritePackage& Package, const TArray<uint8>& Payload) override;
	virtual bool Read(FTGOR_GroupReadPackage& Package, TArray<uint8>& Payload) const override;

	/** Get payload */
	UFUNCTION(BlueprintPure, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		void ToPayload(TArray<uint8>& Payload, UTGOR_Skin* Content) const;

	/** Set payload */
	UFUNCTION(BlueprintPure, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		void FromPayload(const TArray<uint8>& Payload, UTGOR_Skin*& Content);

};
