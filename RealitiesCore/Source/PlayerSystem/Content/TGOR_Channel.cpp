// The Gateway of Realities: Planes of Existence.

#include "TGOR_Channel.h"

#include "DimensionSystem/Data/TGOR_DimensionData.h"
#include "PlayerSystem/Gameplay/TGOR_OnlineController.h"
#include "CreatureSystem/Actors/TGOR_Pawn.h"

#include "DimensionSystem/Regions/Components/TGOR_RegionComponent.h"
#include "DimensionSystem/Regions/TGOR_MainRegionActor.h"

#define LOCTEXT_NAMESPACE "ChatChannels"

UTGOR_Channel::UTGOR_Channel()
	: Super()
{
	ChannelColor.A = 1.0f;
	HelpText = LOCTEXT("ChatChannels_NoHelp", "No Manual has been written for this channel.");
}

FLinearColor UTGOR_Channel::GetChannelColor() const
{
	return ChannelColor;
}


TArray<int32> UTGOR_Channel::GetPlayersInChannel_Implementation(APawn* Pawn, ATGOR_OnlineController* Controller, const FTGOR_UserInstance& UserInstance)
{
	return(ForAllPlayers(UserInstance));
}

TArray<int32> UTGOR_Channel::ForAllPlayers(const FTGOR_UserInstance& UserInstance)
{
	TArray<int32> Keys;

	ForEachPlayer([&UserInstance, &Keys, this](APawn* Pawn, ATGOR_OnlineController* Controller)->bool
	{
		if (QueryPlayerInChannel(Pawn, Controller, UserInstance))
		{
			Keys.Add(Controller->GetActiveUserKey());
		}
		return(true);
	});

	return(Keys);
}

void UTGOR_Channel::ForEachPlayer(std::function<bool(APawn* Pawn, ATGOR_OnlineController* Controller)> func)
{
	// Find playercontroller with specified user name
	UWorld* World = GetWorld();
	auto Iterator = World->GetPlayerControllerIterator();
	for (; Iterator; Iterator++)
	{
		ATGOR_OnlineController* Controller = Cast<ATGOR_OnlineController>(*Iterator);
		if (IsValid(Controller))
		{
			// Find all pawns nearby
			APawn* Pawn = Controller->GetPawn();
			if (IsValid(Pawn))
			{
				if (!func(Pawn, Controller))
				{
					return;
				}
			}
		}
	}
}

TArray<int32> UTGOR_Channel::GetPlayersInDimension(AActor* Actor, ATGOR_OnlineController* Controller)
{
	TArray<int32> Keys;

	// Get actor's component
	const UTGOR_RegionComponent* OwnComponent = Cast<UTGOR_RegionComponent>(Actor->GetComponentByClass(UTGOR_RegionComponent::StaticClass()));
	if (IsValid(OwnComponent))
	{
		const ATGOR_RegionActor* Current = OwnComponent->GetCurrentRegion();
		const ATGOR_MainRegionActor* MainRegion = OwnComponent->GetMainRegion();
		if (IsValid(Current) && IsValid(MainRegion))
		{
			// Get all players in dimension
			ForEachPlayer([&Keys, MainRegion](APawn* Pawn, ATGOR_OnlineController* Controller)->bool
			{
				UTGOR_RegionComponent* TheirComponent = Cast<UTGOR_RegionComponent>(Pawn->GetComponentByClass(UTGOR_RegionComponent::StaticClass()));
				if (IsValid(TheirComponent) && MainRegion == TheirComponent->GetMainRegion())
				{
					Keys.Add(Controller->GetActiveUserKey());
				}
				return(true);
			});
		}
	}

	return(Keys);
}


TArray<int32> UTGOR_Channel::GetPlayersInRadius(AActor* Actor, ATGOR_OnlineController* Controller, float Radius)
{
	TArray<int32> Keys;

	// Get location to get radius from
	const FVector Location = Actor->GetActorLocation();
	const float RR = Radius * Radius;

	// Get all players in radius
	ForEachPlayer([&Keys, &Location, RR](APawn* Pawn, ATGOR_OnlineController* Controller)->bool
	{
		FVector Other = Pawn->GetActorLocation();
		if (FVector::DistSquared(Location, Other) < RR)
		{
			Keys.Add(Controller->GetActiveUserKey());
		}
		return(true);
	});

	return(Keys);
}
#undef LOCTEXT_NAMESPACE