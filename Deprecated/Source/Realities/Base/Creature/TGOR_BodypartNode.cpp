// The Gateway of Realities: Planes of Existence.


#include "TGOR_BodypartNode.h"

#include "Realities/Components/Creature/TGOR_ModularSkeletalMeshComponent.h"
#include "Realities/Base/TGOR_Singleton.h"
#include "Realities/Base/Content/TGOR_ContentManager.h"
#include "Realities/Mod/Customisations/TGOR_Customisation.h"
#include "Realities/Mod/Bodyparts/TGOR_Bodypart.h"
#include "Realities/Mod/Colours/TGOR_Colour.h"
#include "Realities/Mod/Skins/TGOR_Skin.h"

#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SkeletalMeshComponent.h"

UTGOR_BodypartNode::UTGOR_BodypartNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	Parent(nullptr),
	Bodypart(nullptr)
{
	Mesh.Mesh = nullptr;
}

UTGOR_Bodypart* UTGOR_BodypartNode::GetBodypart() const
{
	return Bodypart; 
}

UTGOR_BodypartNode* UTGOR_BodypartNode::GetParent() const 
{ 
	return Parent; 
}

UTGOR_ModularSkeletalMeshComponent* UTGOR_BodypartNode::GetMaster() const
{ 
	return CurrentMaster; 
}

const FTGOR_BodypartInstance& UTGOR_BodypartNode::GetInstance() const 
{ 
	return BodypartInstance; 
}

const TArray<UTGOR_BodypartNode*>& UTGOR_BodypartNode::GetChildren() const 
{ 
	return Children; 
}

void UTGOR_BodypartNode::RemoveChild(UTGOR_BodypartNode* Node)
{
	Node->DestroyMeshes();
	Children.Remove(Node);
}

FString UTGOR_BodypartNode::GetIdentification() const
{
	// Suffix for all outputs
	FString Suffix = FString(" at depth ") + FString::FromInt(BodypartInstance.Depth);

	// Take bodypart information if available
	if (IsValid(Bodypart))
	{
		return(Bodypart->GetDefaultName() + Suffix);
	}
	else
	{
		return(FString("Undefined bodypart") + Suffix);
	}
}

FString UTGOR_BodypartNode::PrintTree() const
{
	FString Out = "";

	for (int i = 0; i < BodypartInstance.Depth; i++)
	{
		Out += ".-";
	}

	Out += "| ";
	Out += GetIdentification();
	Out += "\n ";

	// Print recursively
	for (UTGOR_BodypartNode* Child : Children)
	{
		Out += Child->PrintTree();
	}

	return(Out);
}


void UTGOR_BodypartNode::AttachAndInitialise(bool OverrideMaster)
{
	if (!IsValid(CurrentMaster))
	{
		ERROR(GetIdentification() + ": master not provided", Error)
	}

	if (OverrideMaster)
	{
		Mesh.Mesh = CurrentMaster;
	}
	else
	{
		// Create new component if not yet present
		if (!IsValid(Mesh.Mesh))
		{
			CreateMesh();
		}
	}
	
			
	// Build meshes
	if (IsValid(Bodypart))
	{
		Bodypart->OverrideMesh(Mesh.Mesh);

		// Build and apply materials
		BuildCustomisationInstances(Mesh);
		ApplyInternalCustomisation();
	}
	else
	{
		ERROR(GetIdentification() + ": Bodypart not provided", Error)
	}
}

void UTGOR_BodypartNode::CreateMesh()
{
	if (!IsValid(CurrentMaster))
	{
		ERROR(GetIdentification() + ": No master defined", Error)
	}

	// Create meshcomponent and attach to parent
	FTGOR_BodypartMeshInstance MeshInstance = FTGOR_BodypartMeshInstance();
	MeshInstance.Mesh = NewObject<USkeletalMeshComponent>(this);

	MeshInstance.Mesh->SetCollisionProfileName("NoCollision");
	MeshInstance.Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshInstance.Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	MeshInstance.Mesh->SetGenerateOverlapEvents(false);

	MeshInstance.Mesh->SetRelativeLocation(FVector::ZeroVector);
	MeshInstance.Mesh->SetRelativeRotation(FQuat::Identity);
	MeshInstance.Mesh->SetRelativeScale3D(FVector::OneVector);

	MeshInstance.Mesh->SetIsReplicated(false);
	MeshInstance.Mesh->RegisterComponent();

	MeshInstance.Mesh->SetRenderCustomDepth(CurrentMaster->bRenderCustomDepth);
	MeshInstance.Mesh->SetCullDistance(CurrentMaster->CachedMaxDrawDistance);
	
	// Set the mesh pose
	if (IsValid(Bodypart) && Bodypart->FollowsMasterPose)
	{
		Bodypart->AttachMesh(MeshInstance.Mesh, CurrentMaster);
	}

	if (IsValid(Mesh.Mesh))
	{
		Mesh.Mesh->DestroyComponent();
	}
	Mesh = MeshInstance;
}


void UTGOR_BodypartNode::ClearMesh()
{
	if (!IsValid(Mesh.Mesh))
	{
		ERROR(GetIdentification() + ": Main mesh not defined", Error)
	}

	// Clear materials
	Mesh.CustomisationInstances.Empty();
	Mesh.Mesh->EmptyOverrideMaterials();

	// Clear mesh
	if (IsValid(Bodypart))
	{
		Bodypart->ResetMesh(Mesh.Mesh);
	}
}


void UTGOR_BodypartNode::BuildCustomisationInstances(FTGOR_BodypartMeshInstance& Instance)
{
	SINGLETON_CHK
	if (!IsValid(Bodypart)) return;

	// Cache Material overrides
	int32 Num = Instance.Mesh->GetNumMaterials();
	TMap<int32, UMaterialInstanceDynamic*> Map;
	for (const FTGOR_BodypartCustomisationMapping& Customisation : Bodypart->Customisation)
	{
		UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
		UTGOR_Customisation* Content = ContentManager->GetTFromType<UTGOR_Customisation>(Customisation.Type);
		if (IsValid(Content))
		{
			// Add materialinstances
			FTGOR_BodypartCustomisationInstances& Instances = Instance.CustomisationInstances.FindOrAdd(Content);
			Instances.Param = Customisation.Param;
			Instances.Content = Content;

			// Check if pair applies
			if (0 <= Customisation.MaterialIndex && Customisation.MaterialIndex < Num)
			{
				// Get from cache or create material instance
				UMaterialInstanceDynamic* MaterialInstance;
				UMaterialInstanceDynamic** Cache = Map.Find(Customisation.MaterialIndex);
				if (Cache != nullptr)
				{
					MaterialInstance = *Cache;
				}
				else
				{
					// Create instance and add to cache
					MaterialInstance = Instance.Mesh->CreateDynamicMaterialInstance(Customisation.MaterialIndex);

					// Make sure instance got created
					if (IsValid(MaterialInstance))
					{
						Map.Add(Customisation.MaterialIndex, MaterialInstance);
					}
					else
					{
						continue;
					}
				}

				Instances.MaterialInstances.AddUnique(MaterialInstance);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_BodypartNode::ResetCustomisationInstance(uint8 Index)
{
	// Remove entry from material customisation
	int32 Num = BodypartInstance.CustomisationInstances.Num();
	for (int i = Num - 1; i >= 0; i--)
	{
		FTGOR_CustomisationInstance& CustomisationInstance = BodypartInstance.CustomisationInstances[i];

		if (CustomisationInstance.Index == Index)
		{
			BodypartInstance.CustomisationInstances.RemoveAt(i);
		}
	}

	// Reset mesh (update is not enough)
	if (IsValid(Bodypart))
	{
		ChangeBodypart(Bodypart);
	}
}

bool UTGOR_BodypartNode::QueryCustomisationInstance(FTGOR_CustomisationInstance& Instance) const
{
	if (IsValid(Bodypart) && Singleton)
	{
		if (Bodypart->Customisation.IsValidIndex(Instance.Index))
		{
			// Get instance information from current material state
			const FTGOR_BodypartCustomisationMapping& Mapping = Bodypart->Customisation[Instance.Index];

			// Get instance information from parent
			bool MappingFound = false;
			FTGOR_CustomisationInstance CustomisationInstance;
			if (GetCustomisationMatching(CustomisationInstance, Mapping.Param, Mapping.Type))
			{
				Instance = CustomisationInstance;
				MappingFound = true;
				return true;
			}

			// Load content class if necessary
			if (!IsValid(Instance.Content))
			{
				UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
				Instance.Content = ContentManager->GetTFromType<UTGOR_Customisation>(Mapping.Type);
				if (!Instance.Content)
				{
					ERRET(FString("Couldn't find customisation ") + Mapping.Type->GetName(), Error, false)
				}
			}

			if (!MappingFound)
			{
				// Call func on given instance
				const FTGOR_BodypartCustomisationInstances* Instances = Mesh.CustomisationInstances.Find(Instance.Content);
				if (Instances != nullptr)
				{
					UMaterialInstanceDynamic* MaterialInstance = Instances->MaterialInstances[Mapping.MaterialIndex];
					Instance.Content->Query(CurrentMaster, Instance.Payload, Mapping.Param, Bodypart, Mesh.Mesh, MaterialInstance);
					return true;
				}
			}
		}
	}
	return false;
}

void UTGOR_BodypartNode::ApplyCustomisationInstance(const FTGOR_CustomisationInstance& Instance)
{

	// Update function for self and all children
	auto UpdateChildren = [](UTGOR_BodypartNode* Parent, UTGOR_BodypartNode* Node)
	{
		Node->ApplyInternalCustomisation();
	};

	// Check all current instances for matches to update
	for (FTGOR_CustomisationInstance& CustomisationInstance : BodypartInstance.CustomisationInstances)
	{
		if (CustomisationInstance.Index == Instance.Index)
		{
			CustomisationInstance = Instance;
			WalkTree(UpdateChildren);
			return;
		}
	}

	// Add new instance if no match
	BodypartInstance.CustomisationInstances.Add(Instance);
	WalkTree(UpdateChildren);
}


void UTGOR_BodypartNode::ApplyInternalCustomisation()
{
	if (IsValid(Bodypart))
	{
		for (const FTGOR_BodypartCustomisationMapping& Customisation : Bodypart->Customisation)
		{
			if (*Customisation.Type)
			{
				// Get instance information
				FTGOR_CustomisationInstance CustomisationInstance;
				if (GetCustomisationMatching(CustomisationInstance, Customisation.Param, Customisation.Type))
				{
					// Load content class if necessary
					if (!IsValid(CustomisationInstance.Content))
					{
						SINGLETON_CHK;
						UTGOR_ContentManager* ContentManager = Singleton->GetContentManager();
						CustomisationInstance.Content = ContentManager->GetTFromType<UTGOR_Customisation>(Customisation.Type);
						if (!CustomisationInstance.Content)
						{
							ERROR(FString("Couldn't find customisation ") + Customisation.Type->GetName(), Error)
						}
					}

					// Find instance
					FTGOR_BodypartCustomisationInstances* Instances = Mesh.CustomisationInstances.Find(CustomisationInstance.Content);

					// Call func on given instance
					if (Instances != nullptr)
					{
						UMaterialInstanceDynamic* MaterialInstance = Instances->MaterialInstances[Customisation.MaterialIndex];
						CustomisationInstance.Content->Apply(CurrentMaster, CustomisationInstance.Payload, Customisation.Param, Bodypart, Mesh.Mesh, MaterialInstance);
					}
				}
			}
		}
	}
}

void UTGOR_BodypartNode::ApplyToLocalInstances(FCustomisationInstanceDelegate Delegate, const FName& Param, UTGOR_Customisation* Content)
{
	if (Delegate.IsBound())
	{
		ApplyToLocalInstances(
			[&Delegate](UTGOR_BodypartNode* Node, UMaterialInstanceDynamic* MaterialInstance, FName Param)
			{
				Delegate.Execute(Node, MaterialInstance, Param);
			}, Param, Content);
	}
}

void UTGOR_BodypartNode::ApplyToLocalInstances(CustomisationInstanceLambda Func, const FName& Param, UTGOR_Customisation* Content)
{
	// Find instance
	FTGOR_BodypartCustomisationInstances* Instances = Mesh.CustomisationInstances.Find(Content);

	// Call func on all instances
	if (Instances != nullptr)
	{
		for (UMaterialInstanceDynamic* MaterialInstance : Instances->MaterialInstances)
		{
			Func(this, MaterialInstance, Instances->Param);
		}
	}
}


void UTGOR_BodypartNode::ApplyToAllInstances(FCustomisationInstanceDelegate Delegate, const FName& Param, UTGOR_Customisation* Content)
{
	if (Delegate.IsBound())
	{
		ApplyToAllInstances(
			[&Delegate](UTGOR_BodypartNode* Node, UMaterialInstanceDynamic* MaterialInstance, FName Param)
		{
			Delegate.Execute(Node, MaterialInstance, Param);
		}, Param, Content);
	}
}

void UTGOR_BodypartNode::ApplyToAllInstances(CustomisationInstanceLambda Func, const FName& Param, UTGOR_Customisation* Content)
{
	ApplyToLocalInstances(Func, Param, Content);
	
	// Apply to all children
	for (UTGOR_BodypartNode* Child : Children)
	{
		Child->ApplyToAllInstances(Func, Param, Content);
	}
}



bool UTGOR_BodypartNode::GetCustomisationMatching(FTGOR_CustomisationInstance& CustomisationInstance, const FName& Param, TSubclassOf<UTGOR_Customisation> Type) const
{
	if (IsValid(Bodypart))
	{
		// Check all selected entries for matches
		for (const FTGOR_CustomisationInstance& Instance : BodypartInstance.CustomisationInstances)
		{
			if (Bodypart->Customisation.IsValidIndex(Instance.Index))
			{
				const FTGOR_BodypartCustomisationMapping& Mapping = Bodypart->Customisation[Instance.Index];
				if (Mapping.Type == Type && Param.IsEqual(Mapping.Param, ENameCase::IgnoreCase, false))
				{
					CustomisationInstance = Instance;
					return true;
				}
			}
		}
	}

	// Recursively ask parent node
	if (IsValid(Parent))
	{
		return(Parent->GetCustomisationMatching(CustomisationInstance, Param, Type));
	}

	// Return false if none was found
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_BodypartNode::WalkTree(BodypartLambda Func)
{
	Func(Parent, this);

	// Iterate children
	int32 Num = Children.Num();
	for (int i = 0; i < Num; i++)
	{
		UTGOR_BodypartNode* Node = Children[i];

		// Recursively walk tree
		Node->WalkTree(Func);
	}
}

void UTGOR_BodypartNode::WalkTree(FBodypartDelegate Delegate)
{
	if (Delegate.IsBound())
	{
		WalkTree([Delegate](UTGOR_BodypartNode* Parent, UTGOR_BodypartNode* Node) {
			Delegate.Execute(Parent, Node);
		});
	}
}

void UTGOR_BodypartNode::ChangeBodypart(UTGOR_Bodypart* Content)
{
	if (!IsValid(CurrentMaster))
	{
		ERROR(GetIdentification() + ": No master defined", Error)
	}
	
	if (!IsValid(Content))
	{
		ERROR(GetIdentification() + ": Invalid bodypart index", Error)
	}

	BodypartInstance.Content = Content;

	// Reconstruct the tree from this point
	TArray<FTGOR_BodypartInstance> Path;
	ConstructPath(Path);
	Bodypart = nullptr;
	ConstructTree(Path, CurrentMaster, 0);
}

void UTGOR_BodypartNode::EnsureAutomatic(TSubclassOf<UTGOR_Bodypart> Class)
{
	// Ensure for all children first to not call it twice on new bodyparts
	for (UTGOR_BodypartNode* Child : Children)
	{
		Child->EnsureAutomatic(Class);
	}

	// Add automatic bodypart
	const FTGOR_BodypartChild Requirement = GetRequirement(Class);
	if (Requirement.Type == ETGOR_BodypartChildType::Automatic)
	{
		// TODO: Could check for collisions here by simply looking for "Requirement.Class", but this could lead to bodyparts not being loaded on collision.
		if (!ContainsType(Class))
		{
			AddChildType(Class);
		}
	}
}

void UTGOR_BodypartNode::PurgeAutomatic(TSubclassOf<UTGOR_Bodypart> Class)
{
	// Remove automatic bodypart
	const FTGOR_BodypartChild Requirement = GetRequirement(Class);
	if (Requirement.Type == ETGOR_BodypartChildType::Automatic)
	{
		UTGOR_BodypartNode* Node = GetChildFromType(Class);
		if (IsValid(Node))
		{
			Node->DestroyMeshes();
			Children.Remove(Node);
		}
	}

	// Purge for all children afterwards to not purge branches that are going to be deleted anyway
	for (UTGOR_BodypartNode* Child : Children)
	{
		Child->PurgeAutomatic(Class);
	}

}

void UTGOR_BodypartNode::AddChildType(TSubclassOf<UTGOR_Bodypart> Class)
{
	if (IsValid(Bodypart))
	{
		// Add first possible bodypart of the right type
		const TArray<UTGOR_Bodypart*>& PossibleBodyparts = Bodypart->GetIListFromType<UTGOR_Bodypart>(Class);
		for (UTGOR_Bodypart* Content : PossibleBodyparts)
		{
			if (Content->IsA(Class))
			{
				AddChildBodypart(Content);
			}
		}
	}
}

void UTGOR_BodypartNode::AddChildBodypart(UTGOR_Bodypart* Content)
{
	if (BodypartInstance.Depth >= MAX_BODYPART_DEPTH)
	{
		ERROR(GetIdentification() + ": Bodypart tree too big", Error);
	}

	// Create subpath
	FTGOR_BodypartInstance Instance;

	Instance.Depth = BodypartInstance.Depth + 1;
	Instance.Content = Content;

	TArray<FTGOR_BodypartInstance> Appendix;
	Appendix.Add(Instance);

	// Create new node
	UTGOR_BodypartNode* Node = NewObject<UTGOR_BodypartNode>(this);

	// Add new node to tree structure
	Children.Add(Node);
	Node->Parent = this;

	// Recursively construct subtree
	Node->ConstructTree(Appendix, CurrentMaster, 0);
}

int32 UTGOR_BodypartNode::ConstructTree(const TArray<FTGOR_BodypartInstance>& Path, UTGOR_ModularSkeletalMeshComponent* Master, int32 Index)
{
	if (!IsValid(Master))
	{
		ERRET(GetIdentification() + ": Master not provided", Error, 0);
	}
	
	if (Index >= MAX_BODYPART_DEPTH)
	{
		ERRET(GetIdentification() + ": Too many bodyparts", Error, 0);
	}

	CurrentMaster = Master;
	AssignWorld(Master->GetWorld());
	SINGLETON_RETCHK(0);

	// Check index to prevent empty nodes
	int32 Num = Path.Num();
	if (Index >= Num)
	{
		ERRET(GetIdentification() + ": Reached the end of the path too early", Error, 0);
	}

	// Only rebuild if bodypart changed
	const FTGOR_BodypartInstance& Instance = Path[Index];
	if (Bodypart != Instance.Content)
	{
		// Clear children
		Children.Empty();
		DestroyMeshes();

		// Initialise own node
		BuildNode(Instance, Instance.Depth);
	}

	// Pool to check which children weren't loaded back
	TSet<UTGOR_BodypartNode*> Unused(Children);

	// Look for children and keep track of how many got added
	int32 Count = 1;
	while (Index + Count < Num)
	{
		const FTGOR_BodypartInstance& Next = Path[Index + Count];

		// Break once end of branch reached
		if (Next.Depth < BodypartInstance.Depth + 1)
		{
			break;
		}

		// Skip children (in case of invalid entry)
		if (Next.Depth > BodypartInstance.Depth + 1)
		{
			Count++;
			continue;
		}

		// Check whether child is allowed to be added
		if (CheckChildValidity(Next.Content, Unused, Index + Count))
		{
			// Look for already fitting child node
			UTGOR_BodypartNode* Node = GetChild(Next.Content);
			
			// Create new node if not found
			if (Node == nullptr)
			{
				Node = NewObject<UTGOR_BodypartNode>(this);

				// Add new node to tree structure
				Node->Parent = this;
				Children.Add(Node);
			}
			else
			{
				// Remove from unused pool
				Unused.Remove(Node);
			}

			// Recursively construct subtree
			int32 Constructed = Node->ConstructTree(Path, Master, Index + Count);
			Count += Constructed;

			// Make sure counter doesn't get stuck
			if (Constructed == 0)
			{
				break;
			}
		}
		else
		{
			// Just ignore this entry and go to next
			Count++;
		}
	}

	// Add nodes that are required
	if (IsValid(Bodypart))
	{
		for (const FTGOR_BodypartChild& Requirement : Bodypart->Children)
		{
			if (Requirement.Type == ETGOR_BodypartChildType::Required)
			{
				// Load required classes if not exists yet
				if (!ContainsType(Requirement.Class))
				{
					AddChildType(Requirement.Class);
				}
			}
			else if (Requirement.Type == ETGOR_BodypartChildType::Automatic)
			{
				// Check what kind of bodypart is expected to be loaded automatically
				TSubclassOf<UTGOR_Bodypart> Handle = Master->GetHandleOfType(Requirement.Class);
				if (*Handle)
				{
					UTGOR_BodypartNode* Node = GetChildFromType(Handle);
					if (IsValid(Node))
					{
						// Don't clean up children that are automatically required
						Unused.Remove(Node);
					}
					else
					{
						// Load handle
						AddChildType(Handle);
					}
				}
			}
		}
	}

	// Clean up unused nodes
	for (auto It = Children.CreateIterator(); It; It++)
	{
		if (Unused.Contains(*It))
		{
			(*It)->DestroyMeshes();
			It.RemoveCurrent();
		}
	}

	return(Count);
}

void UTGOR_BodypartNode::ConstructPath(TArray<FTGOR_BodypartInstance>& Path)
{
	if (Path.Num() >= MAX_BODYPART_DEPTH)
	{
		ERROR(GetIdentification() + ": Too many bodyparts", Error)
	}

	Path.Add(BodypartInstance);

	// Add children to path that aren't automatically changed
	int32 Num = Children.Num();
	for (int i = 0; i < Num; i++)
	{
		UTGOR_BodypartNode* Node = Children[i];
		if (IsValid(Node) && IsValid(Node->Bodypart))
		{
			const FTGOR_BodypartChild ChildType = GetRequirement(Node->Bodypart->GetClass());
			if (ChildType.Type != ETGOR_BodypartChildType::Automatic)
			{
				// Recursively add children
				Node->ConstructPath(Path);
			}
		}
	}
}

bool UTGOR_BodypartNode::CheckChildValidity(UTGOR_Bodypart* ChildBodypart, const TSet<UTGOR_BodypartNode*>& Unused, uint8 Depth) const
{
	// Setup node
	if (IsValid(ChildBodypart))
	{
		// Check if selection is allowed
		if (IsValid(Bodypart))
		{
			if (!Bodypart->ContainsI<UTGOR_Bodypart>(ChildBodypart))
			{
				ERRET(ChildBodypart->GetDefaultName() + " Bodypart not allowed in " + Bodypart->GetDefaultName(), Warning, false);
			}

			if (!CurrentMaster->Execute_SupportsContent(CurrentMaster, ChildBodypart))
			{
				ERRET(ChildBodypart->GetDefaultName() + " Bodypart not unlocked for " + CurrentMaster->GetName(), Warning, false);
			}

			FTGOR_BodypartChild ChildType = GetRequirement(ChildBodypart->GetClass());
			if (ChildType.Type == ETGOR_BodypartChildType::Automatic)
			{
				ERRET(ChildBodypart->GetDefaultName() + " is allocated for " + ChildType.Class->GetName() + " which is automatically set in " + Bodypart->GetDefaultName() + " and is not customisable", Warning, false);
			}

			UTGOR_BodypartNode* Node = GetChildFromType(ChildBodypart->GetClass());
			if (IsValid(Node) && !Unused.Contains(Node))
			{
				ERRET(ChildBodypart->GetDefaultName() + " colliding with other " + ChildType.Class->GetName() + " type already present in " + Bodypart->GetDefaultName(), Warning, false);
			}
		}
		else if (Depth > 0)
		{
			ERRET(GetIdentification() + ": Parent not valid while building " + ChildBodypart->GetDefaultName(), Warning, false);
		}
	}
	else
	{
		ERRET(GetIdentification() + ": Bodypart not valid while building", Warning, false);
	}

	return true;
}

void UTGOR_BodypartNode::BuildNode(const FTGOR_BodypartInstance& Instance, uint8 Depth)
{
	SINGLETON_CHK;

	BodypartInstance = Instance;
	BodypartInstance.Depth = Depth;

	// Set the bodypart
	Bodypart = BodypartInstance.Content;

	AttachAndInitialise((Parent == nullptr));

	// Validate all customisations and remove them if necessary
	for (auto Its = BodypartInstance.CustomisationInstances.CreateIterator(); Its; Its++)
	{
		// Remove either if no instance exists or customisation is invalid
		FTGOR_BodypartCustomisationInstances* Instances = Mesh.CustomisationInstances.Find(Its->Content);
		if (!Instances || !Its->Content->Validate(CurrentMaster, Its->Payload, Instances->Param, Bodypart))
		{
			Its.RemoveCurrent();
			RPORT(Its->Content->GetDefaultName() + " is not allowed for this mesh or bodypart.");
		}
	}
}

void UTGOR_BodypartNode::DestroyMeshes()
{
	if (!IsValid(CurrentMaster))
	{
		ERROR(GetIdentification() + ": No master defined", Error);
	}

	if (Mesh.Mesh == CurrentMaster)
	{
		ClearMesh();
	}
	else
	{
		if (IsValid(Mesh.Mesh))
		{
			Mesh.Mesh->DestroyComponent();
		}
		Mesh = FTGOR_BodypartMeshInstance();
	}
	
	// Rcursive call to all children
	for (UTGOR_BodypartNode* Child : Children)
	{
		Child->DestroyMeshes();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_BodypartNode* UTGOR_BodypartNode::GetChild(UTGOR_Bodypart* Content) const
{
	for (UTGOR_BodypartNode* Child : Children)
	{
		if (Child->Bodypart == Content)
		{
			return Child;
		}
	}
	return nullptr;
}

UTGOR_BodypartNode* UTGOR_BodypartNode::GetChildFromType(TSubclassOf<UTGOR_Bodypart> Class) const
{
	if (*Class)
	{
		for (UTGOR_BodypartNode* Child : Children)
		{
			if (IsValid(Child->Bodypart) && Child->Bodypart->IsA(Class))
			{
				return Child;
			}
		}
	}
	return nullptr;
}

FTGOR_BodypartChild UTGOR_BodypartNode::GetRequirement(TSubclassOf<UTGOR_Bodypart> Class) const
{
	// Check for matching requirement class
	for (const FTGOR_BodypartChild& Requirement : Bodypart->Children)
	{
		if (*Requirement.Class && Class->IsChildOf(Requirement.Class))
		{
			return Requirement;
		}
	}
	return FTGOR_BodypartChild();
}

bool UTGOR_BodypartNode::ContainsType(TSubclassOf<UTGOR_Bodypart> Class) const
{
	return IsValid(GetChildFromType(Class));
}