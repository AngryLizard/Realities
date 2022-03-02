// The Gateway of Realities: Planes of Existence.


#include "TGOR_Dimension.h"

#include "Engine/Classes/Components/SplineComponent.h"
#include "Engine/SphereReflectionCapture.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/PlanarReflection.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"

#include "DimensionSystem/Data/TGOR_WorldData.h"
#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "CoreSystem/TGOR_Singleton.h"

#include "Engine.h"



UTGOR_Dimension::UTGOR_Dimension()
	: Super()
{
	Reality = false;
	Volatile = true;
	Lifeline = 25.0f;

	CosmeticTypes.Add(ASkyLight::StaticClass());
	CosmeticTypes.Add(ASphereReflectionCapture::StaticClass());
	CosmeticTypes.Add(AExponentialHeightFog::StaticClass());
	CosmeticTypes.Add(ADirectionalLight::StaticClass());
	CosmeticTypes.Add(APostProcessVolume::StaticClass());
	CosmeticTypes.Add(APlanarReflection::StaticClass());
}

bool UTGOR_Dimension::Validate_Implementation()
{
	if (World.IsNull())
	{
		ERRET("No world defined", Error, false);
	}

	return Super::Validate_Implementation();
}

bool UTGOR_Dimension::AllocateSpace(FVector& Location)
{
	SINGLETON_RETCHK(false);
	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{
		// Allocate space on database
		ETGOR_FetchEnumeration State = ETGOR_FetchEnumeration::Found;

		// Allocate minidimensions, spawn reality above ground
		const FVector BoundMax = FVector(Bounds.GetAbsMax());
		Location = Reality ? WorldData->AllocateRealDimension(BoundMax, State) : WorldData->AllocateMiniDimension(BoundMax, State);

		if (State == ETGOR_FetchEnumeration::Empty)
		{
			ERRET("No dimension space left", Error, false)
		}

		// Create dimension
		return true;
	}
	return false;
}

bool UTGOR_Dimension::DeallocateSpace(const FVector& Location)
{
	SINGLETON_RETCHK(false);
	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{

		ETGOR_FetchEnumeration State = ETGOR_FetchEnumeration::Found;
		if (Reality)
		{
			WorldData->DeallocateRealDimension(Location, State);
		}
		else
		{
			WorldData->DeallocateMiniDimension(Location, State);
		}

		if (State == ETGOR_FetchEnumeration::Empty)
		{
			ERRET("No allocation found", Error, false)
		}

		// Create dimension
		return true;
	}
	return false;
}


ULevelStreamingDynamic* UTGOR_Dimension::LoadLevelInstanceBySoftObjectPtr(UObject* WorldContextObject, const TSoftObjectPtr<UWorld> Level, const FString& UniqueId, const FVector Location, const FRotator Rotation, bool& bOutSuccess)
{
	bOutSuccess = false;
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	// Check whether requested map exists, this could be very slow if LevelName is a short package name
	if (Level.IsNull())
	{
		return nullptr;
	}

	return LoadLevelInstance_Internal(World, Level.GetLongPackageName(), UniqueId, Location, Rotation, bOutSuccess);
}

ULevelStreamingDynamic* UTGOR_Dimension::LoadLevelInstance_Internal(UWorld* World, const FString& LongPackageName, const FString& UniqueId, const FVector Location, const FRotator Rotation, bool& bOutSuccess)
{
	// Create Unique Name for sub-level package
	const FString ShortPackageName = FPackageName::GetShortName(LongPackageName);
	const FString PackagePath = FPackageName::GetLongPackagePath(LongPackageName);
	FString UniqueLevelPackageName = PackagePath + TEXT("/") + World->StreamingLevelsPrefix + ShortPackageName;
	UniqueLevelPackageName += TEXT("_LevelInstance_") + UniqueId;

	// Setup streaming level object that will load specified map
	ULevelStreamingDynamic* StreamingLevel = NewObject<ULevelStreamingDynamic>(World, ULevelStreamingDynamic::StaticClass(), NAME_None, RF_Transient, NULL);
	StreamingLevel->SetWorldAssetByPackageName(FName(*UniqueLevelPackageName));
	StreamingLevel->LevelColor = FColor::MakeRandomColor();
	StreamingLevel->SetShouldBeLoaded(true);
	StreamingLevel->SetShouldBeVisible(false);
	StreamingLevel->bShouldBlockOnLoad = false;
	StreamingLevel->bInitiallyLoaded = true;
	StreamingLevel->bInitiallyVisible = false;

	if (StreamingLevel->GetOutermost()->HasAnyPackageFlags(PKG_PlayInEditor) && StreamingLevel->GetOutermost()->GetPIEInstanceID() != INDEX_NONE)
	{
		StreamingLevel->RenameForPIE(StreamingLevel->GetOutermost()->GetPIEInstanceID());
	}

	// Transform
	StreamingLevel->LevelTransform = FTransform(Rotation, Location);
	// Map to Load
	StreamingLevel->PackageNameToLoad = FName(*LongPackageName);

	// Add the new level to world.
	World->AddStreamingLevel(StreamingLevel);

	bOutSuccess = true;
	return StreamingLevel;
}


UTGOR_DimensionData* UTGOR_Dimension::Create(const FString& UniqueId, const FVector& Location)
{
	SINGLETON_RETCHK(nullptr);
	UTGOR_WorldData* WorldData = Singleton->GetData<UTGOR_WorldData>();
	if (IsValid(WorldData))
	{
		// Create level
		bool Success = false;
		ULevelStreamingDynamic* Streaming = UTGOR_Dimension::LoadLevelInstanceBySoftObjectPtr(WorldData, World, UniqueId, Location, FRotator::ZeroRotator, Success);

		if (!Success)
		{
			ERRET("Loading level failed", Error, nullptr)
		}

		// Create and assign dimension
		UTGOR_DimensionData* Dimension = NewObject<UTGOR_DimensionData>(WorldData);
		Dimension->LevelStreaming = Streaming;
		Dimension->Origin = Location + Offset;

		return Dimension;
	}
	return nullptr;
}

bool UTGOR_Dimension::Load(UTGOR_DimensionData* Dimension, const FString& Path, const TArray<int32>& Options)
{
	SINGLETON_RETCHK(false);
	if (!IsValid(Dimension->LoadedLevel))
	{
		if (IsValid(Dimension->LevelStreaming))
		{
			ERRET("Level not loaded", Error, false)
		}
		else
		{
			ERRET("No streaming nor persistent level loaded", Error, false)
		}
	}

	PreAssemble(Dimension);

	// Assemble dimension
	Dimension->AssembleDimension();
	Assemble(Dimension);

	// Assemble all actors (and regions)
	Dimension->AssembleActors();

	// Load dimension from file
	Dimension->SetFilename(Path);
	Dimension->LoadFromFile();

	// Wrap up loading
	PostAssemble(Dimension);
	return(true);
}


void UTGOR_Dimension::SetCosmeticHidden(UTGOR_DimensionData* Dimension, bool Hidden)
{
	// Get level either from data or stream (Persistent dimension doesn't have LevelStreaming)
	ULevel* Level = (Dimension->LoadedLevel ? Dimension->LoadedLevel : (Dimension->LevelStreaming ? Dimension->LevelStreaming->GetLoadedLevel() : nullptr));
	if (IsValid(Level))
	{
		auto Iter = Level->Actors.CreateIterator();
		for (; Iter; ++Iter)
		{
			AActor* Actor = *Iter;
			if (IsValid(Actor))
			{
				// Look for matching class
				for (TSubclassOf<AActor> Class : CosmeticTypes)
				{
					if (*Class && Actor->IsA(Class))
					{
						const ETGOR_CosmeticInstruction Instr = OnCosmeticChange(Actor, Hidden);
						if (Instr == ETGOR_CosmeticInstruction::ShowHide)
						{
							if (!Actor->GetIsReplicated())
							{
								Actor->SetActorHiddenInGame(Hidden);
							}
						}
						else if (Hidden && Instr == ETGOR_CosmeticInstruction::Remove)
						{
							Iter.RemoveCurrent();
						}
						break;
					}
				}
			}
		}
	}
}

void UTGOR_Dimension::SetCullDistance(AActor* Actor) const
{

	for (TSubclassOf<AActor> Class : CosmeticTypes)
	{
		if (Actor->IsA(Class))
		{
			return;
		}
	}

	const float CullDistance = Bounds.Size() * 0.5f;

	TArray<UPrimitiveComponent*> Primitives;
	Actor->GetComponents(Primitives);
	for (UPrimitiveComponent* Component : Primitives)
	{
		if (IsValid(Component) && Component->CachedMaxDrawDistance < SMALL_NUMBER || CullDistance < Component->CachedMaxDrawDistance)
		{
			Component->SetCullDistance(CullDistance);
		}
	}

}