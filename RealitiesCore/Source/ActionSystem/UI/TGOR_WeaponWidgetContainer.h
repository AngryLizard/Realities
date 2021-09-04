// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "UISystem/UI/TGOR_Widget.h"
#include "TGOR_WeaponWidgetContainer.generated.h"

class UTGOR_EquipableAction;
class UTGOR_WeaponWidget;

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEquipmentChangedDelegate, UTGOR_EquipableComponent* , EquipableComponent);

/**
* TGOR_WeaponWidgetContainer holds WeaponWidgets
*/
UCLASS()
class ACTIONSYSTEM_API UTGOR_WeaponWidgetContainer : public UTGOR_Widget
{
	GENERATED_BODY()
	
public:
	UTGOR_WeaponWidgetContainer();

	/** Updates held WeaponWidget */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Combat", Meta = (Keywords = "C++"))
		void UpdateWeaponWidget(APawn* Pawn, APlayerController* Controller);

	/** Removes and adds new WeaponWidget */
	//UFUNCTION(BlueprintCallable, Category = "!TGOR Interface", Meta = (Keywords = "C++"))
	//	void ChangeWeaponWidget(UTGOR_Weapon* Weapon, APlayerController* Controller);

	/** Current weapon widget */
	UPROPERTY()
		UTGOR_WeaponWidget* WeaponWidget;

	////////////////////////////////////////////////////////////////////////////////////////////////////

//	UPROPERTY(BlueprintAssignable, Category = "!TGOR Combat")
//		FEquipmentChangedDelegate OnEquipmentChanged;

private:
//	UPROPERTY()
//		UTGOR_Weapon* CurrentWeapon;
	
	//UPROPERTY()
	//	UTGOR_WeaponItem* CurrentAugment;
	
	//UPROPERTY()
	//	UTGOR_EquipableAction* CurrentPersistentAction;
};
