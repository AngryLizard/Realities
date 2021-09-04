// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "PlayerSystem/Gameplay/TGOR_OnlineController.h"
#include "PlayerSystem/TGOR_UserInstance.h"

#include "UISystem/UI/TGOR_Menu.h"
#include "TGOR_ChatMenu.generated.h"


/**
 * 
 */
UCLASS()
class PLAYERSYSTEM_API UTGOR_ChatMenu : public UTGOR_Menu
{
	GENERATED_BODY()
	
public:

	/** Receive a message */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "!TGOR Chat", Meta = (Keywords = "C++"))
		void ReceiveMessage(ATGOR_OnlineController* Controller, const FTGOR_ChatMessageInstance& Message);

	
};
