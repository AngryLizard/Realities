// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreSystem/Content/TGOR_CoreContent.h"
#include "TGOR_Overlay.generated.h"

class UTGOR_Menu;

/**
 * 
 */
UCLASS()
class UISYSTEM_API UTGOR_Overlay : public UTGOR_CoreContent
{
	GENERATED_BODY()
	
public:
	UTGOR_Overlay();
	virtual bool Validate_Implementation() override;

	/** Spawns and initialises this menu */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Interface", Meta = (Keywords = "C++"))
		UTGOR_Menu* InitialiseMenu();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "!TGOR Interface")
		TSubclassOf<UTGOR_Menu> Menu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interface")
		bool StartOpen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interface")
		int32 Layer;
};
