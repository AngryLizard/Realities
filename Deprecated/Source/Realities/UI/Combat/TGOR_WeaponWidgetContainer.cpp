// The Gateway of Realities: Planes of Existence.


#include "TGOR_WeaponWidgetContainer.h"

#include "Realities/UI/Combat/TGOR_WeaponWidget.h"
#include "Realities/Components/System/Actions/TGOR_ActionComponent.h"
#include "Realities/Actors/Pawns/TGOR_Pawn.h"

UTGOR_WeaponWidgetContainer::UTGOR_WeaponWidgetContainer()
{
	WeaponWidget = nullptr;

}

void UTGOR_WeaponWidgetContainer::UpdateWeaponWidget(ATGOR_Pawn* Pawn, APlayerController* Controller)
{
	/*
	UTGOR_EquipableComponent* Component = Character->EquipableComponent;

	if (CurrentWeapon != Component->EquippedWeapon.Weapon || CurrentAugment != Component->EquippedWeapon.Augment)
	{
		CurrentWeapon = Component->EquippedWeapon.Weapon;
		CurrentAugment = Component->EquippedWeapon.Augment;
		ChangeWeaponWidget(CurrentWeapon, Controller);
		
		OnEquipmentChanged.Broadcast(Component);
	}

	if (CurrentUseable != Component->EquippedUseable.Useable)
	{
		CurrentUseable = Component->EquippedUseable.Useable;
		OnEquipmentChanged.Broadcast(Component);
	}

	if (IsValid(WeaponWidget))
	{
		WeaponWidget->Update(Component, Character->VitalsComponent);
	}
	*/
}

/*
void UTGOR_WeaponWidgetContainer::ChangeWeaponWidget(UTGOR_Weapon* Weapon, APlayerController* Controller)
{
	UCanvasPanel* Canvas = Cast<UCanvasPanel>(GetRootWidget());
	if (Canvas == nullptr) return;

	if (IsValid(WeaponWidget))
	{
		if (IsValid(Weapon) && (*Weapon->WeaponWidgetClass) == WeaponWidget->GetClass())
		{
			return;
		}

		Canvas->RemoveChild(WeaponWidget);
		WeaponWidget = nullptr;
	}

	if (IsValid(Weapon) && (*Weapon->WeaponWidgetClass) != nullptr)
	{
		WeaponWidget = CreateWidget<UTGOR_WeaponWidget>(Controller, *Weapon->WeaponWidgetClass);

		// Add slot to canvas
		UCanvasPanelSlot* PanelSlot = Canvas->AddChildToCanvas(WeaponWidget);
		if (PanelSlot == nullptr)
		{
			ERROR("Couldn't create panelslot", Error)
		}
		FVector2D Size = GetWidgetSize();

		WeaponWidget->SetWidgetSize(Size);
	}
}

*/
