// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "CustomisationSystem/Components/TGOR_ControlSkeletalMeshComponent.h"
#include "DimensionSystem/Interfaces/TGOR_SpawnerInterface.h"
#include "TGOR_CustomisationComponent.generated.h"

class UTGOR_Skeleton;
class UTGOR_Section;
class UTGOR_Modular;

DECLARE_STATS_GROUP(TEXT("TGOR Customisation System"), STATGROUP_TGOR_CUST, STATCAT_Advanced);

////////////////////////////////////////////// ENUM ////////////////////////////////////////////////////

UENUM(BlueprintType)
enum class ETGOR_CustomisationQueryEnumeration : uint8
{
	Local,
	All,
	Inherited
};

////////////////////////////////////////////// MACROS //////////////////////////////////////////////////

#define MAX_BODYPART_DEPTH 0x10

///////////////////////////////////////////// DELEGATES ////////////////////////////////////////////////

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAppearanceUpdateDelegate, const FTGOR_BodypartMergeOutput&, MergeOutput);


/**
 * UTGOR_CustomisationComponent represents character customisation
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class CUSTOMISATIONSYSTEM_API UTGOR_CustomisationComponent : public UTGOR_ControlSkeletalMeshComponent, public ITGOR_SpawnerInterface
{
	GENERATED_BODY()

public:
	UTGOR_CustomisationComponent();
	virtual void UpdateContent_Implementation(UTGOR_Spawner* Spawner) override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Bodypart")
		FAppearanceUpdateDelegate OnAppearanceChanged;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Modular type this customisation spawns with. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Movement")
		TSubclassOf<UTGOR_Modular> TargetModular;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Bodypart")
		int32 MergedTextureLimit;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "!TGOR Bodypart")
		UTGOR_Modular* Modular;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "!TGOR Bodypart")
		FTGOR_AppearanceInstance CurrentAppearance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "!TGOR Bodypart")
		TArray<FTGOR_CustomisationMeshEntry> CustomisationEntries;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "!TGOR Bodypart")
		TMap<UTGOR_Section*, UMaterialInstanceDynamic*> CustomisationMaterials;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "!TGOR Bodypart")
		TMap<UTGOR_Canvas*, FTGOR_CustomisationMeshTexture> CustomisationTextures;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Previews an appearance to this component without actually setting the local state (create asset only supported in editor) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Creature", Meta = (Keywords = "C++"))
		void ApplyAndCreateMerge(const FTGOR_AppearanceInstance& Instance);

	/** Previews an appearance to this component without actually setting the local state (create asset only supported in editor) */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Creature", Meta = (Keywords = "C++"))
		FTGOR_BodypartMergeOutput MergeBodyparts();

	/** Exports a mesh to file */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Creature", Meta = (Keywords = "C++"))
		void RuntimeExportMesh(const FTGOR_BodypartMergeOutput& Output, const FString& Filename);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Return how many instances of a given type there are */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		int32 CountCustomisationInstances(int32 NodeIndex, UTGOR_Customisation* Content) const;

	/** Reset a customsiation */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void ResetCustomisationInstance(int32 NodeIndex, UTGOR_Customisation* Content, int32 Index);

	/** Get customisation defaults from instance data (fills payload) if available */
	UFUNCTION(BlueprintPure, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		bool QueryCustomisationInstance(int32 NodeIndex, FTGOR_CustomisationInstance& Instance) const;

	/** Sets a customsiation, local only */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void ApplyCustomisationInstance(int32 NodeIndex, const FTGOR_CustomisationInstance& Instance, int32 Index);

	/** Applies internal customisations to the main mesh */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		void ApplyInternalCustomisation();

	/** Finds MaterialCustomisation matching a specificed tag */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		bool GetCustomisationMatching(int32 NodeIndex, TArray<FTGOR_CustomisationInstance>& CustomisationInstances, UTGOR_Customisation* Content, ETGOR_CustomisationQueryEnumeration Query) const;

	/** Changes bodypart content */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		void ChangeBodypart(int32 NodeIndex, UTGOR_Bodypart* Content);

	/** Remove a child node */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		void RemoveChild(int32 NodeIndex, int32 ChildIndex);

	/** Adds a bodypart to this node */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		int32 AddChildBodypart(int32 NodeIndex, UTGOR_Bodypart* Content);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Returns identification for an index */
	UFUNCTION(BlueprintPure, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		FString GetIdentification(int32 NodeIndex) const;

	/** Constructs subtree from this node using a path allowing only a certain amount of customisations. Returns amount of constructed children. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		int32 ConstructTree(int32 NodeIndex);

	/** Checks whether a child can be added to this node */
	UFUNCTION(BlueprintPure, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		bool CheckChildValidity(int32 NodeIndex, int32 ChildIndex) const;

	/** Updates the owning modular skeletal component appearance */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void UpdateAppearance(int32 NodeIndex);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get node bodypart content */
	UFUNCTION(BlueprintPure, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		UTGOR_Bodypart* GetBodypart(int32 NodeIndex) const;

	/** Returns bounds of this and all children */
	UFUNCTION(BlueprintPure, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		bool GetBounds(int32 NodeIndex, bool IncludeChildren, FVector& Center, float& Extend) const;

	/** Get parent node of a given bodypart */
	UFUNCTION(BlueprintPure, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		int32 GetParent(int32 NodeIndex) const;

	/** Get child node of a given bodypart */
	UFUNCTION(BlueprintPure, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		int32 GetChild(int32 NodeIndex, UTGOR_Bodypart* Content) const;

	/** Get child node of a given type */
	UFUNCTION(BlueprintPure, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		int32 GetFirstChildFromType(int32 NodeIndex, TSubclassOf<UTGOR_Bodypart> Class) const;

	/** Get child type for a given bodypart class */
	UFUNCTION(BlueprintPure, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		FTGOR_BodypartChild GetRequirement(int32 NodeIndex, TSubclassOf<UTGOR_Bodypart> Class) const;
};
