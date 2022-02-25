// The Gateway of Realities: Planes of Existence.

#pragma once

#include <functional>
#include "CoreMinimal.h"

#include "Realities/CoreSystem/Storage/TGOR_Serialisation.h"
#include "Realities/CoreSystem/Storage/TGOR_PackageGroup.h"
#include "Realities/CoreSystem/Storage/TGOR_PackageNetwork.h"

#include "../TGOR_BodypartInstance.h"

#include "Components/SkeletalMeshComponent.h"
#include "Realities/CoreSystem/Interfaces/TGOR_SingletonInterface.h"
#include "TGOR_ChildSkeletalMeshComponent.generated.h"

// TODO: Net interface UAsyncTaskDownloadImage

class UTGOR_ModularSkeletalMeshComponent;
class UTGOR_Section;

DECLARE_STATS_GROUP(TEXT("TGOR Customisation System"), STATGROUP_TGOR_CUST, STATCAT_Advanced);

////////////////////////////////////////////// STRUCT //////////////////////////////////////////////////

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_ModularMeshDescriptor
{
	GENERATED_USTRUCT_BODY();
	SERIALISE_INIT_HEADER;

	FTGOR_ModularMeshDescriptor();

	CTGOR_GroupPackageCache LegacyCache;
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_BodypartInstance Instance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TWeakObjectPtr<UTGOR_ChildSkeletalMeshComponent> Parent;

};

template<>
struct TStructOpsTypeTraits<FTGOR_ModularMeshDescriptor> : public TStructOpsTypeTraitsBase2<FTGOR_ModularMeshDescriptor>
{
	enum
	{
		WithNetSerializer = true
	};
};


USTRUCT(BlueprintType)
struct FTGOR_BodypartCustomisationCache
{
	GENERATED_USTRUCT_BODY()

	/** Materials this applies to */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		UMaterialInstanceDynamic* MaterialInstance;

	/** Texture allocation for rendertargets */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Bodypart")
		FTGOR_BodypartCustomisationAllocation Allocation;
};

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

using BodypartNodeLambda = std::function<void(UTGOR_ChildSkeletalMeshComponent*, UTGOR_ChildSkeletalMeshComponent*)>;
using CustomisationNodeInstanceLambda = std::function<void(UTGOR_ChildSkeletalMeshComponent*, UMaterialInstanceDynamic*)>;

///////////////////////////////////////////// DELEGATES ////////////////////////////////////////////////

DECLARE_DYNAMIC_DELEGATE_TwoParams(FBodypartNodeDelegate, UTGOR_ChildSkeletalMeshComponent*, Parent, UTGOR_ChildSkeletalMeshComponent*, Node);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FCustomisationNodeInstanceDelegate, UTGOR_ChildSkeletalMeshComponent*, Node, UMaterialInstanceDynamic*, MaterialInstance);


/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_ChildSkeletalMeshComponent : public USkeletalMeshComponent, public ITGOR_SingletonInterface
{
	GENERATED_BODY()

		friend class UTGOR_MergeCustomisation;

public:
	UTGOR_ChildSkeletalMeshComponent();
	virtual void OnRegister() override;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Appearance information */
	UPROPERTY(ReplicatedUsing = OnReplicateDescriptor)
		FTGOR_ModularMeshDescriptor Descriptor;

	/** Child meshes */
	UPROPERTY()
		TArray<UTGOR_ChildSkeletalMeshComponent*> Children;

	/** Master mesh */
	UPROPERTY()
		TWeakObjectPtr<UTGOR_ModularSkeletalMeshComponent> Master;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Bodypart")
		TMap<UTGOR_Section*, FTGOR_BodypartCustomisationCache> CustomisationCaches;

	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		void OnReplicateDescriptor(const FTGOR_ModularMeshDescriptor& Old);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Component class to spawn children of */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Bodypart")
		TSubclassOf<UTGOR_ChildSkeletalMeshComponent> ChildComponentType;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Returns Bodypart associated with this node */
	UFUNCTION(BlueprintPure, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		UTGOR_Bodypart* GetBodypart() const;

	/** Returns parent node */
	UFUNCTION(BlueprintPure, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		UTGOR_ChildSkeletalMeshComponent* GetParent() const;

	/** Returns child nodes */
	UFUNCTION(BlueprintPure, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		const TArray<UTGOR_ChildSkeletalMeshComponent*>& GetChildren() const;

	/** Returns master mesh */
	UFUNCTION(BlueprintPure, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		UTGOR_ChildSkeletalMeshComponent* GetMaster() const;

	/** Returns bodypart instance */
	UFUNCTION(BlueprintPure, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		const FTGOR_BodypartInstance& GetInstance() const;

	/** Remove a child node */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		void RemoveChild(UTGOR_ChildSkeletalMeshComponent* Node);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Returns bounds of this and all children */
	UFUNCTION(BlueprintPure, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		bool GetBounds(bool IncludeChildren, FVector& Center, float& Extend) const;

	/** Recursively gather meshes/textures from this node and its children to construct a merge */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		void GatherMergeResources(UPARAM(ref) FTGOR_BodypartResources& Resources) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Returns identification */
	UFUNCTION(BlueprintPure, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		FString GetIdentification() const;

	/** Prints tree recursively */
	UFUNCTION(BlueprintPure, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		FString PrintTree() const;

	/** Updates the owning modular skeletal component appearance */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void UpdateAppearance();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Initialises a mesh for this node */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		virtual void AttachAndInitialise();

	/** Clear main mesh */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		void ClearMesh();

	/** Registers materials */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		void BuildCustomisationInstances();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Return how many instances of a given type there are */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		int32 CountCustomisationInstances(UTGOR_Customisation* Content) const;

	/** Reset a customsiation */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void ResetCustomisationInstance(UTGOR_Customisation* Content, int32 Index);

	/** Get customisation defaults from instance data (fills payload) if available */
	UFUNCTION(BlueprintPure, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		bool QueryCustomisationInstance(FTGOR_CustomisationInstance& Instance) const;

	/** Sets a customsiation, local only */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		void ApplyCustomisationInstance(const FTGOR_CustomisationInstance& Instance, int32 Index);


	/** Applies internal customisations to the main mesh */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		void ApplyInternalCustomisation();

	/** Applies a delegate on matching Instances to operate on */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Categories = "MaterialCustomisation", Keywords = "C++"))
		void ApplyToLocalInstances(FCustomisationNodeInstanceDelegate Delegate, UTGOR_Customisation* Content);
	void ApplyToLocalInstances(CustomisationNodeInstanceLambda Func, UTGOR_Customisation* Content);

	/** Recursively applies a delegate on matching MaterialInstances to operate on in this subtree */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Categories = "MaterialCustomisation", Keywords = "C++"))
		void ApplyToAllInstances(FCustomisationNodeInstanceDelegate Delegate, UTGOR_Customisation* Content);
	void ApplyToAllInstances(CustomisationNodeInstanceLambda Func, UTGOR_Customisation* Content);

	/** Finds MaterialCustomisation matching a specificed tag */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		bool GetCustomisationMatching(TArray<FTGOR_CustomisationInstance>& CustomisationInstances, UTGOR_Customisation* Content, ETGOR_CustomisationQueryEnumeration Query) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Calls a delegate/lambda function on all children of this tree */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		void WalkTree(FBodypartNodeDelegate Delegate);
	void WalkTree(BodypartNodeLambda Func);

	/** Changes bodypart content */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		void ChangeBodypart(UTGOR_Bodypart* Content);

	/** Makes sure any automatic child of given type is added */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		void EnsureAutomatic(TSubclassOf<UTGOR_Bodypart> Class);

	/** Makes sure any automatic child of given type is removed */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		void PurgeAutomatic(TSubclassOf<UTGOR_Bodypart> Class);

	/** Adds a bodypart of given type to this node*/
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		UTGOR_ChildSkeletalMeshComponent* AddChildType(TSubclassOf<UTGOR_Bodypart> Class);

	/** Adds a bodypart to this node */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		UTGOR_ChildSkeletalMeshComponent* AddChildBodypart(UTGOR_Bodypart* Content);

	/** Constructs subtree from this node using a path allowing only a certain amount of customisations. Returns amount of constructed children. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		int32 ConstructTree(const TArray<FTGOR_BodypartInstance>& Path, int32 Index);

	/** Constructs a path from this node using subtree */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		void ConstructPath(TArray<FTGOR_BodypartInstance>& Path) const;

	/** Checks whether a child can be added to this node */
	UFUNCTION(BlueprintPure, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		bool CheckChildValidity(UTGOR_Bodypart* ChildBodypart, const TSet<UTGOR_ChildSkeletalMeshComponent*>& Unused, uint8 Depth) const;

	/** Constructs class attributes from instance fields and sets node depth in this tree */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		void BuildNode(const FTGOR_BodypartInstance& Instance, uint8 Depth);

	/** Recursively destroys all meshcomponents */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		void DestroyMeshes();

	/** Merge all child meshes into this component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		void MergeChildren();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Get child node of a given bodypart */
	UFUNCTION(BlueprintPure, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		UTGOR_ChildSkeletalMeshComponent* GetChild(UTGOR_Bodypart* Content) const;

	/** Get child node of a given type */
	UFUNCTION(BlueprintPure, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		UTGOR_ChildSkeletalMeshComponent* GetChildFromType(TSubclassOf<UTGOR_Bodypart> Class) const;

	/** Get child type for a given bodypart class */
	UFUNCTION(BlueprintPure, Category = "!TGOR Bodypart|Internal", Meta = (Keywords = "C++"))
		FTGOR_BodypartChild GetRequirement(TSubclassOf<UTGOR_Bodypart> Class) const;

	/** Checks whether there is a certain child type present */
	UFUNCTION(BlueprintPure, Category = "!TGOR Bodypart", Meta = (Keywords = "C++"))
		bool ContainsType(TSubclassOf<UTGOR_Bodypart> Class) const;

};
