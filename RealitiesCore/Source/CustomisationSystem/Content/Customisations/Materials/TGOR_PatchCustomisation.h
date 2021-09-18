// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_SurfaceCustomisation.h"
#include "TGOR_PatchCustomisation.generated.h"

class UTGOR_Skin;
class UTGOR_Mask;
class UTGOR_Palette;
class UTGOR_Canvas;

/**
*
*/
UCLASS()
class CUSTOMISATIONSYSTEM_API UTGOR_PatchCustomisation : public UTGOR_SurfaceCustomisation
{
	GENERATED_BODY()

public:
	UTGOR_PatchCustomisation();
	virtual void BuildResource() override;

	virtual bool Check(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart) override;
	virtual void Apply(UTGOR_CustomisationComponent* Master, const FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation) override;
	virtual void Query(const UTGOR_CustomisationComponent* Master, FTGOR_CustomisationPayload& Payload, UTGOR_Bodypart* Bodypart, int32 NodeIndex, UMaterialInstanceDynamic* Instance, const FTGOR_CustomisationMeshSection& Allocation) const override;

	virtual void Write(FTGOR_GroupWritePackage& Package, const FTGOR_CustomisationPayload& Payload) override;
	virtual bool Read(FTGOR_GroupReadPackage& Package, FTGOR_CustomisationPayload& Payload) const override;


	/** Get payload */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void ToPayload(FTGOR_CustomisationPayload& Payload, const FTransform& Transform, UTGOR_Skin* Skin, UTGOR_Mask* Mask, UTGOR_Palette* Palette) const;

	/** Set payload */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void FromPayload(const FTGOR_CustomisationPayload& Payload, FTransform& Transform, UTGOR_Skin*& Skin, UTGOR_Mask*& Mask, UTGOR_Palette*& Palette);

	// /////////////////////////////////////////////////////////////////////

	/** Write/Merge material parameter name for the mesh extend */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Customisation")
		FName ExtendName;

	/** Write/Merge material parameter name for the mesh center */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Customisation")
		FName CenterName;

	/** Write/Merge material parameter name for the vertex location bake texture */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Customisation")
		FName LocationBakeName;

	/** Write/Merge material parameter name for the vertex tangent bake texture */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Customisation")
		FName TangentBakeName;

	/** Write/Merge material parameter name for the vertex normal bake texture */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Customisation")
		FName NormalBakeName;

	// /////////////////////////////////////////////////////////////////////

	/** Write/Merge material parameter name for the patch location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Customisation")
		FName LocationName;

	/** Write/Merge material parameter name for the patch forward direction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Customisation")
		FName ForwardName;

	/** Write/Merge material parameter name for the patch right direction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Customisation")
		FName RightName;

	/** Write/Merge material parameter name for the patch up direction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Customisation")
		FName UpName;

	/** Write/Merge material parameter name for the patch scale */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Customisation")
		FName ScaleName;

private:
};
