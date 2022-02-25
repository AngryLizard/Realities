// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Mod/Customisations/MeshMorphs/TGOR_MeshMorphCustomisation.h"
#include "TGOR_CustomMeshMorphCustomisation.generated.h"

/**
*
*/
UCLASS()
class REALITIES_API UTGOR_CustomMeshMorphCustomisation : public UTGOR_MeshMorphCustomisation
{
	GENERATED_BODY()

public:
	UTGOR_CustomMeshMorphCustomisation();

	virtual void Apply(UTGOR_ModularSkeletalMeshComponent* Master, const TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance) override;
	virtual void Query(UTGOR_ModularSkeletalMeshComponent* Master, TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent, UMaterialInstanceDynamic* Instance) const override;

	virtual void Write(FTGOR_GroupWritePackage& Package, const TArray<uint8>& Payload) override;
	virtual bool Read(FTGOR_GroupReadPackage& Package, TArray<uint8>& Payload) const override;

	/** Apply meshmprph */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		void ApplyMeshMorph(UTGOR_ModularSkeletalMeshComponent* Master, const TArray<uint8>& Payload, const FName& Param, UTGOR_Bodypart* Bodypart, USkeletalMeshComponent* MeshComponent);

	/** Query material */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		void QueryMeshMorph(UTGOR_ModularSkeletalMeshComponent* Master, TArray<uint8>& Payload, UTGOR_Bodypart* Bodypart, UMaterialInstanceDynamic* Instance) const;

	/** Write payload to package */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		void WriteMeshMorph(UPARAM(ref) FTGOR_GroupWritePackage& Package, const TArray<uint8>& Payload);

	/** Read payload from package */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Customisation", Meta = (Keywords = "C++"))
		bool ReadMeshMorph(UPARAM(ref) FTGOR_GroupReadPackage& Package, TArray<uint8>& Payload) const;

};
