#pragma once

#include "CoreMinimal.h"

#include "Mod/Spawner/Items/TGOR_StorageItem.h"
#include "Base/Inventory/TGOR_ArrayStorage.h"

#include "TGOR_FloatStorage.generated.h"

class UTGOR_StorageItem;

/**
 * A test item storage for float values.
 */
UCLASS(BlueprintType, Blueprintable)
class REALITIES_API UTGOR_FloatStorage : public UTGOR_ArrayStorage
{
	GENERATED_BODY()

public:
	UTGOR_FloatStorage();

	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		virtual float GetFloat(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		virtual int32 AddFloat(float Value);

	UFUNCTION(BlueprintCallable, Category = "TGOR Storage", Meta = (Keywords = "C++"))
		virtual int32 ReplaceFloat(float Value, int32 Index);

	/* UTGOR_Storage */

	virtual void Clear() override;

	virtual bool RemoveAt(int32 Index) override;

	virtual void Assign(const UTGOR_Storage* Other) override;

	virtual int32 GetMaxSize() const override;

	virtual int32 GetCurrentSize() const override;

	virtual bool IsIndexValid(int32 Index) const override;

	virtual UTGOR_Item* GetItem() const override;

	virtual bool Equals(const UTGOR_Storage* Other) const override;

protected:
	virtual void SetItem(UTGOR_Item* NewItem) override;

private:
	/** The item of this storage. */
	UPROPERTY(VisibleAnywhere, Category = "TGOR ItemStorage")
		UTGOR_StorageItem* ItemRef;

	/** The actual float value storage. */
	UPROPERTY(VisibleAnywhere, Category = "TGOR Storage")
		TArray<float> Values;
};