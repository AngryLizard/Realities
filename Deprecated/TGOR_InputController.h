// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Base/Controller/TGOR_CombatController.h"
#include "TGOR_InputController.generated.h"


////////////////////////////////////////////// ENUM //////////////////////////////////////////////////////

UENUM(BlueprintType)
enum class ETGOR_NumberKey : uint8
{
	Hotkey1,
	Hotkey2,
	Hotkey3,
	Hotkey4,
	Hotkey5,
	Hotkey6,
	Hotkey7,
	Hotkey8,
	Hotkey9,
	Hotkey0,
	None
};

////////////////////////////////////////////// STRUCTS //////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_FloatTimer
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Attributes")
		float Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Attributes")
		float Count;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Attributes")
		bool Active;
};

USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_BoolTimer
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Attributes")
		bool Value;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Attributes")
		float Count;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR|Attributes")
		bool Active;
};

////////////////////////////////////////////// DELEGATES //////////////////////////////////////////////////////

DECLARE_DYNAMIC_DELEGATE_OneParam(FInputBoolDelegate, bool, Input);
DECLARE_DYNAMIC_DELEGATE_OneParam(FInputFloatDelegate, float, Input);

/**
* TGOR_InputController allows a playercontroller to filter input and relay it to a controllable
*/
UCLASS()
class REALITIES_API ATGOR_InputController : public ATGOR_CombatController
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_InputController();
	virtual void PlayerTick(float DeltaTime) override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	/** Call short key */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR|HUD", Meta = (Keywords = "C++"))
		void ShortKey(const FKey& Key);

	/** Call long key */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR|HUD", Meta = (Keywords = "C++"))
		void LongKey(const FKey& Key);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Calls input and filters input spamming */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Attributes", Meta = (Keywords = "C++"))
		void CallInputBool(FName Key, bool Input, float ReactionTime = 0.5f);

	/** Calls input without filtering */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Attributes", Meta = (Keywords = "C++"))
		void ForceInputBool(FName Key, bool Input);


	/** Calls input and filters input spamming */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Attributes", Meta = (Keywords = "C++"))
		void CallInputFloat(FName Key, float Input, float ReactionTime = 0.5f);

	/** Calls input without filtering */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Attributes", Meta = (Keywords = "C++"))
		void ForceInputFloat(FName Key, float Input);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(BlueprintReadOnly, Category = "TGOR|Attributes")
		TMap<FName, FTGOR_BoolTimer> BoolTimer;

	UPROPERTY(BlueprintReadOnly, Category = "TGOR|Attributes")
		TMap<FName, FTGOR_FloatTimer> FloatTimer;

	UPROPERTY()
		FInputBoolDelegate InputBoolDelegate;

	UPROPERTY()
		FInputFloatDelegate InputFloatDelegate;

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Switch key into hotkeys */
	UFUNCTION(BlueprintCallable, Category = "TGOR|HUD", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void SwitchHotkey(const FKey& Key, ETGOR_NumberKey& Branches);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Bool deciding whether or not player can control their character */
	UPROPERTY(BlueprintReadWrite, Category = "TGOR|Customisation")
		bool HasControl;

	/** Returns true if player can control their character (Wrapper around HasControl) */
	UFUNCTION(BlueprintCallable, Category = "TGOR|Customisation", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void IsInControl(ETGOR_BoolEnumeration& Branches);

	/** Timeout until longkey gets triggered. */
	UPROPERTY(EditAnywhere, Category = "TGOR|Game", Meta = (Keywords = "C++"))
		float KeyTimeout;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

	virtual bool InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad) override;
	virtual void Tick(float DeltaSeconds) override;

private:

	TArray<FKey> _pressedKeys;
};
