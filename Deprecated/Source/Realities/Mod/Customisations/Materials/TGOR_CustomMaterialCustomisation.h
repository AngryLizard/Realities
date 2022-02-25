// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Mod/Customisations/Materials/TGOR_MaterialCustomisation.h"
#include "TGOR_CustomMaterialCustomisation.generated.h"

/**
*
*/
UCLASS()
class REALITIES_API UTGOR_CustomMaterialCustomisation : public UTGOR_MaterialCustomisation
{
	GENERATED_BODY()

public:
	UTGOR_CustomMaterialCustomisation();

	virtual void Apply(UTGOR_ModularSkeletalMeshComponent* Master, const TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance) override;
	virtual void Query(UTGOR_ModularSkeletalMeshComponent* Master, TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance) const override;

	virtual void Write(FTGOR_GroupWritePackage& Package, const TArray<uint8>& Payload) override;
	virtual bool Read(FTGOR_GroupReadPackage& Package, TArray<uint8>& Payload) const override;

	/** Apply material */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		void ApplyMaterial(UTGOR_ModularSkeletalMeshComponent* Master, const TArray<uint8>& Payload, UTGOR_Bodypart* Bodypart, UMaterialInstanceDynamic* Instance);

	/** Query material */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		void QueryMaterial(UTGOR_ModularSkeletalMeshComponent* Master, TArray<uint8>& Payload, UTGOR_Bodypart* Bodypart, UMaterialInstanceDynamic* Instance) const;

	/** Write payload to package */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		void WriteMaterial(UPARAM(ref) FTGOR_GroupWritePackage& Package, const TArray<uint8>& Payload);

	/** Read payload from package */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		bool ReadMaterial(UPARAM(ref) FTGOR_GroupReadPackage& Package, TArray<uint8>& Payload) const;
};
