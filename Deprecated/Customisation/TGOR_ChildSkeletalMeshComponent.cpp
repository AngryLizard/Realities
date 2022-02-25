// The Gateway of Realities: Planes of Existence.


#include "TGOR_ChildSkeletalMeshComponent.h"

#include "Realities/CustomisationSystem/Components/TGOR_ModularSkeletalMeshComponent.h"

#include "Realities/CustomisationSystem/Content/TGOR_Customisation.h"
#include "Realities/CustomisationSystem/Content/TGOR_Bodypart.h"
#include "Realities/CustomisationSystem/Content/TGOR_Section.h"
#include "Realities/CustomisationSystem/Content/TGOR_Canvas.h"

#include "Realities/CustomisationSystem/UserData/TGOR_SkeletalMerge.h"
#include "Realities/CustomisationSystem/UserData/TGOR_SkeletalExport.h"

#include "Realities/CoreSystem/Gameplay/TGOR_GameInstance.h"
#include "Realities/CoreSystem/TGOR_Singleton.h"

#include "Kismet/KismetRenderingLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SkeletalMeshComponent.h"

#include "Realities/CustomisationSystem/UserData/TGOR_BodypartUserData.h"
#include "Realities/CustomisationSystem/UserData/TGOR_SkeletalMergeUserData.h"

#include "Realities/CoreSystem/Storage/TGOR_Package.h"
#include "Net/UnrealNetwork.h"

SERIALISE_INIT_SOURCE_IMPLEMENT(FTGOR_ModularMeshDescriptor);

DECLARE_CYCLE_STAT(TEXT("Total customisation time"), STAT_CustomisationTotal, STATGROUP_TGOR_CUST);
DECLARE_CYCLE_STAT(TEXT("Begin drawing customisation"), STAT_CustomisationBeginDraw, STATGROUP_TGOR_CUST);
DECLARE_CYCLE_STAT(TEXT("Apply customisations"), STAT_CustomisationApply, STATGROUP_TGOR_CUST);
DECLARE_CYCLE_STAT(TEXT("End drawing customisation"), STAT_CustomisationEndDraw, STATGROUP_TGOR_CUST);

FTGOR_ModularMeshDescriptor::FTGOR_ModularMeshDescriptor()
{
}

void FTGOR_ModularMeshDescriptor::Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const
{
	Package.WriteEntry(Instance);
	Package.WriteEntry(Parent);
}

void FTGOR_ModularMeshDescriptor::Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context)
{
	Package.ReadEntry(Instance);
	Package.ReadEntry(Parent);
}



UTGOR_ChildSkeletalMeshComponent::UTGOR_ChildSkeletalMeshComponent()
	: Super(),
	ChildComponentType(UTGOR_ChildSkeletalMeshComponent::StaticClass())
{
	SetIsReplicatedByDefault(true);
}

void UTGOR_ChildSkeletalMeshComponent::OnRegister()
{
	Super::OnRegister();
	SetUpdateAnimationInEditor(true);
}

void UTGOR_ChildSkeletalMeshComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME_CONDITION(UTGOR_ChildSkeletalMeshComponent, Descriptor, COND_None);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ChildSkeletalMeshComponent::OnReplicateDescriptor(const FTGOR_ModularMeshDescriptor& Old)
{
	AttachAndInitialise();
}

////////////////////////////////////////////////////////////////////////////////////////////////////


UTGOR_Bodypart* UTGOR_ChildSkeletalMeshComponent::GetBodypart() const
{
	return Descriptor.Instance.Content;
}

UTGOR_ChildSkeletalMeshComponent* UTGOR_ChildSkeletalMeshComponent::GetParent() const
{
	return Descriptor.Parent.Get();
}

UTGOR_ChildSkeletalMeshComponent* UTGOR_ChildSkeletalMeshComponent::GetMaster() const
{
	return Master.Get();
}

const FTGOR_BodypartInstance& UTGOR_ChildSkeletalMeshComponent::GetInstance() const
{
	return Descriptor.Instance;
}

const TArray<UTGOR_ChildSkeletalMeshComponent*>& UTGOR_ChildSkeletalMeshComponent::GetChildren() const
{
	return Children;
}

void UTGOR_ChildSkeletalMeshComponent::RemoveChild(UTGOR_ChildSkeletalMeshComponent* Node)
{
	Children.Remove(Node);
	Node->DestroyMeshes();
	Node->DestroyComponent();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool UTGOR_ChildSkeletalMeshComponent::GetBounds(bool IncludeChildren, FVector& Center, float& Extend) const
{
	UTGOR_Bodypart* Bodypart = GetBodypart();
	if (!IsValid(Bodypart)) return false;

	UTGOR_BodypartUserData* UserData = Bodypart->Mesh->GetAssetUserData<UTGOR_BodypartUserData>();
	if (!IsValid(UserData)) return false;

	int32 MaterialNum = GetNumMaterials();

	FVector MinPos = FVector(FLT_MAX);
	FVector MaxPos = FVector(-FLT_MAX);
	const FTransform& Transform = GetComponentTransform();

	// Recreate dynamic material instances
	for (const auto& Pair : Bodypart->Instanced_SectionInsertions.Collection)
	{
		if (UserData->RenderSections.IsValidIndex(Pair.Value))
		{
			const FVector SectionCenter = Transform.TransformPosition(UserData->RenderSections[Pair.Value].Center);
			const float SectionExtend = Transform.TransformVector(UserData->RenderSections[Pair.Value].Extend).Size();

			MinPos = MinPos.ComponentMin(SectionCenter - FVector(SectionExtend));
			MaxPos = MaxPos.ComponentMax(SectionCenter + FVector(SectionExtend));
		}
	}

	if (IncludeChildren)
	{
		for (UTGOR_ChildSkeletalMeshComponent* Child : Children)
		{
			if (IsValid(Child))
			{
				FVector ChildCenter;
				float ChildExtend;
				Child->GetBounds(true, ChildCenter, ChildExtend);

				MinPos = MinPos.ComponentMin(ChildCenter - FVector(ChildExtend));
				MaxPos = MaxPos.ComponentMax(ChildCenter + FVector(ChildExtend));
			}
		}
	}

	Center = (MaxPos + MinPos) / 2;
	Extend = (MaxPos - MinPos).Size() / 2;
	return true;
}


void UTGOR_ChildSkeletalMeshComponent::GatherMergeResources(FTGOR_BodypartResources& Resources) const
{
	// Gather all meshes
	UTGOR_Bodypart* Bodypart = GetBodypart();
	if (IsValid(Bodypart))
	{
		UTGOR_BodypartUserData* UserData = Bodypart->Mesh->GetAssetUserData<UTGOR_BodypartUserData>();
		if (IsValid(UserData))
		{
			for (const auto& SectionPair : Bodypart->Instanced_SectionInsertions.Collection)
			{
				const FTGOR_BodypartCustomisationCache* Cache = CustomisationCaches.Find(SectionPair.Key);
				if (Cache)
				{
					FTGOR_BodypartSectionResources& SectionResources = Resources.Sections.FindOrAdd(SectionPair.Key);
					const FIntPoint Size = UserData->GetTextureSize(SectionPair.Value);

					FTGOR_BodypartMeshResources MeshResources;
					MeshResources.Mesh = this;
					MeshResources.SectionIndex = SectionPair.Value;
					MeshResources.SizeX = Size.X;
					MeshResources.SizeY = Size.Y;

					for (const auto& TexturePair : Cache->Allocation.Textures)
					{
						MeshResources.Textures.Emplace(TexturePair.Key, TexturePair.Value.RenderTarget);
					}
					SectionResources.Meshes.Emplace(MeshResources);
				}
			}
		}
	}

	// Gather all customisations
	for (const FTGOR_CustomisationInstance& Instance : Descriptor.Instance.CustomisationInstances)
	{
		Resources.Customisations.FindOrAdd(Instance.Content).List.Emplace(Instance.Payload);
	}

	// Gather children
	for (UTGOR_ChildSkeletalMeshComponent* Child : Children)
	{
		if (IsValid(Child))
		{
			Child->GatherMergeResources(Resources);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FString UTGOR_ChildSkeletalMeshComponent::GetIdentification() const
{
	// Suffix for all outputs
	FString Suffix = FString(" at depth ") + FString::FromInt(Descriptor.Instance.Depth);

	// Take bodypart information if available
	UTGOR_Bodypart* Bodypart = GetBodypart();
	if (IsValid(Bodypart))
	{
		return(Bodypart->GetDefaultName() + Suffix);
	}
	else
	{
		return(FString("Undefined bodypart") + Suffix);
	}
}

FString UTGOR_ChildSkeletalMeshComponent::PrintTree() const
{
	FString Out = "";

	for (int i = 0; i < Descriptor.Instance.Depth; i++)
	{
		Out += ".-";
	}

	Out += "| ";
	Out += GetIdentification();
	Out += "\n ";

	// Print recursively
	for (UTGOR_ChildSkeletalMeshComponent* Child : Children)
	{
		if (IsValid(Child))
		{
			Out += Child->PrintTree();
		}
	}

	return(Out);
}

void UTGOR_ChildSkeletalMeshComponent::UpdateAppearance()
{
	if (Master.IsValid())
	{
		Master->OnAppearanceChanged.Broadcast();
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ChildSkeletalMeshComponent::AttachAndInitialise()
{
	if (Descriptor.Parent.IsValid())
	{
		Descriptor.Parent->Children.AddUnique(this);
		Master = Descriptor.Parent->Master;
	}

	if (Master.IsValid() && this != Master)
	{
		SetRenderCustomDepth(Master->bRenderCustomDepth);
		SetCullDistance(Master->CachedMaxDrawDistance);

		bEnablePerPolyCollision = Master->bEnablePerPolyCollision;
		SetCollisionProfileName(Master->GetCollisionProfileName());
		SetCollisionEnabled(Master->GetCollisionEnabled());
		SetCollisionResponseToChannels(Master->GetCollisionResponseToChannels());
		SetGenerateOverlapEvents(Master->GetGenerateOverlapEvents());
		bUpdateAnimationInEditor = Master->bUpdateAnimationInEditor;
	}

	// Build meshes
	UTGOR_Bodypart* Bodypart = GetBodypart();
	if (IsValid(Bodypart))
	{
		// Always reattach in case something went wrong
		Bodypart->AttachMesh(this, Master.Get(), Master->IgnoreFollowsMasterPose);

		// Set mesh and override materials
		Bodypart->OverrideMesh(this);

		// Build and apply materials
		BuildCustomisationInstances();
		ApplyInternalCustomisation();
	}
	else
	{
		ERROR(GetIdentification() + ": Bodypart not provided", Error)
	}

	// Update children
	for (UTGOR_ChildSkeletalMeshComponent* Child : Children)
	{
		if (IsValid(Child))
		{
			Child->AttachAndInitialise();
		}
	}
}

void UTGOR_ChildSkeletalMeshComponent::ClearMesh()
{
	// Clear materials
	CustomisationCaches.Empty();
	EmptyOverrideMaterials();

	// Clear mesh
	UTGOR_Bodypart* Bodypart = GetBodypart();
	if (IsValid(Bodypart))
	{
		Bodypart->ResetMesh(this);
	}
}


void UTGOR_ChildSkeletalMeshComponent::BuildCustomisationInstances()
{
	UTGOR_Bodypart* Bodypart = GetBodypart();
	if (!IsValid(Bodypart) || !IsValid(Bodypart->Mesh)) return;

	UTGOR_SkeletalMergeUserData* MergeUserData = Bodypart->Mesh->GetAssetUserData<UTGOR_SkeletalMergeUserData>();
	UTGOR_BodypartUserData* BodypartUserData = Bodypart->Mesh->GetAssetUserData<UTGOR_BodypartUserData>();
	if (!IsValid(BodypartUserData)) return;

	int32 MaterialNum = GetNumMaterials();

	// Recreate dynamic material instances
	for (const auto& Pair : Bodypart->Instanced_SectionInsertions.Collection)
	{
		// Add cache if valid Segment
		if (0 <= Pair.Value && Pair.Value < MaterialNum)
		{
			// Ensure texture bakes
			BodypartUserData->EnsureVertexBake(this, SkeletalMesh, Pair.Value, false);
			const FIntPoint Size = BodypartUserData->GetTextureSize(Pair.Value);

			if (IsValid(MergeUserData))
			{
				MergeUserData->EnsureVertexBake(this, SkeletalMesh, Pair.Value, false);
			}

			// Create dynamic material instances
			FTGOR_BodypartCustomisationCache& Cache = CustomisationCaches.FindOrAdd(Pair.Key);
			if (!IsValid(Cache.MaterialInstance))
			{
				Cache.MaterialInstance = CreateDynamicMaterialInstance(Pair.Value);
			}

			// Allocate textures
			for (UTGOR_Canvas* Canvas : Pair.Key->Instanced_CanvasInsertions.Collection)
			{
				const int32 SizeX = (1 << FMath::Max(Size.X + Canvas->Scale.ScaleX, 1));
				const int32 SizeY = (1 << FMath::Max(Size.Y + Canvas->Scale.ScaleY, 1));

				FTGOR_TextureAllocation& Texture = Cache.Allocation.Textures.FindOrAdd(Canvas);

				Texture.Pos = FVector2D::ZeroVector;
				Texture.Size = FVector2D(SizeX, SizeY);
				if (IsValid(Texture.RenderTarget) && Texture.RenderTarget->GetFormat() == Canvas->Format)
				{
					Texture.RenderTarget->ResizeTarget(SizeX, SizeY);
				}
				else
				{
					Texture.RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, SizeX, SizeY, Canvas->Format, FLinearColor::Black, false);
				}
			}
		}
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////

int32 UTGOR_ChildSkeletalMeshComponent::CountCustomisationInstances(UTGOR_Customisation* Content) const
{
	int32 Count = 0;
	UTGOR_Bodypart* Bodypart = GetBodypart();
	if (IsValid(Bodypart))
	{
		for (const FTGOR_CustomisationInstance& Instance : Descriptor.Instance.CustomisationInstances)
		{
			if (Instance.Content == Content)
			{
				Count++;
			}
		}
	}

	if (IsValid(Content) && Content->LayerType == ETGOR_LayerTypeEnumeration::Mandatory)
	{
		Count = FMath::Max(Count, 1);
	}
	return Count;
}

void UTGOR_ChildSkeletalMeshComponent::ResetCustomisationInstance(UTGOR_Customisation* Content, int32 Index)
{
	// Remove entry from material customisation
	int32 Num = Descriptor.Instance.CustomisationInstances.Num();
	for (int i = 0; i < Num; i++)
	{
		FTGOR_CustomisationInstance& CustomisationInstance = Descriptor.Instance.CustomisationInstances[i];

		if (CustomisationInstance.Content == Content)
		{
			if (Index-- == 0)
			{
				Descriptor.Instance.CustomisationInstances.RemoveAt(i);
				break;
			}
		}
	}

	// Reapply customisation
	ApplyInternalCustomisation();

	UpdateAppearance();
}

bool UTGOR_ChildSkeletalMeshComponent::QueryCustomisationInstance(FTGOR_CustomisationInstance& Instance) const
{
	UTGOR_Bodypart* Bodypart = GetBodypart();
	if (IsValid(Bodypart))
	{
		if (Bodypart->Instanced_CustomisationInsertions.Collection.Contains(Instance.Content))
		{
			Instance.Payload = Instance.Content->CreatePayload();

			// Call func on given instance
			const FTGOR_BodypartCustomisationCache* Cache = CustomisationCaches.Find(Instance.Content->Instanced_SectionInsertion.Collection);
			if (Cache != nullptr && IsValid(Cache->MaterialInstance))
			{
				Instance.Content->Query(Master.Get(), Instance.Payload, Bodypart, this, Cache->MaterialInstance, Cache->Allocation);
			}
			return true;
		}
	}
	return false;
}

void UTGOR_ChildSkeletalMeshComponent::ApplyCustomisationInstance(const FTGOR_CustomisationInstance& Instance, int32 Index)
{

	// Update function for self and all children
	auto UpdateChildren = [](UTGOR_ChildSkeletalMeshComponent* Parent, UTGOR_ChildSkeletalMeshComponent* Node)
	{
		Node->ApplyInternalCustomisation();
	};

	// Check all current instances for matches to update
	for (FTGOR_CustomisationInstance& CustomisationInstance : Descriptor.Instance.CustomisationInstances)
	{
		if (CustomisationInstance.Content == Instance.Content)
		{
			if (Index-- == 0)
			{
				CustomisationInstance = Instance;
				WalkTree(UpdateChildren);
				UpdateAppearance();
				return;
			}
		}
	}

	// Add new instance if no match
	Descriptor.Instance.CustomisationInstances.Add(Instance);
	WalkTree(UpdateChildren);
	UpdateAppearance();
}


void UTGOR_ChildSkeletalMeshComponent::ApplyInternalCustomisation()
{
	SCOPE_CYCLE_COUNTER(STAT_CustomisationTotal);

	UTGOR_Bodypart* Bodypart = GetBodypart();
	if (!IsValid(Bodypart) || !IsValid(Bodypart->Mesh)) return;

	// Clear materials
	UWorld* World = GetWorld();
	World->FlushDeferredParameterCollectionInstanceUpdates();
	for (auto& CachePair : CustomisationCaches)
	{
		SCOPE_CYCLE_COUNTER(STAT_CustomisationBeginDraw);

		if(IsValid(CachePair.Value.MaterialInstance))
		{
			CachePair.Value.MaterialInstance->ClearParameterValues();
		}

		for (auto& TexturePair : CachePair.Value.Allocation.Textures)
		{
			TexturePair.Value.Context = FDrawToRenderTargetContext();
			TexturePair.Value.Context.RenderTarget = TexturePair.Value.RenderTarget;
			TexturePair.Value.Context.DrawEvent = new FDrawEvent();

			FTextureRenderTargetResource* RenderTargetResource = TexturePair.Value.RenderTarget->GameThread_GetRenderTargetResource();
			TexturePair.Value.Canvas = new FCanvas(RenderTargetResource, nullptr, World, World->FeatureLevel,FCanvas::CDM_ImmediateDrawing);
					
			FName RTName = TexturePair.Value.RenderTarget->GetFName();
			FDrawEvent* DrawEvent = TexturePair.Value.Context.DrawEvent;
			ENQUEUE_RENDER_COMMAND(BeginDrawEventCommand)(
				[RTName, DrawEvent, RenderTargetResource](FRHICommandListImmediate& RHICmdList)
			{
				RenderTargetResource->FlushDeferredResourceUpdate(RHICmdList);
				BEGIN_DRAW_EVENTF(
					RHICmdList,
					DrawCanvasToTarget,
					(*DrawEvent),
					*RTName.ToString());
			});

			TexturePair.Value.Canvas->Clear(FLinearColor::Black);

			/*
			UTexture* DefaultTexture;
			CachePair.Value.MaterialInstance->GetTextureParameterDefaultValue(TexturePair.Key->ParameterName, DefaultTexture);
			if (IsValid(DefaultTexture))
			{
				const FVector2D Size(TexturePair.Value.RenderTarget->SizeX, TexturePair.Value.RenderTarget->SizeY);
				FCanvasTileItem TileItem(FVector2D(0.0f, 0.0f), DefaultTexture->Resource, Size, FVector2D(0.0f, 0.0f), FVector2D(1.0f, 1.0f), FColor(127, 127, 127, 255));
				TileItem.BlendMode = ESimpleElementBlendMode::SE_BLEND_Opaque;
				TexturePair.Value.Canvas->DrawItem(TileItem);
			}
			else
			{
				TexturePair.Value.Canvas->Clear(FLinearColor::Black);
			}
			*/
			//UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, TexturePair.Value.RenderTarget, TexturePair.Value.Canvas, TexturePair.Value.Size, TexturePair.Value.Context);
		}
	}

	// Apply customisations in order
	TArray<UTGOR_Customisation*> CustomList = Bodypart->Instanced_CustomisationInsertions.GetListOfType<UTGOR_Customisation>();
	for (UTGOR_Customisation* CustomEntry : CustomList)
	{
		SCOPE_CYCLE_COUNTER(STAT_CustomisationApply);

		// Find instanceFTGOR_BodypartCustomisationCache* Cache = CustomisationCaches.Find(Instance.Content);
		FTGOR_BodypartCustomisationCache* Cache = CustomisationCaches.Find(CustomEntry->Instanced_SectionInsertion.Collection);
		if (Cache != nullptr)
		{
			// Get instance information
			TArray<FTGOR_CustomisationInstance> Instances;
			if (GetCustomisationMatching(Instances, CustomEntry, ETGOR_CustomisationQueryEnumeration::All))
			{
				// Actually apply the customisations
				for (const FTGOR_CustomisationInstance& Instance : Instances)
				{
					if (IsValid(Cache->MaterialInstance))
					{
						Instance.Content->Apply(Master.Get(), Instance.Payload, Bodypart, this, Cache->MaterialInstance, Cache->Allocation);
					}
				}
			}
		}
	}

	// Get merge data
	UTGOR_SkeletalMergeUserData* MergeUserData = Bodypart->Mesh->GetAssetUserData<UTGOR_SkeletalMergeUserData>();
	UTGOR_BodypartUserData* UserData = Bodypart->Mesh->GetAssetUserData<UTGOR_BodypartUserData>();

	// Apply rendertargets
	for (auto& CachePair : CustomisationCaches)
	{
		SCOPE_CYCLE_COUNTER(STAT_CustomisationEndDraw);
		for (auto& TexturePair : CachePair.Value.Allocation.Textures)
		{

			/*
			if (Descriptor.Parent.IsValid())
			{
				SCOPE_CYCLE_COUNTER(STAT_CustomisationMerge);

				FTGOR_BodypartCustomisationCache* ParentCache = Descriptor.Parent->CustomisationCaches.Find(CachePair.Key);

				UTGOR_Bodypart* ParentBodypart = Descriptor.Parent->GetBodypart();
				UTGOR_BodypartUserData* ParentUserData = ParentBodypart->Mesh->GetAssetUserData<UTGOR_BodypartUserData>();
				if (ParentCache && IsValid(ParentUserData))
				{
					FTGOR_TextureAllocation* ParentTexture = ParentCache->Allocation.Textures.Find(TexturePair.Key);
					if (ParentTexture)
					{
						const FVector2D Size = FVector2D(TexturePair.Value.RenderTarget->SizeX, TexturePair.Value.RenderTarget->SizeY);
						if (IsValid(MergeUserData))
						{
							const FTransform PointTransform = FTransform(FQuat::Identity, FVector::ZeroVector, FVector(Size, 1.0f));
							TexturePair.Key->RenderBlend(MergeUserData, ParentUserData, Bodypart, CachePair.Key, ParentTexture->RenderTarget, PointTransform, TexturePair.Value.Canvas);
						}
						else
						{
							TexturePair.Key->RenderBlendTexture(UserData, ParentUserData, Bodypart, CachePair.Key, ParentTexture->RenderTarget, TexturePair.Value.Canvas, Size);
						}
					}
				}
			}


			SCOPE_CYCLE_COUNTER(STAT_CustomisationMerge);
			// Get merge data
			UTGOR_Bodypart* ParentBodypart = Descriptor.Parent->GetBodypart();
			UTGOR_BodypartUserData* ParentUserData = ParentBodypart->Mesh->GetAssetUserData<UTGOR_BodypartUserData>();
			if (IsValid(ParentUserData))
			{
				FTGOR_BodypartCustomisationCache* ParentCache = Descriptor.Parent->CustomisationCaches.Find(CachePair.Key);
				if (ParentCache)
				{
					FTGOR_TextureAllocation* ParentTexture = ParentCache->Allocation.Textures.Find(TexturePair.Key);
					if (ParentTexture)
					{
						const FVector2D Size = FVector2D(TexturePair.Value.RenderTarget->SizeX, TexturePair.Value.RenderTarget->SizeY);
						const FTransform PointTransform = FTransform(FQuat::Identity, FVector::ZeroVector, FVector(Size, 1.0f));
						TexturePair.Key->RenderMerge(ParentUserData, Bodypart, CachePair.Key, ParentTexture->RenderTarget, PointTransform, TexturePair.Value.Canvas);
					}
				}
			}
			*/

			//UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, TexturePair.Value.Context);
			if (TexturePair.Value.Canvas)
			{
				TexturePair.Value.Canvas->Flush_GameThread();
				delete TexturePair.Value.Canvas;
				TexturePair.Value.Canvas = nullptr;
			}

			FTextureRenderTargetResource* RenderTargetResource = TexturePair.Value.Context.RenderTarget->GameThread_GetRenderTargetResource();
			FDrawEvent* DrawEvent = TexturePair.Value.Context.DrawEvent;
			ENQUEUE_RENDER_COMMAND(CanvasRenderTargetResolveCommand)(
				[RenderTargetResource, DrawEvent](FRHICommandList& RHICmdList)
				{
					RHICmdList.CopyToResolveTarget(RenderTargetResource->GetRenderTargetTexture(), RenderTargetResource->TextureRHI, FResolveParams());
					STOP_DRAW_EVENT((*DrawEvent));
					delete DrawEvent;
				}
			);

			// Remove references to the context now that we've resolved it, to avoid a crash when EndDrawCanvasToRenderTarget is called multiple times with the same context
			// const cast required, as BP will treat Context as an output without the const
			const_cast<FDrawToRenderTargetContext&>(TexturePair.Value.Context) = FDrawToRenderTargetContext();

			if (IsValid(CachePair.Value.MaterialInstance))
			{
				// Set texture parameter to computed RT
				CachePair.Value.MaterialInstance->SetTextureParameterValue(TexturePair.Key->ParameterName, TexturePair.Value.RenderTarget);

				// Apply texture override if needed
				for (const auto& Pair : Bodypart->CanvasOverride)
				{
					if (TexturePair.Key->IsA(Pair.Key))
					{
						CachePair.Value.MaterialInstance->SetTextureParameterValue(TexturePair.Key->ParameterName, Pair.Value);
					}
				}
			}
		}
	}
}

void UTGOR_ChildSkeletalMeshComponent::ApplyToLocalInstances(FCustomisationNodeInstanceDelegate Delegate, UTGOR_Customisation* Content)
{
	if (Delegate.IsBound())
	{
		ApplyToLocalInstances([&Delegate](UTGOR_ChildSkeletalMeshComponent* Node, UMaterialInstanceDynamic* MaterialInstance)
		{
			Delegate.Execute(Node, MaterialInstance);
		}, Content);
	}
}

void UTGOR_ChildSkeletalMeshComponent::ApplyToLocalInstances(CustomisationNodeInstanceLambda Func, UTGOR_Customisation* Content)
{
	// Find instance
	FTGOR_BodypartCustomisationCache* Cache = CustomisationCaches.Find(Content->Instanced_SectionInsertion.Collection);

	// Call func on all instances
	if (Cache != nullptr)
	{
		for (auto& CachePair : CustomisationCaches)
		{
			if (IsValid(CachePair.Value.MaterialInstance))
			{
				Func(this, CachePair.Value.MaterialInstance);
			}
		}
	}
}


void UTGOR_ChildSkeletalMeshComponent::ApplyToAllInstances(FCustomisationNodeInstanceDelegate Delegate, UTGOR_Customisation* Content)
{
	if (Delegate.IsBound())
	{
		ApplyToAllInstances(
			[&Delegate](UTGOR_ChildSkeletalMeshComponent* Node, UMaterialInstanceDynamic* MaterialInstance)
		{
			Delegate.Execute(Node, MaterialInstance);
		}, Content);
	}
}

void UTGOR_ChildSkeletalMeshComponent::ApplyToAllInstances(CustomisationNodeInstanceLambda Func, UTGOR_Customisation* Content)
{
	ApplyToLocalInstances(Func, Content);

	// Apply to all children
	for (UTGOR_ChildSkeletalMeshComponent* Child : Children)
	{
		if (IsValid(Child))
		{
			Child->ApplyToAllInstances(Func, Content);
		}
	}
}



bool UTGOR_ChildSkeletalMeshComponent::GetCustomisationMatching(TArray<FTGOR_CustomisationInstance>& CustomisationInstances, UTGOR_Customisation* Content, ETGOR_CustomisationQueryEnumeration Query) const
{
	// Ask locally if enabled
	UTGOR_Bodypart* Bodypart = GetBodypart();
	if (IsValid(Bodypart))
	{
		if (Bodypart->Instanced_CustomisationInsertions.Collection.Contains(Content))
		{
			// Check all selected entries for matches
			for (const FTGOR_CustomisationInstance& Instance : Descriptor.Instance.CustomisationInstances)
			{
				if (Instance.Content == Content)
				{
					// Actually add instances for all or local only (but still return if not multiple since that means there is *no* inheritance)
					if (Query != ETGOR_CustomisationQueryEnumeration::Inherited)
					{
						CustomisationInstances.Emplace(Instance);
					}

					if (Content->LayerType != ETGOR_LayerTypeEnumeration::Multiple)
					{
						return true;
					}
				}
			}
		}
	}


	// Recursively ask parent node all or inheritance only
	if (Descriptor.Parent.IsValid() && Content->TrickleDown && Query >= ETGOR_CustomisationQueryEnumeration::All)
	{
		// TODO: Do parent layers go *before* child layers?
		// All queries are inherited from parent
		Descriptor.Parent->GetCustomisationMatching(CustomisationInstances, Content, ETGOR_CustomisationQueryEnumeration::All);
	}

	// Create empty customisation if mandatory
	if (Content->LayerType == ETGOR_LayerTypeEnumeration::Mandatory && CustomisationInstances.Num() == 0)
	{
		// Actually add instances for all or local only
		if (Query <= ETGOR_CustomisationQueryEnumeration::All)
		{
			FTGOR_CustomisationInstance Instance;
			Instance.Content = Content;
			Instance.Payload = Content->CreatePayload();
			CustomisationInstances.Emplace(Instance);
		}
		return true;
	}

	return CustomisationInstances.Num() > 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_ChildSkeletalMeshComponent::WalkTree(BodypartNodeLambda Func)
{
	Func(Descriptor.Parent.Get(), this);

	// Iterate children
	int32 Num = Children.Num();
	for (int i = 0; i < Num; i++)
	{
		UTGOR_ChildSkeletalMeshComponent* Child = Children[i];
		if (IsValid(Child))
		{
			// Recursively walk tree
			Child->WalkTree(Func);
		}
	}
}

void UTGOR_ChildSkeletalMeshComponent::WalkTree(FBodypartNodeDelegate Delegate)
{
	if (Delegate.IsBound())
	{
		WalkTree([Delegate](UTGOR_ChildSkeletalMeshComponent* Parent, UTGOR_ChildSkeletalMeshComponent* Node) {
			Delegate.Execute(Parent, Node);
		});
	}
}

void UTGOR_ChildSkeletalMeshComponent::ChangeBodypart(UTGOR_Bodypart* Content)
{
	if (!Master.IsValid())
	{
		ERROR(GetIdentification() + ": No master defined", Error)
	}

	if (!IsValid(Content))
	{
		ERROR(GetIdentification() + ": Invalid bodypart index", Error)
	}

	Descriptor.Instance.Content = Content;

	// Reconstruct the tree from this point
	TArray<FTGOR_BodypartInstance> Path;
	FTGOR_BodypartInstance Instance;
	Instance.Content = Content;
	Path.Emplace(Instance);

	ConstructPath(Path);
	ConstructTree(Path, 0);
}

void UTGOR_ChildSkeletalMeshComponent::EnsureAutomatic(TSubclassOf<UTGOR_Bodypart> Class)
{
	// Ensure for all children first to not call it twice on new bodyparts
	for (UTGOR_ChildSkeletalMeshComponent* Child : Children)
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

void UTGOR_ChildSkeletalMeshComponent::PurgeAutomatic(TSubclassOf<UTGOR_Bodypart> Class)
{
	// Remove automatic bodypart
	const FTGOR_BodypartChild Requirement = GetRequirement(Class);
	if (Requirement.Type == ETGOR_BodypartChildType::Automatic)
	{
		UTGOR_ChildSkeletalMeshComponent* Node = GetChildFromType(Class);
		if (IsValid(Node))
		{
			Node->DestroyMeshes();
			Children.Remove(Node);
		}
	}

	// Purge for all children afterwards to not purge branches that are going to be deleted anyway
	for (UTGOR_ChildSkeletalMeshComponent* Child : Children)
	{
		if (IsValid(Child))
		{
			Child->PurgeAutomatic(Class);
		}
	}

}

UTGOR_ChildSkeletalMeshComponent* UTGOR_ChildSkeletalMeshComponent::AddChildType(TSubclassOf<UTGOR_Bodypart> Class)
{
	UTGOR_Bodypart* Bodypart = GetBodypart();
	if (IsValid(Bodypart))
	{
		// Add first possible bodypart of the right type
		const TArray<UTGOR_Bodypart*>& PossibleBodyparts = Bodypart->Instanced_ChildInsertions.GetListOfType(Class); //GetIListFromType<UTGOR_Bodypart>(Class);
		for (UTGOR_Bodypart* Content : PossibleBodyparts)
		{
			if (Content->IsA(Class))
			{
				return AddChildBodypart(Content);
			}
		}
	}
	return nullptr;
}

UTGOR_ChildSkeletalMeshComponent* UTGOR_ChildSkeletalMeshComponent::AddChildBodypart(UTGOR_Bodypart* Content)
{
	if (Descriptor.Instance.Depth >= MAX_BODYPART_DEPTH)
	{
		ERRET(GetIdentification() + ": Bodypart tree too big", Error, nullptr);
	}

	// Create subpath
	FTGOR_BodypartInstance Instance;

	Instance.Depth = Descriptor.Instance.Depth + 1;
	Instance.Content = Content;

	TArray<FTGOR_BodypartInstance> Appendix;
	Appendix.Add(Instance);

	// Create meshcomponent and attach to parent
	UTGOR_ChildSkeletalMeshComponent* Node = NewObject<UTGOR_ChildSkeletalMeshComponent>(this, ChildComponentType);
	if (IsValid(Node))
	{
		/*
		bEnablePerPolyCollision = false;
		Node->SetCollisionProfileName("NoCollision");
		Node->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Node->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		Node->SetGenerateOverlapEvents(false);
		*/

		Node->SetRelativeLocation(FVector::ZeroVector);
		Node->SetRelativeRotation(FQuat::Identity);
		Node->SetRelativeScale3D(FVector::OneVector);

		Node->RegisterComponent();

		Node->Descriptor.Parent = this;

		// Recursively construct subtree (children register themselves on parent)
		Node->ConstructTree(Appendix, 0);
	}

	return Node;
}


int32 UTGOR_ChildSkeletalMeshComponent::ConstructTree(const TArray<FTGOR_BodypartInstance>& Path, int32 Index)
{
	if (Index >= MAX_BODYPART_DEPTH)
	{
		ERRET(GetIdentification() + ": Too many bodyparts", Error, 0);
	}

	SINGLETON_RETCHK(0);

	// Check index to prevent empty nodes
	int32 Num = Path.Num();
	if (Index >= Num)
	{
		ERRET(GetIdentification() + ": Reached the end of the path too early", Error, 0);
	}

	const FTGOR_BodypartInstance& Instance = Path[Index];

	// Only rebuild if bodypart changed
	UTGOR_Bodypart* Bodypart = GetBodypart();
	if (Bodypart != Instance.Content)
	{
		// Clear children
		DestroyMeshes();
		Children.Empty();
	}

	// Initialise own node
	BuildNode(Instance, Instance.Depth);
	Bodypart = GetBodypart();

	// Pool to check which children weren't loaded back
	TSet<UTGOR_ChildSkeletalMeshComponent*> Unused(Children);

	// Look for children and keep track of how many got added
	int32 Count = 1;
	while (Index + Count < Num)
	{
		const FTGOR_BodypartInstance& Next = Path[Index + Count];

		// Break once end of branch reached
		if (Next.Depth < Descriptor.Instance.Depth + 1)
		{
			break;
		}

		// Skip children (in case of invalid entry)
		if (Next.Depth > Descriptor.Instance.Depth + 1)
		{
			Count++;
			continue;
		}

		// Check whether child is allowed to be added
		if (CheckChildValidity(Next.Content, Unused, Index + Count))
		{
			// Look for already fitting child node
			UTGOR_ChildSkeletalMeshComponent* Child = GetChild(Next.Content);

			// Create new node if not found
			if (Child == nullptr)
			{
				Child = AddChildBodypart(Next.Content);
			}
			else
			{
				// Remove from unused pool
				Unused.Remove(Child);
			}

			// Recursively construct subtree
			int32 Constructed = Child->ConstructTree(Path, Index + Count);
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
				UTGOR_ChildSkeletalMeshComponent* Node = GetChildFromType(Requirement.Class);
				if (!IsValid(Node))
				{
					Node = AddChildType(Requirement.Class);
				}

				// Don't clean up children that are always required
				Unused.Remove(Node);
			}
			else if (Requirement.Type == ETGOR_BodypartChildType::Automatic)
			{
				// Check what kind of bodypart is expected to be loaded automatically
				TSubclassOf<UTGOR_Bodypart> Handle = Master->GetHandleOfType(Requirement.Class);
				if (*Handle)
				{
					UTGOR_ChildSkeletalMeshComponent* Node = GetChildFromType(Handle);
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

void UTGOR_ChildSkeletalMeshComponent::ConstructPath(TArray<FTGOR_BodypartInstance>& Path) const
{
	if (Path.Num() >= MAX_BODYPART_DEPTH)
	{
		ERROR(GetIdentification() + ": Too many bodyparts", Error)
	}

	Path.Add(Descriptor.Instance);

	// Add children to path that aren't automatically changed
	int32 Num = Children.Num();
	for (int i = 0; i < Num; i++)
	{
		UTGOR_ChildSkeletalMeshComponent* Child = Children[i];
		if (IsValid(Child))
		{
			UTGOR_Bodypart* Bodypart = Child->GetBodypart();
			if (IsValid(Bodypart))
			{
				const FTGOR_BodypartChild ChildType = GetRequirement(Bodypart->GetClass());
				if (ChildType.Type != ETGOR_BodypartChildType::Automatic)
				{
					// Recursively add children
					Child->ConstructPath(Path);
				}
			}
		}
	}
}

bool UTGOR_ChildSkeletalMeshComponent::CheckChildValidity(UTGOR_Bodypart* ChildBodypart, const TSet<UTGOR_ChildSkeletalMeshComponent*>& Unused, uint8 Depth) const
{
	// Setup node
	if (IsValid(ChildBodypart))
	{
		// Check if selection is allowed
		UTGOR_Bodypart* Bodypart = GetBodypart();
		if (IsValid(Bodypart))
		{
			//if (!Bodypart->ContainsI<UTGOR_Bodypart>(ChildBodypart))
			if (!Bodypart->Instanced_ChildInsertions.Contains(ChildBodypart))
			{
				ERRET(ChildBodypart->GetDefaultName() + " Bodypart not allowed in " + Bodypart->GetDefaultName(), Warning, false);
			}

			FTGOR_BodypartChild ChildType = GetRequirement(ChildBodypart->GetClass());
			if (ChildType.Type == ETGOR_BodypartChildType::Automatic)
			{
				ERRET(ChildBodypart->GetDefaultName() + " is allocated for " + ChildType.Class->GetName() + " which is automatically set in " + Bodypart->GetDefaultName() + " and is not customisable", Warning, false);
			}

			UTGOR_ChildSkeletalMeshComponent* Node = GetChildFromType(ChildBodypart->GetClass());
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

void UTGOR_ChildSkeletalMeshComponent::BuildNode(const FTGOR_BodypartInstance& Instance, uint8 Depth)
{
	SINGLETON_CHK;

	Descriptor.Instance = Instance;
	Descriptor.Instance.Depth = Depth;

	AttachAndInitialise();
	UTGOR_Bodypart* Bodypart = GetBodypart();

	TMap<UTGOR_Customisation*, int32> ContentCount;

	// Validate all customisations and remove them if necessary
	for (auto Its = Descriptor.Instance.CustomisationInstances.CreateIterator(); Its; Its++)
	{
		int32& Count = ContentCount.FindOrAdd(Its->Content);

		// Remove either if no instance exists or customisation is invalid
		FTGOR_BodypartCustomisationCache* Cache = CustomisationCaches.Find(Its->Content->Instanced_SectionInsertion.Collection);
		if (!Cache || // Customisation not supported by this bodypart
			//!Its->Content->Check(Master.Get(), Its->Payload, Bodypart) || // Invalid payload
			(Its->Content->LayerType <= ETGOR_LayerTypeEnumeration::Optional && Count > 0)) // Only one customisation of this type allowed
		{
			Its.RemoveCurrent();
		}
		else
		{
			Count += 1;
		}
	}
}

void UTGOR_ChildSkeletalMeshComponent::DestroyMeshes()
{
	ClearMesh();

	// Recursive call to all children
	for (UTGOR_ChildSkeletalMeshComponent* Child : Children)
	{
		if (IsValid(Child))
		{
			Child->DestroyMeshes();
			Child->DestroyComponent();
		}
	}
	Children.Empty();
}


void UTGOR_ChildSkeletalMeshComponent::MergeChildren()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_ChildSkeletalMeshComponent* UTGOR_ChildSkeletalMeshComponent::GetChild(UTGOR_Bodypart* Content) const
{
	for (UTGOR_ChildSkeletalMeshComponent* Child : Children)
	{
		if (IsValid(Child))
		{
			UTGOR_Bodypart* Bodypart = Child->GetBodypart();
			if (Bodypart == Content)
			{
				return Child;
			}
		}
	}
	return nullptr;
}

UTGOR_ChildSkeletalMeshComponent* UTGOR_ChildSkeletalMeshComponent::GetChildFromType(TSubclassOf<UTGOR_Bodypart> Class) const
{
	if (*Class)
	{
		for (UTGOR_ChildSkeletalMeshComponent* Child : Children)
		{
			if (IsValid(Child))
			{
				UTGOR_Bodypart* Bodypart = Child->GetBodypart();
				if (IsValid(Bodypart) && Bodypart->IsA(Class))
				{
					return Child;
				}
			}
		}
	}
	return nullptr;
}

FTGOR_BodypartChild UTGOR_ChildSkeletalMeshComponent::GetRequirement(TSubclassOf<UTGOR_Bodypart> Class) const
{
	// Check for matching requirement class
	UTGOR_Bodypart* Bodypart = GetBodypart();
	if (IsValid(Bodypart))
	{
		for (const FTGOR_BodypartChild& Requirement : Bodypart->Children)
		{
			if (*Requirement.Class && Class->IsChildOf(Requirement.Class))
			{
				return Requirement;
			}
		}
	}
	return FTGOR_BodypartChild();
}

bool UTGOR_ChildSkeletalMeshComponent::ContainsType(TSubclassOf<UTGOR_Bodypart> Class) const
{
	return IsValid(GetChildFromType(Class));
}