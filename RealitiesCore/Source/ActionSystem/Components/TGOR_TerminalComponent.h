// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TargetSystem/TGOR_AimInstance.h"

#include "../UI/TGOR_TabMenu.h"
#include "InventorySystem/TGOR_ItemInstance.h"

#include "TargetSystem/Components/TGOR_WorldInteractableComponent.h"
#include "TGOR_TerminalComponent.generated.h"

class UTGOR_Content;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTerminalContentInteractDelegate, UTGOR_ActionTask*, ActionSlot, UTGOR_Content*, Content);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTerminalRegisterInteractDelegate, UTGOR_ActionTask*, ActionSlot, int32, Index);

/**
* 
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class ACTIONSYSTEM_API UTGOR_TerminalComponent : public UTGOR_WorldInteractableComponent
{
	GENERATED_BODY()

public:	
	UTGOR_TerminalComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Action")
		FTerminalContentInteractDelegate OnTerminalContentInteract;

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Action")
		FTerminalRegisterInteractDelegate OnTerminalRegisterInteract;

	/** Which UMG tab menu to display when terminal is opened. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Action")
		TSubclassOf<UTGOR_TerminalMenu> TabMenu;

	/** The UMG tab Menu defines whether there is an item row at all, this filter additionally decides which item types are displayed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Action")
		FTGOR_ItemRestriction SupportedItem;

	/** Called to get custom slot data assigned to a interacting action component. IsValid is automatically set by IsSupported after this call. */
	UFUNCTION(BlueprintPure, BlueprintImplementableEvent, Category = "!TGOR Action|Internal", Meta = (Keywords = "C++"))
		void GetTerminalSlotData(UTGOR_ActionComponent* Component, int32& Data) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Interacts with this terminal given an index. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action|Internal", Meta = (Keywords = "C++"))
		virtual void ContentInteract(UTGOR_ActionTask* ActionSlot, UTGOR_Content* Content);

	/** Component event when this terminal is interacted with and index */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "!TGOR Action|Internal", Meta = (Keywords = "C++"))
		void OnContentInteract(UTGOR_ActionTask* ActionSlot, UTGOR_Content* Content);

	/** Interacts with this terminal given a content. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Action|Internal", Meta = (Keywords = "C++"))
		virtual void RegisterInteract(UTGOR_ActionTask* ActionSlot, int32 Index);

	/** Component event when this terminal is interacted with a content */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "!TGOR Action|Internal", Meta = (Keywords = "C++"))
		void OnRegisterInteract(UTGOR_ActionTask* ActionSlot, int32 Index);

	/** Checks on the server whether this terminal supports given action component (TabMenu is still displayed but without "IsSupported" update). */
	UFUNCTION(BlueprintPure, BlueprintAuthorityOnly, Category = "!TGOR Action|Internal", Meta = (Keywords = "C++"))
		virtual bool IsSupported(UTGOR_ActionComponent* Component) const;

	/** Gets slot data to send information from server to client */
	UFUNCTION(BlueprintPure, BlueprintAuthorityOnly, Category = "!TGOR Action|Internal", Meta = (Keywords = "C++"))
		virtual void GetServerSlotData(UTGOR_ActionComponent* Component, int32& Data) const;
	
private:
};
