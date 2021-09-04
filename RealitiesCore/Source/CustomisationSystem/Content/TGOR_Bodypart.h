// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Materials/MaterialInstance.h"

#include "CustomisationSystem/TGOR_BodypartInstance.h"
#include "CustomisationSystem/Content/Customisations/TGOR_Customisation.h"

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_Bodypart.generated.h"

class UTGOR_Skin;
class UTGOR_Mask;
class UTGOR_Canvas;
class UTGOR_Palette;
class UTGOR_Section;
class UTGOR_Archetype;
class UTGOR_MaterialCustomisation;
class UTGOR_MeshMorphCustomisation;

class UTGOR_ChildSkeletalMeshComponent;
class UTGOR_ModularSkeletalMeshComponent;

////////////////////////////////////////////// STRUCT //////////////////////////////////////////////////////


/**
 * 
 */
UCLASS()
class CUSTOMISATIONSYSTEM_API UTGOR_Bodypart : public UTGOR_CoreContent
{
	GENERATED_BODY()

public:
	UTGOR_Bodypart();
	virtual void PostBuildResource() override;
	virtual bool Validate_Implementation() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Bodypart materials */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		TMap<uint8, UMaterialInstance*> MaterialOverride;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Bodypart mesh */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		USkeletalMesh* Mesh;

	/** Whether bodypart follows the master mesh's pose */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		bool FollowsMasterPose;

	/** Whether this bodypart can be spawned as root bodypart */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		bool IsBodypartRoot;
		
	/** Bodypart types that can be loaded */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Bodypart", Meta = (Categories = "BodypartType"))
		TArray<FTGOR_BodypartChild> Children;

	/** Texture overrides to use instead of layered RT (gets rid of all customisation) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Bodypart")
		TMap<TSubclassOf<UTGOR_Canvas>, UTexture*> CanvasOverride;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Available geometries mapped to the mesh Segment index */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TMap<TSubclassOf<UTGOR_Section>, int32> SectionInsertions;
	DECL_INSERTION_REQUIRED(SectionInsertions);

	/** Skin insertions */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Skin>> SkinInsertions;
	DECL_INSERTION(SkinInsertions);

	/** Mask insertions */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Mask>> MaskInsertions;
	DECL_INSERTION(MaskInsertions);

	/** Palette insertions */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TSubclassOf<UTGOR_Palette> DefaultPaletteInsertion;
	DECL_INSERTION(DefaultPaletteInsertion);

	/** Customisation insertions */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Insertion")
		TArray<TSubclassOf<UTGOR_Customisation>> CustomisationInsertions;
	DECL_INSERTION(CustomisationInsertions);

	virtual void MoveInsertion(UTGOR_Content* Insertion, ETGOR_InsertionActionEnumeration Action, bool& Success) override;


#if WITH_EDITOR

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
};
