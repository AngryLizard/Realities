// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Base/Instances/Combat/TGOR_AimInstance.h"
#include "Realities/Base/Instances/System/TGOR_IndexInstance.h"
#include "Realities/UI/Inventory/TGOR_TerminalTabMenu.h"

#include "Realities/Components/Dimension/Interactable/TGOR_WorldInteractableComponent.h"
#include "TGOR_TerminalComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FTerminalInteractDelegate, UTGOR_ActionStorage*, Storage, UTGOR_ActionComponent*, Component, FTGOR_SlotIndex, Slot);

/**
* 
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_TerminalComponent : public UTGOR_WorldInteractableComponent
{
	GENERATED_BODY()

public:	
	UTGOR_TerminalComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	UPROPERTY(BlueprintAssignable, Category = "TGOR Action")
		FTerminalInteractDelegate OnTerminalInteract;

	/** Paired tab menu class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Action")
		TSubclassOf<UTGOR_TerminalTabMenu> TabMenu;

	/** Additional filter on items to be displayed for the user */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Action")
		FTGOR_ItemRestriction SupportedItem;

	/** Called to get custom slot data assigned to a interacting action component. IsValid is automatically set by IsSupported after this call. */
	UFUNCTION(BlueprintPure, BlueprintImplementableEvent, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void GetTerminalSlotData(UTGOR_ActionComponent* Component, FTGOR_SlotIndex& Slot) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Interacts with this terminal. */
	UFUNCTION(BlueprintCallable, Category = "TGOR Action", Meta = (Keywords = "C++"))
		virtual void Interact(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component, const FTGOR_SlotIndex& Slot);

	/** Component event when this terminal is interacted with */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "TGOR Action", Meta = (Keywords = "C++"))
		void OnInteract(UTGOR_ActionStorage* Storage, UTGOR_ActionComponent* Component, const FTGOR_SlotIndex& Slot);

	/** Checks on the server whether this terminal supports given action component (TabMenu is still displayed but without "IsSupported" update). */
	UFUNCTION(BlueprintPure, BlueprintAuthorityOnly, Category = "TGOR Action", Meta = (Keywords = "C++"))
		virtual bool IsSupported(UTGOR_ActionComponent* Component) const;

	/** Gets slot data to send information from server to client */
	UFUNCTION(BlueprintPure, BlueprintAuthorityOnly, Category = "TGOR Action", Meta = (Keywords = "C++"))
		virtual FTGOR_SlotIndex GetServerSlotData(UTGOR_ActionComponent* Component) const;
	
private:
};
