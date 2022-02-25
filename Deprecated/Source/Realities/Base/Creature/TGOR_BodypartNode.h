// The Gateway of Realities: Planes of Existence.

#pragma once

#include <functional>

#include "Realities/Base/Instances/Creature/TGOR_BodypartInstance.h"
#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Templates/SubclassOf.h"

#include "Realities/Base/TGOR_Object.h"
#include "TGOR_BodypartNode.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

class UTGOR_ModularSkeletalMeshComponent;
class UTGOR_ContentManager;
class UTGOR_BodypartNode;

class UTGOR_Bodypart;
class UTGOR_Skin;

////////////////////////////////////////////// MACROS //////////////////////////////////////////////////////

#define MAX_BODYPART_DEPTH 0x10

using BodypartLambda = std::function<void(UTGOR_BodypartNode*, UTGOR_BodypartNode*)>;
DECLARE_DYNAMIC_DELEGATE_TwoParams(FBodypartDelegate, UTGOR_BodypartNode*, Parent, UTGOR_BodypartNode*, Node);

using CustomisationInstanceLambda = std::function<void(UTGOR_BodypartNode*, UMaterialInstanceDynamic*, const FName&)>;
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FCustomisationInstanceDelegate, UTGOR_BodypartNode*, Node, UMaterialInstanceDynamic*, MaterialInstance, const FName&, Param);

///////////////////////////////////////////// STRUCTS ///////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FTGOR_BodypartCustomisationInstances
{
	GENERATED_USTRUCT_BODY()

	/** Type of customisation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Bodypart")
		UTGOR_Customisation* Content;

	/** Parameter name */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Bodypart")
		FName Param;

	/** Meshes this applies to */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Bodypart")
		TArray<UMaterialInstanceDynamic*> MaterialInstances;
};

USTRUCT(BlueprintType)
struct FTGOR_BodypartMeshInstance
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Bodypart")
		USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Bodypart")
		TMap<UTGOR_Customisation*, FTGOR_BodypartCustomisationInstances> CustomisationInstances;
};


/**
 * UTGOR_BodypartNode defines a node inside the bodypart tree
 */
UCLASS(Blueprintable)
class REALITIES_API UTGOR_BodypartNode : public UTGOR_Object
{
	GENERATED_BODY()

public:

	UTGOR_BodypartNode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	/** */

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(EditAnywhere, Category = "TGOR Bodypart")
		TArray<UTGOR_BodypartNode*> Children;

	UPROPERTY(EditAnywhere, Category = "TGOR Bodypart")
		UTGOR_BodypartNode* Parent;

	UPROPERTY(EditAnywhere, Category = "TGOR Bodypart")
		FTGOR_BodypartInstance BodypartInstance;

	UPROPERTY(EditAnywhere, Category = "TGOR Bodypart")
		UTGOR_ModularSkeletalMeshComponent* CurrentMaster;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	UPROPERTY(EditAnywhere, Category = "TGOR Bodypart")
		UTGOR_Bodypart* Bodypart;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Bodypart")
		FTGOR_BodypartMeshInstance Mesh;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Returns Bodypart associated with this node */
	UFUNCTION(BlueprintPure, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		UTGOR_Bodypart* GetBodypart() const;

	/** Returns parent node */
	UFUNCTION(BlueprintPure, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		UTGOR_BodypartNode* GetParent() const;

	/** Returns master mesh */
	UFUNCTION(BlueprintPure, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		UTGOR_ModularSkeletalMeshComponent* GetMaster() const;

	/** Returns bodypart instance */
	UFUNCTION(BlueprintPure, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		const FTGOR_BodypartInstance& GetInstance() const;

	/** Returns child nodes */
	UFUNCTION(BlueprintPure, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		const TArray<UTGOR_BodypartNode*>& GetChildren() const;

	/** Remove a child node */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void RemoveChild(UTGOR_BodypartNode* Node);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Returns identification */
	UFUNCTION(BlueprintPure, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		FString GetIdentification() const;

	/** Prints tree recursively */
	UFUNCTION(BlueprintPure, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		FString PrintTree() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Initialises a mesh for this node */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void AttachAndInitialise(bool OverrideMaster = false);

	/** Creates a mesh for this node */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void CreateMesh();
	
	/** Clear main mesh */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void ClearMesh();
	
	/** Registers materials */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void BuildCustomisationInstances(FTGOR_BodypartMeshInstance& Instance);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Reset a customsiation */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void ResetCustomisationInstance(uint8 Index);

	/** Get customisation defaults from instance data (fills payload) */
	UFUNCTION(BlueprintPure, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		bool QueryCustomisationInstance(FTGOR_CustomisationInstance& Instance) const;

	/** Sets a customsiation */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void ApplyCustomisationInstance(const FTGOR_CustomisationInstance& Instance);


	/** Applies internal customisations to the main mesh */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void ApplyInternalCustomisation();
	
	/** Applies a delegate on matching Instances to operate on */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Categories = "MaterialCustomisation", Keywords = "C++"))
		void ApplyToLocalInstances(FCustomisationInstanceDelegate Delegate, const FName& Param, UTGOR_Customisation* Content);
		void ApplyToLocalInstances(CustomisationInstanceLambda Func, const FName& Param, UTGOR_Customisation* Content);

	/** Recursively applies a delegate on matching MaterialInstances to operate on in this subtree */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Categories = "MaterialCustomisation", Keywords = "C++"))
		void ApplyToAllInstances(FCustomisationInstanceDelegate Delegate, const FName& Param, UTGOR_Customisation* Content);
		void ApplyToAllInstances(CustomisationInstanceLambda Func, const FName& Param, UTGOR_Customisation* Content);

	/** Finds MaterialCustomisation matching a specificed tag */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		bool GetCustomisationMatching(FTGOR_CustomisationInstance& CustomisationInstance, const FName& Param, TSubclassOf<UTGOR_Customisation> Type) const;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Calls a delegate/lambda function on all children of this tree */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void WalkTree(FBodypartDelegate Delegate);
		void WalkTree(BodypartLambda Func);

	/** Changes bodypart content */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void ChangeBodypart(UTGOR_Bodypart* Content);

	/** Makes sure any automatic child of given type is added */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void EnsureAutomatic(TSubclassOf<UTGOR_Bodypart> Class);

	/** Makes sure any automatic child of given type is removed */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void PurgeAutomatic(TSubclassOf<UTGOR_Bodypart> Class);

	/** Adds a bodypart of given type to this node*/
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void AddChildType(TSubclassOf<UTGOR_Bodypart> Class);

	/** Adds a bodypart to this node */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void AddChildBodypart(UTGOR_Bodypart* Content);

	/** Constructs subtree from this node using a path. returns amount of constructed children */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		int32 ConstructTree(const TArray<FTGOR_BodypartInstance>& Path, UTGOR_ModularSkeletalMeshComponent* Master, int32 Index = 0);

	/** Constructs a path from this node using subtree */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void ConstructPath(TArray<FTGOR_BodypartInstance>& Path);

	/** Checks whether a child can be added to this node */
	UFUNCTION(BlueprintPure, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		bool CheckChildValidity(UTGOR_Bodypart* ChildBodypart, const TSet<UTGOR_BodypartNode*>& Unused, uint8 Depth) const;

	/** Constructs class attributes from instance fields and sets node depth in this tree */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void BuildNode(const FTGOR_BodypartInstance& Instance, uint8 Depth);

	/** Recursively destroys all meshcomponents */
	UFUNCTION(BlueprintCallable, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		void DestroyMeshes();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Get child node of a given bodypart */
	UFUNCTION(BlueprintPure, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		UTGOR_BodypartNode* GetChild(UTGOR_Bodypart* Content) const;

	/** Get child node of a given type */
	UFUNCTION(BlueprintPure, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		UTGOR_BodypartNode* GetChildFromType(TSubclassOf<UTGOR_Bodypart> Class) const;

	/** Get child type for a given bodypart class */
	UFUNCTION(BlueprintPure, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		FTGOR_BodypartChild GetRequirement(TSubclassOf<UTGOR_Bodypart> Class) const;

	/** Checks whether there is a certain child type present */
	UFUNCTION(BlueprintPure, Category = "TGOR Bodypart", Meta = (Keywords = "C++"))
		bool ContainsType(TSubclassOf<UTGOR_Bodypart> Class) const;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

private:

};
