// The Gateway of Realities: Planes of Existence.


#include "TGOR_Loader.h"

#include "CoreSystem/Gameplay/TGOR_GameInstance.h"


#if WITH_EDITOR
#include "UnrealEd.h"
#endif // WITH_EDITOR


UTGOR_Loader::UTGOR_Loader()
	: Optional(false)
{
}

bool UTGOR_Loader::CanLoad(UWorld* World, const FTransform& Origin, UTGOR_Dimension* Dimension) const
{
	return true;
}

bool UTGOR_Loader::Load(UWorld* World, const FTransform& Origin, UTGOR_Dimension* Dimension)
{
	return true;
}

bool UTGOR_Loader::IsLoaded(UWorld* World, const FTransform& Origin, UTGOR_Dimension* Dimension) const
{
	return false;
}

bool UTGOR_Loader::Unload(UWorld* World, const FTransform& Origin, UTGOR_Dimension* Dimension)
{
	return true;
}


/*
UTGOR_Loader::UTGOR_Loader()
	: Super()
{
	Optional = false;
}

bool UTGOR_Loader::CanLoad_Implementation()
{
	return(true);
}

void UTGOR_Loader::Load(UWorld* World, bool Post)
{
	if (!CanLoad()) return;

	int32 Num = Spawns.Num();
	for (int i = 0; i < Num; i++)
	{
		ETGOR_LoaderType Type = Spawns[i].Type;
		if (Post)
		{
			if (Type == ETGOR_LoaderType::Level || Type == ETGOR_LoaderType::Actor)
			{
				Load(DefaultMod, World, i);
				PostInit();
			}
		}
		else
		{
			if (Type == ETGOR_LoaderType::RemoveByTag || Type == ETGOR_LoaderType::RemoveByTransform)
			{
				Load(DefaultMod, World, i);
				PostAssemble();
			}
		}
	}
}

void UTGOR_Loader::Load(const FString& ModName, UWorld* World, int32 Index)
{
	if (IsLoaded(ModName, World, Index))
	{
		ERROR("Already loaded", Error)
	}

	int32 Num = Spawns.Num();
	if (Index < 0 || Num <= Index) return;
	FTGOR_LoaderSpawn& Spawn = Spawns[Index];

	if (Spawn.Type == ETGOR_LoaderType::Level)
	{
		ULevelStreaming* LevelStreaming = CreateLevelStreaming(World, Index);
		if (IsValid(LevelStreaming))
		{
			World->AddStreamingLevel(LevelStreaming);
			World->UpdateLevelStreaming();
		}
	}
	else if (Spawn.Type == ETGOR_LoaderType::Actor)
	{
		// Make sure class exists
		if (!IsValid(Spawn.Class))
		{
			ERROR(Spawn.Name + " not a class", Error)
		}
		if (!Spawn.Class->IsChildOf(AActor::StaticClass()))
		{
			ERROR(Spawn.Class->GetName() + " not an actor class", Error)
		}

		// Spawn actor (don't assemble yet, since loader get called before dimensions were initialised)
		FActorSpawnParameters Params;
		Params.bAllowDuringConstructionScript = true;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Params.bNoFail = true;
		AActor* Actor = World->SpawnActor<AActor>(Spawn.Class, Spawn.Transform, Params);

		// Set datacomponent correctly if available
		if (IsValid(Actor))
		{
			UActorComponent* ActorComponent = Actor->GetComponentByClass(UTGOR_IdentityComponent::StaticClass());
			UTGOR_IdentityComponent* IdentityComponent = Cast<UTGOR_IdentityComponent>(ActorComponent);
			if (IsValid(IdentityComponent))
			{
				IdentityComponent->ActorInstance.ActorPrefix = ModName;
				IdentityComponent->ActorInstance.ActorTag = Spawn.Identifier;
			}
			else
			{
				ERROR(Spawn.Class->GetName() + " no datacomponent found", Error)
			}
		}
		else
		{
			ERROR(Spawn.Class->GetName() + " couldn't spawn actor", Error)
		}
	}
	else if (Spawn.Type == ETGOR_LoaderType::RemoveByTag)
	{
		for (TActorIterator<AActor> a(World); a; ++a)
		{
			TArray<UActorComponent*> Components = a->GetComponentsByClass(UTGOR_IdentityComponent::StaticClass());
			for (int32 i = 0; i < Components.Num(); i++)
			{
				UTGOR_IdentityComponent* Component = Cast<UTGOR_IdentityComponent>(Components[i]);
				if (IsValid(Component))
				{
					FString Tag = Spawn.Identifier;
					if (UTGOR_IdentityComponent::Compare(Component->ActorInstance, Spawn.Name, Tag))
					{
						#if WITH_EDITOR
							if (World->IsPlayInEditor())
							{
								World->DestroyActor(*a);
							}
							else
							{
								a->SetIsTemporarilyHiddenInEditor(true);
							}
						#else
							World->DestroyActor(*a);
						#endif
						return;
					}
				}
			}
		}
	}
	else if (Spawn.Type == ETGOR_LoaderType::RemoveByTransform)
	{
		for (TActorIterator<AActor> a(World); a; ++a)
		{
			if (a->GetTransform().Equals(Spawn.Transform, SMALL_NUMBER))
			{
				if (IsValid(Spawn.Class))
				{
					if (a->IsA(Spawn.Class))
					{
						#if WITH_EDITOR
							if (World->IsPlayInEditor())
							{
								World->DestroyActor(*a);
							}
							else
							{
								a->SetIsTemporarilyHiddenInEditor(true);
							}
						#else
							World->DestroyActor(*a);
						#endif
						return;
					}
				}
			}
		}
	}
}

bool UTGOR_Loader::IsLoaded(const FString& ModName, UWorld* World, int32 Index)
{
	int32 Num = Spawns.Num();
	if (Index < 0 || Num <= Index) return(false);
	FTGOR_LoaderSpawn& Spawn = Spawns[Index];

	if (Spawn.Type == ETGOR_LoaderType::Level)
	{
		FName Identifier = FName(*Spawn.Identifier);
		ULevelStreaming* LevelStreaming = World->GetLevelStreamingForPackageName(Identifier);
		return(IsValid(LevelStreaming));
	}
	else if (Spawn.Type == ETGOR_LoaderType::Actor)
	{
		for (TActorIterator<AActor> a(World); a; ++a)
		{
			TArray<UActorComponent*> Components = a->GetComponentsByClass(UTGOR_IdentityComponent::StaticClass());
			for (int32 i = 0; i < Components.Num(); i++)
			{
				UTGOR_IdentityComponent* Component = Cast<UTGOR_IdentityComponent>(Components[i]);
				if (IsValid(Component))
				{
					FString Tag = Spawn.Identifier;
					if (UTGOR_IdentityComponent::Compare(Component->ActorInstance, ModName, Tag))
					{
						return(true);
					}
				}
			}
		}
		return(false);
	}
	else if (Spawn.Type == ETGOR_LoaderType::RemoveByTag)
	{
		for (TActorIterator<AActor> a(World); a; ++a)
		{
			TArray<UActorComponent*> Components = a->GetComponentsByClass(UTGOR_IdentityComponent::StaticClass());
			for (int32 i = 0; i < Components.Num(); i++)
			{
				UTGOR_IdentityComponent* Component = Cast<UTGOR_IdentityComponent>(Components[i]);
				if (IsValid(Component))
				{
					FString Tag = Spawn.Identifier;
					if (UTGOR_IdentityComponent::Compare(Component->ActorInstance, Spawn.Name, Tag))
					{
						#if WITH_EDITOR
						return(a->IsHiddenEd());
						#endif
					}
				}
			}
		}
		return(true);
	}
	else if (Spawn.Type == ETGOR_LoaderType::RemoveByTransform)
	{
		for (TActorIterator<AActor> a(World); a; ++a)
		{
			if (a->GetTransform().Equals(Spawn.Transform, SMALL_NUMBER))
			{
				if (IsValid(Spawn.Class))
				{
					if (a->IsA(Spawn.Class))
					{
						#if WITH_EDITOR
						return(a->IsHiddenEd());
						#endif
					}
				}
			}
		}
		return(true);
	}
	return(false);
}


void UTGOR_Loader::Unload(const FString& ModName, UWorld* World, int32 Index)
{
	int32 Num = Spawns.Num();
	if (Index < 0 || Num <= Index) return;
	FTGOR_LoaderSpawn& Spawn = Spawns[Index];

	if (Spawn.Type == ETGOR_LoaderType::Level)
	{
		FName Identifier = FName(*Spawn.Identifier);
		ULevelStreaming* LevelStreaming = World->GetLevelStreamingForPackageName(Identifier);
		if (IsValid(LevelStreaming))
		{
			LevelStreaming->SetIsRequestingUnloadAndRemoval(true);
			World->UpdateLevelStreaming();
		}
	}
	else if (Spawn.Type == ETGOR_LoaderType::Actor)
	{
		for (TActorIterator<AActor> a(World); a; ++a)
		{
			TArray<UActorComponent*> Components = a->GetComponentsByClass(UTGOR_IdentityComponent::StaticClass());
			for (int32 i = 0; i < Components.Num(); i++)
			{
				UTGOR_IdentityComponent* Component = Cast<UTGOR_IdentityComponent>(Components[i]);
				if (IsValid(Component))
				{
					FString Tag = Spawn.Identifier;
					if (UTGOR_IdentityComponent::Compare(Component->ActorInstance, ModName, Tag))
					{
						World->DestroyActor(*a);
						return;
					}
				}
			}
		}
	}
	else if (Spawn.Type == ETGOR_LoaderType::RemoveByTag)
	{
		for (TActorIterator<AActor> a(World); a; ++a)
		{
			TArray<UActorComponent*> Components = a->GetComponentsByClass(UTGOR_IdentityComponent::StaticClass());
			for (int32 i = 0; i < Components.Num(); i++)
			{
				UTGOR_IdentityComponent* Component = Cast<UTGOR_IdentityComponent>(Components[i]);
				if (IsValid(Component))
				{
					FString Tag = Spawn.Identifier;
					if (UTGOR_IdentityComponent::Compare(Component->ActorInstance, Spawn.Name, Tag))
					{
						#if WITH_EDITOR
							a->SetIsTemporarilyHiddenInEditor(false);
						#endif
						return;
					}
				}
			}
		}
	}
	else if (Spawn.Type == ETGOR_LoaderType::RemoveByTransform)
	{
		for (TActorIterator<AActor> a(World); a; ++a)
		{
			if (a->GetTransform().Equals(Spawn.Transform, SMALL_NUMBER))
			{
				if (IsValid(Spawn.Class))
				{
					if (a->IsA(Spawn.Class))
					{
						#if WITH_EDITOR
							a->SetIsTemporarilyHiddenInEditor(false);
						#endif
						return;
					}
				}
			}
		}
	}
}

void UTGOR_Loader::Move(const FString& ModName, UWorld* World, int32 Index, FTransform Transform)
{
	int32 Num = Spawns.Num();
	if (Index < 0 || Num <= Index) return;
	FTGOR_LoaderSpawn& Spawn = Spawns[Index];

	if (Spawn.Type == ETGOR_LoaderType::Level)
	{
		FName Identifier = FName(*Spawn.Identifier);
		ULevelStreaming* LevelStreaming = World->GetLevelStreamingForPackageName(Identifier);
		if (IsValid(LevelStreaming))
		{
			ULevel *Level = LevelStreaming->GetLoadedLevel();
			if (!Level) return;

			FVector Old = LevelStreaming->LevelTransform.GetLocation();
			FVector Delta = Transform.GetLocation() - Old;

			Level->ApplyWorldOffset(Delta, false);
			World->UpdateLevelStreaming();
		}
	}
	else if (Spawn.Type == ETGOR_LoaderType::Actor)
	{
		for (TActorIterator<AActor> a(World); a; ++a)
		{
			TArray<UActorComponent*> Components = a->GetComponentsByClass(UTGOR_IdentityComponent::StaticClass());
			for (int32 i = 0; i < Components.Num(); i++)
			{
				UTGOR_IdentityComponent* Component = Cast<UTGOR_IdentityComponent>(Components[i]);
				if (IsValid(Component))
				{
					FString Tag = Spawn.Identifier;
					if (UTGOR_IdentityComponent::Compare(Component->ActorInstance, ModName, Tag))
					{
						a->SetActorTransform(Transform);
						return;
					}
				}
			}
		}
	}
	else if (Spawn.Type == ETGOR_LoaderType::RemoveByTag)
	{

	}
	else if (Spawn.Type == ETGOR_LoaderType::RemoveByTransform)
	{

	}
}

FTransform UTGOR_Loader::GetTransform(const FString& ModName, UWorld* World, int32 Index)
{
	int32 Num = Spawns.Num();
	if (Index < 0 || Num <= Index) return(FTransform());
	FTGOR_LoaderSpawn& Spawn = Spawns[Index];

	if (Spawn.Type == ETGOR_LoaderType::Level)
	{
		FName Identifier = FName(*Spawn.Identifier);
		ULevelStreaming* LevelStreaming = World->GetLevelStreamingForPackageName(Identifier);
		if (IsValid(LevelStreaming))
		{
			return(LevelStreaming->LevelTransform);
		}
	}
	else if (Spawn.Type == ETGOR_LoaderType::Actor)
	{
		for (TActorIterator<AActor> a(World); a; ++a)
		{
			TArray<UActorComponent*> Components = a->GetComponentsByClass(UTGOR_IdentityComponent::StaticClass());
			for (int32 i = 0; i < Components.Num(); i++)
			{
				UTGOR_IdentityComponent* Component = Cast<UTGOR_IdentityComponent>(Components[i]);
				if (IsValid(Component))
				{
					FString Tag = Spawn.Identifier;
					if (UTGOR_IdentityComponent::Compare(Component->ActorInstance, ModName, Tag))
					{
						return(a->GetTransform());
					}
				}
			}
		}
	}
	else if (Spawn.Type == ETGOR_LoaderType::RemoveByTag)
	{
		return(FTransform());
	}
	else if (Spawn.Type == ETGOR_LoaderType::RemoveByTransform)
	{
		return(FTransform());
	}

	return(FTransform());
}

ULevelStreaming* UTGOR_Loader::CreateLevelStreaming(UWorld* World, int32 Index)
{
	int32 Num = Spawns.Num();
	if (Index < 0 || Num <= Index) return(nullptr);
	FTGOR_LoaderSpawn& Spawn = Spawns[Index];

	ULevelStreamingDynamic* StreamingLevel = NewObject<ULevelStreamingDynamic>(World, ULevelStreamingDynamic::StaticClass());

	// Associate a package name.
	FName Identifier = FName(*Spawn.Identifier);
	StreamingLevel->SetWorldAssetByPackageName(Identifier); // Spawn.Identifier
	if (World->IsPlayInEditor())
	{
		FWorldContext WorldContext = GEngine->GetWorldContextFromWorldChecked(World);
		StreamingLevel->RenameForPIE(WorldContext.PIEInstance);
	}

	StreamingLevel->LevelColor = FColor::MakeRandomColor();
	//StreamingLevel->SetShouldBeLoaded(true);
	StreamingLevel->SetShouldBeVisible(true);
	StreamingLevel->bShouldBlockOnLoad = false;
	//StreamingLevel->bInitiallyLoaded = true;
	//StreamingLevel->bInitiallyVisible = true;

	StreamingLevel->LevelTransform = Spawn.Transform;
	StreamingLevel->PackageNameToLoad = Identifier;

	if (!FPackageName::DoesPackageExist(StreamingLevel->PackageNameToLoad.ToString(), NULL, &Spawn.Name))
	{
		ERRET("Package does not exists", Error, nullptr)
	}

	StreamingLevel->PackageNameToLoad = FName(*FPackageName::FilenameToLongPackageName(Spawn.Name));

	return(StreamingLevel);
}
*/