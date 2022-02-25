// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Realities/Base/Instances/Creature/TGOR_BodypartInstance.h"
#include "Realities/Mod/Customisations/TGOR_Customisation.h"

#include "Engine/SkeletalMesh.h"
#include "Materials/MaterialInstance.h"

#include "Realities/Base/Content/TGOR_Content.h"
#include "TGOR_Bodypart.generated.h"

class UTGOR_Skin;
class UTGOR_Colour;
class UTGOR_MaterialCustomisation;
class UTGOR_MeshMorphCustomisation;

class UTGOR_ContentManager;
class UTGOR_Bodypart;


////////////////////////////////////////////// STRUCT //////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FTGOR_BodypartCustomisationMapping
{
	GENERATED_USTRUCT_BODY()

	/** Material parameter */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Bodypart", Meta = (Categories = "MaterialCustomisation"))
		FName Param;

	/** Material index */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Bodypart")
		int32 MaterialIndex;

	/** Customisable type */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Bodypart")
		TSubclassOf<UTGOR_Customisation> Type;
};


/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_Bodypart : public UTGOR_Content
{
	GENERATED_BODY()

public:
	UTGOR_Bodypart();
	virtual void PostBuildResource() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Bodypart materials */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Bodypart")
		TMap<uint8, UMaterialInstance*> MaterialOverride;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	
	/** Bodypart mesh */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Bodypart")
		USkeletalMesh* Mesh;

	/** Whether bodypart follows the master mesh's pose */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Bodypart")
		bool FollowsMasterPose;

	/** Override a mesh component with bodypart properties */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void OverrideMesh(USkeletalMeshComponent* Component);

	/** Attach mesh component with a master mesh */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void AttachMesh(USkeletalMeshComponent* Component, USkeletalMeshComponent* MasterComponent);

	/** Reset a mesh before removal */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void ResetMesh(USkeletalMeshComponent* Component);
	
	/** Bodypart types that can be loaded */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Bodypart", Meta = (Categories = "BodypartType"))
		TArray<FTGOR_BodypartChild> Children;

	/** Tags that represent colour choices in the shader of the mesh (TypeOfCustomisation/Category, for example Gradient/Accessories/Horn) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Bodypart")
		TArray<FTGOR_BodypartCustomisationMapping> Customisation;

	////////////////////////////////////////////////////////////////////////////////////////////////////

private:
};
