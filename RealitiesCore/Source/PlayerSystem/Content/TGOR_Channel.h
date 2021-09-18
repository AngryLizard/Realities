// The Gateway of Realities: Planes of Existence.

#pragma once

#include "../TGOR_UserInstance.h"

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_Channel.generated.h"

class ATGOR_MainRegionActor;
class ATGOR_OnlineController;

/**
 * 
 */

UCLASS(Blueprintable)
class PLAYERSYSTEM_API UTGOR_Channel : public UTGOR_CoreContent
{
	GENERATED_BODY()
	
public:
	UTGOR_Channel();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Channel display color */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Chat")
		FLinearColor ChannelColor;

	/** Manual text to be displayed on help */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Chat")
		FText HelpText;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get channel color */
	UFUNCTION(BlueprintPure, Category = "!TGOR Chat", Meta = (Keywords = "C++"))
		FLinearColor GetChannelColor() const;

	/** Returns keys of players in this channel */
	UFUNCTION(BlueprintNativeEvent, Category = "!TGOR Chat", Meta = (Keywords = "C++"))
		TArray<int32> GetPlayersInChannel(APawn* Pawn, ATGOR_OnlineController* Controller, const FTGOR_UserInstance& UserInstance);
		TArray<int32> GetPlayersInChannel_Implementation(APawn* Pawn, ATGOR_OnlineController* Controller, const FTGOR_UserInstance& UserInstance);

	/** Returns true if player is part of channel */
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR Chat", Meta = (Keywords = "C++"))
		bool QueryPlayerInChannel(APawn* Pawn, ATGOR_OnlineController* Controller, const FTGOR_UserInstance& UserInstance);

	/** Iterates through all players */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Chat", Meta = (Keywords = "C++"))
		TArray<int32> ForAllPlayers(const FTGOR_UserInstance& UserInstance);
		void ForEachPlayer(std::function<bool(APawn* Pawn, ATGOR_OnlineController* Controller)> func);

	/** Get player in same dimension as an actor */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Chat", Meta = (Keywords = "C++"))
		TArray<int32> GetPlayersInDimension(AActor* Actor, ATGOR_OnlineController* Controller);

	/** Get nearby player keys */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Chat", Meta = (Keywords = "C++"))
		TArray<int32> GetPlayersInRadius(AActor* Actor, ATGOR_OnlineController* Controller, float Radius);
};
