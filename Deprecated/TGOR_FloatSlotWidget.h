#pragma once

#include "CoreMinimal.h"
#include "UI/Inventory/TGOR_StorageSlotWidget.h"

#include "TGOR_FloatSlotWidget.generated.h"

/**
 * A test slot for float values.
 */
UCLASS(BlueprintType, Blueprintable)
class REALITIES_API UTGOR_FloatSlotWidget : public UTGOR_StorageSlotWidget
{
	GENERATED_BODY()

public:
	UTGOR_FloatSlotWidget();

	UFUNCTION(BlueprintPure, Category = "TGOR Storage UI", Meta = (Keywords = "C++"))
		float GetFloat() const;

	UFUNCTION(BlueprintCallable, Category = "TGOR Storage UI", Meta = (Keywords = "C++"))
		bool PutFloat(float Value);

private:

};