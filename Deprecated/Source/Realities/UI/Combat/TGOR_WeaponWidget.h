// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/UI/TGOR_Widget.h"
#include "TGOR_WeaponWidget.generated.h"

class UTGOR_Augment;
class UTGOR_ActionComponent;
class UTGOR_CreatureVitalsComponent;


/**
* TGOR_WeaponWidget displays current weapon's widget
*/
UCLASS()
class REALITIES_API UTGOR_WeaponWidget : public UTGOR_Widget
{
	GENERATED_BODY()
	
public:
	
	/** Called when widget should update state */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "TGOR Combat", Meta = (Keywords = "C++"))
		void Update(UTGOR_ActionComponent* ActionComponent, UTGOR_EnergyComponent* EnergyComponent);
};
