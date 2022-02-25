// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Realities/Base/Instances/Player/TGOR_UserInstance.h"

#include "Realities/Base/Content/TGOR_Content.h"
#include "TGOR_Channel.generated.h"

class ATGOR_MainRegionActor;
class ATGOR_OnlineController;

/**
 * 
 */

UCLASS(Blueprintable)
class REALITIES_API UTGOR_Channel : public UTGOR_Content
{
	GENERATED_BODY()
	
public:
	UTGOR_Channel();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Channel display colour */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Chat")
		FLinearColor ChannelColour;

	/** Manual text to be displayed on help */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TGOR Chat")
		FText HelpText;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get channel colour */
	UFUNCTION(BlueprintPure, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		FLinearColor GetChannelColour() const;

	/** Returns keys of players in this channel */
	UFUNCTION(BlueprintNativeEvent, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		TArray<int32> GetPlayersInChannel(ATGOR_Pawn* Pawn, ATGOR_OnlineController* Controller, const FTGOR_UserInstance& UserInstance);
		TArray<int32> GetPlayersInChannel_Implementation(ATGOR_Pawn* Pawn, ATGOR_OnlineController* Controller, const FTGOR_UserInstance& UserInstance);

	/** Returns true if player is part of channel */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		bool QueryPlayerInChannel(APawn* Pawn, ATGOR_OnlineController* Controller, const FTGOR_UserInstance& UserInstance);

	/** Iterates through all players */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		TArray<int32> ForAllPlayers(const FTGOR_UserInstance& UserInstance);
		void ForEachPlayer(std::function<bool(APawn* Pawn, ATGOR_OnlineController* Controller)> func);

	/** Get player in same dimension as an actor */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		TArray<int32> GetPlayersInDimension(AActor* Actor, ATGOR_OnlineController* Controller);

	/** Get nearby player keys */
	UFUNCTION(BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		TArray<int32> GetPlayersInRadius(AActor* Actor, ATGOR_OnlineController* Controller, float Radius);
};
