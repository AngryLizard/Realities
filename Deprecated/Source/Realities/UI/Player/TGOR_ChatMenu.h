// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Base/Controller/TGOR_OnlineController.h"
#include "Realities/Base/Instances/Player/TGOR_UserInstance.h"

#include "Realities/UI/TGOR_Menu.h"
#include "TGOR_ChatMenu.generated.h"


/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_ChatMenu : public UTGOR_Menu
{
	GENERATED_BODY()
	
public:

	/** Receive a message */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "TGOR Chat", Meta = (Keywords = "C++"))
		void ReceiveMessage(ATGOR_OnlineController* Controller, const FTGOR_ChatMessageInstance& Message);

	
};
