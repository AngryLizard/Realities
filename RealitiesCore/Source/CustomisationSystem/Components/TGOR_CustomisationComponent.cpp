// The Gateway of Realities: Planes of Existence.

#include "TGOR_CustomisationComponent.h"

#include "CustomisationSystem/UserData/TGOR_BodypartUserData.h"
#include "CustomisationSystem/UserData/TGOR_SkeletalMergeUserData.h"

#include "CustomisationSystem/UserData/TGOR_SkeletalMerge.h"
#include "CustomisationSystem/UserData/TGOR_SkeletalExport.h"
#include "CustomisationSystem/Content/Customisations/TGOR_Customisation.h"
#include "CustomisationSystem/Content/TGOR_Bodypart.h"
#include "CustomisationSystem/Content/TGOR_Section.h"
#include "CustomisationSystem/Content/TGOR_Canvas.h"
#include "CustomisationSystem/Content/TGOR_Modular.h"
#include "DimensionSystem/Content/TGOR_Spawner.h"

#include "Materials/MaterialInstanceDynamic.h"

#include "CoreSystem/TGOR_Singleton.h"
#include "RealitiesUtility/Utility/TGOR_Error.h"

DECLARE_CYCLE_STAT(TEXT("Total customisation time"), STAT_CustomisationTotal, STATGROUP_TGOR_CUST);
DECLARE_CYCLE_STAT(TEXT("Begin drawing customisation"), STAT_CustomisationBeginDraw, STATGROUP_TGOR_CUST);
DECLARE_CYCLE_STAT(TEXT("Apply customisations"), STAT_CustomisationApply, STATGROUP_TGOR_CUST);
DECLARE_CYCLE_STAT(TEXT("End drawing customisation"), STAT_CustomisationEndDraw, STATGROUP_TGOR_CUST);

UTGOR_CustomisationComponent::UTGOR_CustomisationComponent()
:	Super()
,	MergedTextureLimit(12)
,	Modular(nullptr)
{
}

void UTGOR_CustomisationComponent::UpdateContent_Implementation(FTGOR_SpawnerDependencies& Dependencies)
{
	ITGOR_SpawnerInterface::UpdateContent_Implementation(Dependencies);

	Modular = Dependencies.Spawner->GetMFromType<UTGOR_Modular>(TargetModular);
	ApplyAndCreateMerge(CurrentAppearance);
}

TMap<int32, UTGOR_SpawnModule*> UTGOR_CustomisationComponent::GetModuleType_Implementation() const
{
	return MakeModuleList(Modular);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_CustomisationComponent::ApplyAndCreateMerge(const FTGOR_AppearanceInstance& Instance)
{
	if (IsValid(Modular))
	{
		TArray<UTGOR_Bodypart*> AvailableRoots = Modular->Instanced_BodypartInsertions.GetListOfType<UTGOR_Bodypart>().FilterByPredicate([](UTGOR_Bodypart* Bodypart) {return Bodypart->IsBodypartRoot; });
		if (AvailableRoots.Num() > 0)
		{
			CurrentAppearance = Instance;
			if (CurrentAppearance.Bodyparts.Num() == 0 || !AvailableRoots.Contains(CurrentAppearance.Bodyparts[0].Content))
			{
				FTGOR_BodypartInstance NewInstance;
				NewInstance.CustomisationInstances.Empty();
				NewInstance.Content = AvailableRoots[0];

				CurrentAppearance.Bodyparts.Reset(1);
				CurrentAppearance.Bodyparts.Emplace(NewInstance);
			}

			CurrentAppearance.Bodyparts[0].Depth = 0;

			CustomisationEntries.Empty();
			CustomisationMaterials.Empty();
			CustomisationTextures.Empty();
			ConstructTree(0);

			FTGOR_BodypartMergeOutput Output = MergeBodyparts();

			FEditorScriptExecutionGuard ScriptGuard;
			OnAppearanceChanged.Broadcast(Output);

			ApplyInternalCustomisation();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_BodypartMergeOutput UTGOR_CustomisationComponent::MergeBodyparts()
{
	FTGOR_BodypartMergeOutput Output;

	USkeletalMesh* TargetMesh = NewObject<USkeletalMesh>();
	if (IsValid(TargetMesh) && IsValid(Modular) && IsValid(Modular->Skeleton))
	{
		TargetMesh->SetSkeleton(Modular->Skeleton);

		// Establish order of sections
		TArray<UTGOR_Section*> Sections;
		CustomisationMaterials.GetKeys(Sections);
		const int32 SectionNum = Sections.Num();

		// ...
		TArray<const USkeletalMesh*> Meshes;
		TArray<FTGOR_SkelMeshMergeSectionMapping> Mappings;
		FTGOR_SkelMeshMergeUVTransforms UVTransforms;

		// Gather meshes and textures
		TMap<UTGOR_Canvas*, FIntPoint> SmallestSizes;
		TMap<UTGOR_Canvas*, TArray<TPair<FIntVector, FIntPoint>>> MeshAllocations;
		Output.Sections.SetNum(SectionNum);
		for (int32 SectionIndex = 0; SectionIndex < SectionNum; SectionIndex++)
		{
			UTGOR_Section* Section = Sections[SectionIndex];
			Output.Sections[SectionIndex].Section = Section;

			const int32 NodeNum = CurrentAppearance.Bodyparts.Num();
			for(int32 NodeIndex = 0; NodeIndex < NodeNum; NodeIndex++)
			{
				const FTGOR_BodypartInstance& Instance = CurrentAppearance.Bodyparts[NodeIndex];
				const FTGOR_CustomisationMeshEntry& Entry = CustomisationEntries[NodeIndex];
				if (const FTGOR_CustomisationMeshSection* MeshSection = Entry.Sections.Find(Section))
				{
					// Get mesh index
					const int32 MeshIndex = Meshes.AddUnique(Instance.Content->Mesh);
					Mappings.SetNum(FMath::Max(Mappings.Num(), MeshIndex + 1));

					const FIntVector MeshPair = FIntVector(MeshIndex, SectionIndex, NodeIndex);

					// Extract resources
					for (const auto& TexturePair : MeshSection->Textures)
					{
						// Extract smallest texture unit
						if (FIntPoint* SmallestSize = SmallestSizes.Find(TexturePair.Key))
						{
							SmallestSize->X = FMath::Min(TexturePair.Value.Allocation.X, SmallestSize->X);
							SmallestSize->Y = FMath::Min(TexturePair.Value.Allocation.Y, SmallestSize->Y);
						}
						else
						{
							SmallestSizes.Emplace(TexturePair.Key, TexturePair.Value.Allocation);
						}
						MeshAllocations.FindOrAdd(TexturePair.Key).Emplace(MeshPair, TexturePair.Value.Allocation);
					}

					// Section remapping
					const int32 MaterialIndex = Instance.Content->Instanced_SectionInsertions.Collection[Section];
					const int32 MaterialNum = Instance.Content->Mesh->GetMaterials().Num();
					FTGOR_SkelMeshMergeSectionMapping& Mapping = Mappings[MeshIndex];
					Mapping.SectionIDs.SetNumZeroed(MaterialNum);
					if (Mapping.SectionIDs.IsValidIndex(MaterialIndex))
					{
						Mapping.SectionIDs[MaterialIndex] = SectionIndex;
					}
				}
			}
		}

		// Create transforms
		const int32 MeshNum = Meshes.Num();
		UVTransforms.UVTransformsPerMesh.SetNum(MeshNum);
		for (int32 MeshIndex = 0; MeshIndex < MeshNum; MeshIndex++)
		{
			const int32 MaterialNum = Meshes[MeshIndex]->GetMaterials().Num();
			UVTransforms.UVTransformsPerMesh[MeshIndex].SetNum(MaterialNum);
			for (int32 MaterialIndex = 0; MaterialIndex < MaterialNum; MaterialIndex++)
			{
				UVTransforms.UVTransformsPerMesh[MeshIndex][MaterialIndex].SetNum(1); // Only need first UV channel
				UVTransforms.UVTransformsPerMesh[MeshIndex][MaterialIndex][0] = FTransform::Identity;
			}
		}

		for (auto& CanvasPair : MeshAllocations)
		{
			const FIntPoint& SmallestSize = SmallestSizes[CanvasPair.Key];

			// Smallest texture unit size
			const int32 UnitX = 1 << SmallestSize.X;
			const int32 UnitY = 1 << SmallestSize.Y;

			// Texture limit
			const int32 LimitX = 1 << (MergedTextureLimit - SmallestSize.X);
			const int32 LimitY = 1 << (MergedTextureLimit - SmallestSize.Y);

			// Normalise size to grid of length 1
			for (TPair<FIntVector, FIntPoint>& MeshAllocation : CanvasPair.Value)
			{
				MeshAllocation.Value.X -= SmallestSize.X;
				MeshAllocation.Value.X = (1 << MeshAllocation.Value.X);
				MeshAllocation.Value.Y -= SmallestSize.Y;
				MeshAllocation.Value.Y = (1 << MeshAllocation.Value.Y);
			}

			// Sort by height so we can stack
			CanvasPair.Value.Sort([](const TPair<FIntVector, FIntPoint>& A, const TPair<FIntVector, FIntPoint>& B) -> bool
			{
				if (A.Value.Y == B.Value.Y)
				{
					return A.Value.X > B.Value.X;
				}
				return A.Value.Y > B.Value.Y;
			});

			// Create image segments
			int32 AllocPosX = 0;
			int32 AllocPosY = 0;
			int32 AllocSizeX = 0;
			int32 AllocSizeY = 0;
			TMap<FIntVector, FTGOR_BodypartPlacement> Placements;
			for (const TPair<FIntVector, FIntPoint>& MeshAllocation : CanvasPair.Value)
			{
				if (AllocPosX + MeshAllocation.Value.X > LimitX)
				{
					AllocPosY = AllocSizeY;
					AllocPosX = 0;
				}

				FTGOR_BodypartPlacement Placement;
				Placement.PosX = AllocPosX;
				Placement.PosY = AllocPosY;
				Placement.SizeX = MeshAllocation.Value.X;
				Placement.SizeY = MeshAllocation.Value.Y;
				Placements.Emplace(MeshAllocation.Key, Placement);

				AllocSizeY = FMath::Max(AllocPosY + MeshAllocation.Value.Y, AllocSizeY);
				AllocSizeX = FMath::Max(AllocPosX + MeshAllocation.Value.X, AllocSizeX);

				AllocPosX += MeshAllocation.Value.X;
			}

			// Write textures to rendertarget
			const int32 TotalWidth = AllocSizeX * UnitX;
			const int32 TotalHeight = AllocSizeY * UnitY;
			const FVector2D TotSize = FVector2D(TotalWidth, TotalHeight);
			UTextureRenderTarget2D* RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, TotalWidth, TotalHeight, CanvasPair.Key->Format, FLinearColor::Black, false);

			for (const auto& PlacementPair : Placements)
			{
				const int32 MeshIndex = PlacementPair.Key.X;
				const int32 SectionIndex = PlacementPair.Key.Y;
				const int32 NodeIndex = PlacementPair.Key.Z;

				UTGOR_Section* Section = Sections[SectionIndex];
				FTGOR_CustomisationTextureAllocation& Allocation = CustomisationEntries[NodeIndex].Sections[Section].Textures[CanvasPair.Key];

				const int32 PosX = PlacementPair.Value.PosX * UnitX;
				const int32 PosY = PlacementPair.Value.PosY * UnitY;
				Allocation.Pos = FVector2D(PosX, PosY);

				const int32 Width = PlacementPair.Value.SizeX * UnitX;
				const int32 Height = PlacementPair.Value.SizeY * UnitY;
				Allocation.Size = FVector2D(Width, Height);

				if (Section->TransformUV)
				{
					const int32 MaterialIndex = CurrentAppearance.Bodyparts[NodeIndex].Content->Instanced_SectionInsertions.Collection[Section];
					UVTransforms.UVTransformsPerMesh[MeshIndex][MaterialIndex][0] = FTransform(FQuat::Identity, FVector(Allocation.Pos / TotSize, 0.0f), FVector(Allocation.Size / TotSize, 1.0f));
				}

				Output.Sections[SectionIndex].Textures.Emplace(CanvasPair.Key);
			}

			Output.Textures.Emplace(CanvasPair.Key, RenderTarget);
		}

		// Merge meshes into one
		FTGOR_SkeletalMeshMerge Merge(TargetMesh, Meshes, Mappings, 0, EMeshBufferAccess::Default, UVTransforms, LocalSpaceTransforms, TargetCache.RigBoneMapping);
		if (Merge.DoMerge())
		{
			TArray<FSkeletalMaterial>& MeshMaterials = TargetMesh->GetMaterials();

			// Create textures
			const int32 Num = Sections.Num();
			MeshMaterials.SetNum(Num);
			for (int32 Index = 0; Index < Num; Index++)
			{
				UTGOR_Section* Section = Sections[Index];
				UMaterialInstanceDynamic** MaterialPtr = CustomisationMaterials.Find(Section);
				if (MaterialPtr && *MaterialPtr)
				{
					for (UTGOR_Canvas* Canvas : Section->Instanced_CanvasInsertions.Collection)
					{
						(*MaterialPtr)->SetTextureParameterValue(Canvas->ParameterName, Output.Textures[Canvas]);
					}

					MeshMaterials[Index].MaterialInterface = (*MaterialPtr);
				}
			}
			Output.Mesh = TargetMesh;

			for (const auto& Pair : Output.Textures)
			{
				FTGOR_CustomisationMeshTexture MeshTexture;
				MeshTexture.Canvas = nullptr;
				MeshTexture.RenderTarget = Pair.Value;
				CustomisationTextures.Emplace(Pair.Key, MeshTexture);
			}
			/*
			// Merge customisations
			for (const auto& Pair : Resources.Customisations)
			{
				FTGOR_CustomisationInstance Instance;
				if (Pair.Key->MergePayloads(Pair.Value.List, Instance.Payload))
				{
					Instance.Content = Pair.Key;
					Output.Customisations.Emplace(Instance);
				}
			}
			*/
		}
	}
	return Output;
}



void UTGOR_CustomisationComponent::RuntimeExportMesh(const FTGOR_BodypartMergeOutput& Output, const FString& Filename)
{
#if WITH_EDITOR
	FTGOR_SkeletalMeshExport SkeletalExport(Output);
	SkeletalExport.Export(Filename);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////


int32 UTGOR_CustomisationComponent::CountCustomisationInstances(int32 NodeIndex, UTGOR_Customisation* Content) const
{
	if (!CurrentAppearance.Bodyparts.IsValidIndex(NodeIndex)) return 0;

	int32 Count = 0;
	const FTGOR_BodypartInstance& Instance = CurrentAppearance.Bodyparts[NodeIndex];
	if (IsValid(Instance.Content))
	{
		for (const FTGOR_CustomisationInstance& CustomisationInstance : Instance.CustomisationInstances)
		{
			if (CustomisationInstance.Content == Content)
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

void UTGOR_CustomisationComponent::ResetCustomisationInstance(int32 NodeIndex, UTGOR_Customisation* Content, int32 Index)
{
	if (!CurrentAppearance.Bodyparts.IsValidIndex(NodeIndex)) return;

	// Remove entry from material customisation
	FTGOR_BodypartInstance& Instance = CurrentAppearance.Bodyparts[NodeIndex];
	int32 Num = Instance.CustomisationInstances.Num();
	for (int i = 0; i < Num; i++)
	{
		FTGOR_CustomisationInstance& CustomisationInstance = Instance.CustomisationInstances[i];

		if (CustomisationInstance.Content == Content)
		{
			if (Index-- == 0)
			{
				Instance.CustomisationInstances.RemoveAt(i);
				break;
			}
		}
	}

	// Reapply customisation
	ApplyInternalCustomisation();

	UpdateAppearance(NodeIndex);
}

bool UTGOR_CustomisationComponent::QueryCustomisationInstance(int32 NodeIndex, FTGOR_CustomisationInstance& CustomisationInstance) const
{
	if (!CurrentAppearance.Bodyparts.IsValidIndex(NodeIndex)) return false;
	const FTGOR_BodypartInstance& Instance = CurrentAppearance.Bodyparts[NodeIndex];
	const FTGOR_CustomisationMeshEntry& Entry = CustomisationEntries[NodeIndex];
	if (IsValid(Instance.Content))
	{
		if (Instance.Content->Instanced_CustomisationInsertions.Collection.Contains(CustomisationInstance.Content))
		{
			CustomisationInstance.Payload = CustomisationInstance.Content->CreatePayload();

			// Call func on given instance
			UTGOR_Section* Section = CustomisationInstance.Content->Instanced_SectionInsertion.Collection;
			UMaterialInstanceDynamic* const* Cache = CustomisationMaterials.Find(Section);
			if (Cache != nullptr && IsValid((*Cache)))
			{
				CustomisationInstance.Content->Query(this, CustomisationInstance.Payload, Instance.Content, NodeIndex, *Cache, Entry.Sections[Section]);
			}
			return true;
		}
	}
	return false;
}

void UTGOR_CustomisationComponent::ApplyCustomisationInstance(int32 NodeIndex, const FTGOR_CustomisationInstance& CustomisationInstance, int32 Index)
{
	if (!CurrentAppearance.Bodyparts.IsValidIndex(NodeIndex)) return;

	// Check all current instances for matches to update
	FTGOR_BodypartInstance& Instance = CurrentAppearance.Bodyparts[NodeIndex];
	for (FTGOR_CustomisationInstance& OtherCustomisationInstance : Instance.CustomisationInstances)
	{
		if (OtherCustomisationInstance.Content == CustomisationInstance.Content)
		{
			if (Index-- == 0)
			{
				OtherCustomisationInstance = CustomisationInstance;
				ApplyInternalCustomisation();
				UpdateAppearance(NodeIndex);
				return;
			}
		}
	}

	// Add new instance if no match
	Instance.CustomisationInstances.Add(CustomisationInstance);
	ApplyInternalCustomisation();
	UpdateAppearance(NodeIndex);
}

void UTGOR_CustomisationComponent::ApplyInternalCustomisation()
{
	// Start drawing
	{
		SCOPE_CYCLE_COUNTER(STAT_CustomisationBeginDraw);

		// Clear materials
		UWorld* World = GetWorld();
		World->FlushDeferredParameterCollectionInstanceUpdates();

		for (auto& TexturePair : CustomisationTextures)
		{
			if (IsValid(TexturePair.Value.RenderTarget))
			{
				TexturePair.Value.Context = FDrawToRenderTargetContext();
				TexturePair.Value.Context.RenderTarget = TexturePair.Value.RenderTarget;
				TexturePair.Value.Context.DrawEvent = new FDrawEvent();

				FTextureRenderTargetResource* RenderTargetResource = TexturePair.Value.RenderTarget->GameThread_GetRenderTargetResource();
				TexturePair.Value.Canvas = new FCanvas(RenderTargetResource, nullptr, World, World->FeatureLevel, FCanvas::CDM_ImmediateDrawing);

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
			}
		}
	}

	// Apply customisations
	{
		SCOPE_CYCLE_COUNTER(STAT_CustomisationTotal);

		const int32 NodeNum = CurrentAppearance.Bodyparts.Num();
		for (int32 NodeIndex = 0; NodeIndex < NodeNum; NodeIndex++)
		{
			const FTGOR_BodypartInstance& Instance = CurrentAppearance.Bodyparts[NodeIndex];
			const FTGOR_CustomisationMeshEntry& Entry = CustomisationEntries[NodeIndex];
			if (!IsValid(Instance.Content) || !IsValid(Instance.Content->Mesh)) return;

			// Apply customisations in order
			TArray<UTGOR_Customisation*> CustomList = Instance.Content->Instanced_CustomisationInsertions.GetListOfType<UTGOR_Customisation>();
			for (UTGOR_Customisation* CustomEntry : CustomList)
			{
				SCOPE_CYCLE_COUNTER(STAT_CustomisationApply);

				UTGOR_Section* Section = CustomEntry->Instanced_SectionInsertion.Collection;
				UMaterialInstanceDynamic* const* Cache = CustomisationMaterials.Find(Section);
				if (Cache != nullptr && IsValid((*Cache)))
				{
					// Get instance information
					TArray<FTGOR_CustomisationInstance> Instances;
					if (GetCustomisationMatching(NodeIndex, Instances, CustomEntry, ETGOR_CustomisationQueryEnumeration::All))
					{
						// Actually apply the customisations
						for (const FTGOR_CustomisationInstance& CustomisationInstance : Instances)
						{
							CustomisationInstance.Content->Apply(this, CustomisationInstance.Payload, Instance.Content, NodeIndex, *Cache, Entry.Sections[Section]);
						}
					}
				}
			}
		}
	}

	// End drawing
	{
		SCOPE_CYCLE_COUNTER(STAT_CustomisationEndDraw);

		// Apply rendertargets
		for (auto& TexturePair : CustomisationTextures)
		{
			//UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, TexturePair.Value.Context);
			if (TexturePair.Value.Canvas)
			{
				TexturePair.Value.Canvas->Flush_GameThread();
				delete TexturePair.Value.Canvas;
				TexturePair.Value.Canvas = nullptr;
			}

			if (IsValid(TexturePair.Value.RenderTarget))
			{
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
			}

			// Remove references to the context now that we've resolved it, to avoid a crash when EndDrawCanvasToRenderTarget is called multiple times with the same context
			// const cast required, as BP will treat Context as an output without the const
			const_cast<FDrawToRenderTargetContext&>(TexturePair.Value.Context) = FDrawToRenderTargetContext();
		}
	}
}

bool UTGOR_CustomisationComponent::GetCustomisationMatching(int32 NodeIndex, TArray<FTGOR_CustomisationInstance>& CustomisationInstances, UTGOR_Customisation* Content, ETGOR_CustomisationQueryEnumeration Query) const
{
	if (!CurrentAppearance.Bodyparts.IsValidIndex(NodeIndex)) return false;

	// Ask locally if enabled
	const FTGOR_BodypartInstance& Instance = CurrentAppearance.Bodyparts[NodeIndex];
	if (IsValid(Instance.Content))
	{
		if (Instance.Content->Instanced_CustomisationInsertions.Collection.Contains(Content))
		{
			// Check all selected entries for matches
			for (const FTGOR_CustomisationInstance& OtherCustomisationInstance : Instance.CustomisationInstances)
			{
				if (OtherCustomisationInstance.Content == Content)
				{
					// Actually add instances for all or local only (but still return if not multiple since that means there is *no* inheritance)
					if (Query != ETGOR_CustomisationQueryEnumeration::Inherited)
					{
						CustomisationInstances.Emplace(OtherCustomisationInstance);
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
	const FTGOR_CustomisationMeshEntry& Entry = CustomisationEntries[NodeIndex];
	if (Entry.Parent != INDEX_NONE && Content->TrickleDown && Query >= ETGOR_CustomisationQueryEnumeration::All)
	{
		// TODO: Do parent layers go *before* child layers?
		// All queries are inherited from parent
		GetCustomisationMatching(Entry.Parent, CustomisationInstances, Content, ETGOR_CustomisationQueryEnumeration::All);
	}

	// Create empty customisation if mandatory
	if (Content->LayerType == ETGOR_LayerTypeEnumeration::Mandatory && CustomisationInstances.Num() == 0)
	{
		// Actually add instances for all or local only
		if (Query <= ETGOR_CustomisationQueryEnumeration::All)
		{
			FTGOR_CustomisationInstance NewCustomisationInstance;
			NewCustomisationInstance.Content = Content;
			NewCustomisationInstance.Payload = Content->CreatePayload();
			CustomisationInstances.Emplace(NewCustomisationInstance);
		}
		return true;
	}

	return CustomisationInstances.Num() > 0;
}

void UTGOR_CustomisationComponent::ChangeBodypart(int32 NodeIndex, UTGOR_Bodypart* Content)
{
	if (!CurrentAppearance.Bodyparts.IsValidIndex(NodeIndex)) return;

	if (!IsValid(Content))
	{
		ERROR(GetIdentification(NodeIndex) + ": Invalid bodypart index", Error)
	}

	FTGOR_BodypartInstance& Instance = CurrentAppearance.Bodyparts[NodeIndex];
	Instance.Content = Content;

	// Reconstruct the tree from this point
	ConstructTree(0);
}

void UTGOR_CustomisationComponent::RemoveChild(int32 NodeIndex, int32 ChildIndex)
{
	FTGOR_CustomisationMeshEntry& Entry = CustomisationEntries[NodeIndex];
	const int32 OldIndex = Entry.Children.Remove(ChildIndex);

	// Reconstruct the tree from parent
	UpdateAppearance(NodeIndex);
}

int32 UTGOR_CustomisationComponent::AddChildBodypart(int32 NodeIndex, UTGOR_Bodypart* Content)
{
	if (!CurrentAppearance.Bodyparts.IsValidIndex(NodeIndex)) return INDEX_NONE;
	const FTGOR_BodypartInstance& Instance = CurrentAppearance.Bodyparts[NodeIndex];

	// Create subpath
	FTGOR_BodypartInstance NewInstance;
	NewInstance.Depth = Instance.Depth + 1;
	NewInstance.Content = Content;

	const int32 NewNodeIndex = CurrentAppearance.Bodyparts.Insert(NewInstance, NodeIndex + 1);

	// Reconstruct the whole tree to refresh indices
	UpdateAppearance(0);

	return NewNodeIndex;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FString UTGOR_CustomisationComponent::GetIdentification(int32 NodeIndex) const
{
	// Suffix for all outputs
	FString Suffix = FString(" at depth ") + FString::FromInt(CurrentAppearance.Bodyparts[NodeIndex].Depth);

	// Take bodypart information if available
	UTGOR_Bodypart* Bodypart = CurrentAppearance.Bodyparts[NodeIndex].Content;
	if (IsValid(Bodypart))
	{
		return(Bodypart->GetDefaultName() + Suffix);
	}
	else
	{
		return(FString("Undefined bodypart") + Suffix);
	}
}

int32 UTGOR_CustomisationComponent::ConstructTree(int32 NodeIndex)
{
	// Check index to prevent empty nodes
	if (!CurrentAppearance.Bodyparts.IsValidIndex(NodeIndex))
	{
		ERRET(GetIdentification(NodeIndex) + ": Reached the end of the path too early", Error, 0);
	}

	if (NodeIndex >= MAX_BODYPART_DEPTH)
	{
		ERRET(GetIdentification(NodeIndex) + ": Too many bodyparts", Error, 0);
	}

	SINGLETON_RETCHK(0);

	UTGOR_Bodypart* Bodypart = CurrentAppearance.Bodyparts[NodeIndex].Content;
	for (const auto& Pair : Bodypart->Instanced_SectionInsertions.Collection)
	{
		if (IsValid(Pair.Key))
		{
			UMaterialInstanceDynamic*& Material = CustomisationMaterials.FindOrAdd(Pair.Key, nullptr);
			if (!IsValid(Material))
			{
				const TArray<FSkeletalMaterial>& MeshMaterials = Bodypart->Mesh->GetMaterials();
				if (IsValid(Pair.Key->Material))
				{
					Material = UMaterialInstanceDynamic::Create(Pair.Key->Material, this);
				}
				else if (MeshMaterials.IsValidIndex(Pair.Value))
				{
					Material = UMaterialInstanceDynamic::Create(MeshMaterials[Pair.Value].MaterialInterface, this);
				}
			}
		}
	}

	// Validate all customisations and remove them if necessary
	TMap<UTGOR_Customisation*, int32> ContentCount;
	for (auto Its = CurrentAppearance.Bodyparts[NodeIndex].CustomisationInstances.CreateIterator(); Its; Its++)
	{
		int32& Count = ContentCount.FindOrAdd(Its->Content);

		// Remove either if no instance exists or customisation is invalid
		UMaterialInstanceDynamic** Material = CustomisationMaterials.Find(Its->Content->Instanced_SectionInsertion.Collection);
		if (!Material || // Customisation not supported by this bodypart
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

	// Initialise Entry
	while (NodeIndex >= CustomisationEntries.Num())
	{
		CustomisationEntries.Add(FTGOR_CustomisationMeshEntry());
	}
	
	FTGOR_CustomisationMeshEntry& Entry = CustomisationEntries[NodeIndex];
	Entry.Sections.Empty();
	Entry.Children.Empty();

	if (IsValid(Bodypart))
	{
		UTGOR_BodypartUserData* UserData = Bodypart->Mesh->GetAssetUserData<UTGOR_BodypartUserData>();
		if (IsValid(UserData))
		{
			for (const auto& SectionPair : Bodypart->Instanced_SectionInsertions.Collection)
			{
				const FIntPoint Size = UserData->GetTextureSize(SectionPair.Value);

				// Allocate textures
				FTGOR_CustomisationMeshSection& Cache = Entry.Sections.FindOrAdd(SectionPair.Key);
				for (UTGOR_Canvas* Canvas : SectionPair.Key->Instanced_CanvasInsertions.Collection)
				{
					const int32 SizeX = FMath::Max(Size.X + Canvas->Scale.ScaleX, 1);
					const int32 SizeY = FMath::Max(Size.Y + Canvas->Scale.ScaleY, 1);
					Cache.Textures.FindOrAdd(Canvas).Allocation = FIntPoint(SizeX, SizeY);
				}
			}
		}
	}

	// Look for children and keep track of how many got added
	int32 Count = 1;
	const int32 ChildDepth = CurrentAppearance.Bodyparts[NodeIndex].Depth + 1;
	while (NodeIndex + Count < CurrentAppearance.Bodyparts.Num())
	{
		const int32 Child = NodeIndex + Count;
		const FTGOR_BodypartInstance& Next = CurrentAppearance.Bodyparts[Child];

		// Break once end of branch reached
		if (Next.Depth < ChildDepth)
		{
			break;
		}

		// Skip children (in case of invalid entry)
		if (Next.Depth > ChildDepth)
		{
			CurrentAppearance.Bodyparts.RemoveAt(Child);
			continue;
		}

		// Check whether child is allowed to be here
		if (CheckChildValidity(NodeIndex, Child))
		{
			CustomisationEntries[NodeIndex].Children.Emplace(Child);

			// Recursively construct subtree
			const int32 Constructed = ConstructTree(Child);
			Count += Constructed;

			CustomisationEntries[Child].Parent = NodeIndex;

			// Make sure counter doesn't get stuck
			if (Constructed == 0)
			{
				break;
			}
		}
		else
		{
			// Just remove this entry and go to next
			CurrentAppearance.Bodyparts.RemoveAt(Child);
		}
	}

	// Add nodes that are required
	if (IsValid(Bodypart))
	{
		for (const FTGOR_BodypartChild& Requirement : Bodypart->Children)
		{
			if (Requirement.Type == ETGOR_BodypartChildType::Required)
			{
				// Load required classes if doesn't exist yet
				const int32 Child = GetFirstChildFromType(NodeIndex, Requirement.Class);
				if (Child == INDEX_NONE)
				{
					// Create subpath
					FTGOR_BodypartInstance NewInstance;
					NewInstance.Depth = ChildDepth;
					NewInstance.Content = Modular->Instanced_BodypartInsertions.GetOfType(Requirement.Class);

					// Recursively construct subtree (children register themselves on parent)
					const int32 NewNodeIndex = CurrentAppearance.Bodyparts.Insert(NewInstance, NodeIndex + Count);
					Entry.Children.Emplace(NewNodeIndex);
					Count += ConstructTree(NewNodeIndex);
					CustomisationEntries[NewNodeIndex].Parent = NodeIndex;
				}
			}
			else if (Requirement.Type == ETGOR_BodypartChildType::Automatic)
			{
				/*
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
				*/
			}
		}
	}

	return Count;
}

bool UTGOR_CustomisationComponent::CheckChildValidity(int32 NodeIndex, int32 Child) const
{
	if (!CurrentAppearance.Bodyparts.IsValidIndex(NodeIndex)) return false;

	// Setup node
	UTGOR_Bodypart* Bodypart = CurrentAppearance.Bodyparts[NodeIndex].Content;
	UTGOR_Bodypart* ChildBodypart = CurrentAppearance.Bodyparts[Child].Content;
	if (IsValid(Bodypart) && IsValid(ChildBodypart))
	{
		if (!Modular->Instanced_BodypartInsertions.Contains(ChildBodypart))
		{
			ERRET(ChildBodypart->GetDefaultName() + " Bodypart not allowed in " + Modular->GetDefaultName(), Warning, false);
		}

		FTGOR_BodypartChild ChildType = GetRequirement(NodeIndex, ChildBodypart->GetClass());
		if (ChildType.Type == ETGOR_BodypartChildType::Automatic)
		{
			ERRET(ChildBodypart->GetDefaultName() + " is allocated for " + ChildType.Class->GetName() + " which is automatically set in " + Bodypart->GetDefaultName() + " and is not customisable", Warning, false);
		}

		// Check whether a previous child already contains this type
		const int32 OtherChild = GetFirstChildFromType(NodeIndex, ChildBodypart->GetClass());
		if (OtherChild != INDEX_NONE && OtherChild < Child)
		{
			ERRET(ChildBodypart->GetDefaultName() + " colliding with other " + ChildType.Class->GetName() + " type already present in " + Bodypart->GetDefaultName(), Warning, false);
		}
	}
	else
	{
		ERRET(GetIdentification(NodeIndex) + ": Bodypart not valid while building", Warning, false);
	}

	return true;
}

void UTGOR_CustomisationComponent::UpdateAppearance(int32 NodeIndex)
{
	ApplyAndCreateMerge(CurrentAppearance);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

UTGOR_Bodypart* UTGOR_CustomisationComponent::GetBodypart(int32 NodeIndex) const
{
	if (CurrentAppearance.Bodyparts.IsValidIndex(NodeIndex))
	{
		return CurrentAppearance.Bodyparts[NodeIndex].Content;
	}
	return nullptr;
}

bool UTGOR_CustomisationComponent::GetBounds(int32 NodeIndex, bool IncludeChildren, FVector& Center, float& Extend) const
{
	if (!CurrentAppearance.Bodyparts.IsValidIndex(NodeIndex)) return false;

	const FTGOR_BodypartInstance& Instance = CurrentAppearance.Bodyparts[NodeIndex];
	if (!IsValid(Instance.Content)) return false;

	UTGOR_BodypartUserData* UserData = Instance.Content->Mesh->GetAssetUserData<UTGOR_BodypartUserData>();
	if (!IsValid(UserData)) return false;

	int32 MaterialNum = GetNumMaterials();

	FVector MinPos = FVector(FLT_MAX);
	FVector MaxPos = FVector(-FLT_MAX);
	const FTransform& Transform = GetComponentTransform(); // TODO: This is not inaccurate, use bone instead

	// Recreate dynamic material instances
	for (const auto& Pair : Instance.Content->Instanced_SectionInsertions.Collection)
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
		const FTGOR_CustomisationMeshEntry& Entry = CustomisationEntries[NodeIndex];
		for (int32 Child : Entry.Children)
		{
			FVector ChildCenter;
			float ChildExtend;
			GetBounds(Child, true, ChildCenter, ChildExtend);

			MinPos = MinPos.ComponentMin(ChildCenter - FVector(ChildExtend));
			MaxPos = MaxPos.ComponentMax(ChildCenter + FVector(ChildExtend));
		}
	}

	Center = (MaxPos + MinPos) / 2;
	Extend = (MaxPos - MinPos).Size() / 2;
	return true;
}

int32 UTGOR_CustomisationComponent::GetParent(int32 NodeIndex) const
{
	return CustomisationEntries[NodeIndex].Parent;
}

int32 UTGOR_CustomisationComponent::GetChild(int32 NodeIndex, UTGOR_Bodypart* Content) const
{
	const FTGOR_CustomisationMeshEntry& Entry = CustomisationEntries[NodeIndex];
	for (int32 Child : Entry.Children)
	{
		if (CurrentAppearance.Bodyparts.IsValidIndex(Child))
		{
			UTGOR_Bodypart* Bodypart = CurrentAppearance.Bodyparts[Child].Content;
			if (Bodypart == Content)
			{
				return Child;
			}
		}
	}
	return INDEX_NONE;
}

int32 UTGOR_CustomisationComponent::GetFirstChildFromType(int32 NodeIndex, TSubclassOf<UTGOR_Bodypart> Class) const
{
	if (*Class)
	{
		const FTGOR_CustomisationMeshEntry& Entry = CustomisationEntries[NodeIndex];
		for (int32 Child : Entry.Children)
		{
			if (CurrentAppearance.Bodyparts.IsValidIndex(Child))
			{
				UTGOR_Bodypart* Bodypart = CurrentAppearance.Bodyparts[Child].Content;
				if (IsValid(Bodypart) && Bodypart->IsA(Class))
				{
					return Child;
				}
			}
		}
	}
	return INDEX_NONE;
}

FTGOR_BodypartChild UTGOR_CustomisationComponent::GetRequirement(int32 NodeIndex, TSubclassOf<UTGOR_Bodypart> Class) const
{
	if (CurrentAppearance.Bodyparts.IsValidIndex(NodeIndex))
	{
		// Check for matching requirement class
		UTGOR_Bodypart* Bodypart = CurrentAppearance.Bodyparts[NodeIndex].Content;
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
	}
	return FTGOR_BodypartChild();
}