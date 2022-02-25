// The Gateway of Realities: Planes of Existence.

#include "Realities.h"
#include "TGOR_InputController.h"


ATGOR_InputController::ATGOR_InputController()
	: Super()
{
}


void ATGOR_InputController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	for (auto Pair = FloatTimer.CreateIterator(); Pair; ++Pair)
	{
		FTGOR_FloatTimer& Timer = Pair->Value;
		Timer.Count -= DeltaTime;
		// Call filtered input after timer runs out
		if (Timer.Count < 0.0f)
		{
			// reset timer
			Pair.RemoveCurrent();
		}
	}

	for (auto Pair = BoolTimer.CreateIterator(); Pair; ++Pair)
	{
		FTGOR_BoolTimer& Timer = Pair->Value;
		Timer.Count -= DeltaTime;
		// Call filtered input after timer runs out
		if (Timer.Count < 0.0f)
		{
			// reset timer
			Pair.RemoveCurrent();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_InputController::CallInputBool(FName Key, bool Input, float ReactionTime)
{
	// If still suspended store last input
	if (BoolTimer.Contains(Key))
	{
		FTGOR_BoolTimer& Timer = BoolTimer[Key];
		Timer.Value = Input;

		// Allow immediate stopping but delay further tapping
		if (Input && Timer.Active)
		{
		}
		else
		{
			if (Timer.Active)
			{
				ForceInputBool(Key, Input);
			}
			Timer.Active = false;
		}
	}
	else
	{
		// If newly activated, register timer
		if (Input)
		{
			FTGOR_BoolTimer Timer = { Input, ReactionTime, true };
			BoolTimer.Add(Key, Timer);
		}
		// Call event
		ForceInputBool(Key, Input);
	}
}


void ATGOR_InputController::ForceInputBool(FName Key, bool Input)
{
	// Call function on controller pawn
	InputBoolDelegate.BindUFunction(GetPawn(), Key);
	InputBoolDelegate.ExecuteIfBound(Input);
}


void ATGOR_InputController::CallInputFloat(FName Key, float Input, float ReactionTime)
{
	// If still suspended store last input
	if (FloatTimer.Contains(Key))
	{
		FTGOR_FloatTimer& Timer = FloatTimer[Key];
		Timer.Value = Input;

		// Allow immediate stopping but delay further tapping
		if (FMath::Abs(Input) > SMALL_NUMBER && Timer.Active)
		{
		}
		else
		{
			if (Timer.Active)
			{
				ForceInputFloat(Key, Input);
			}
			Timer.Active = false;
		}
	}
	else
	{
		// If newly activated, register timer
		if (FMath::Abs(Input) > SMALL_NUMBER)
		{
			FTGOR_FloatTimer Timer = { Input, ReactionTime, true };
			FloatTimer.Add(Key, Timer);
		}
		// Call event
		ForceInputFloat(Key, Input);
	}
}

void ATGOR_InputController::ForceInputFloat(FName Key, float Input)
{
	// Call function on controller pawn
	InputFloatDelegate.BindUFunction(GetPawn(), Key);
	InputFloatDelegate.ExecuteIfBound(Input);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_InputController::SwitchHotkey(const FKey& Key, ETGOR_NumberKey& Branches)
{
	if (Key == EKeys::Zero) Branches = ETGOR_NumberKey::Hotkey0; else
		if (Key == EKeys::One) Branches = ETGOR_NumberKey::Hotkey1; else
			if (Key == EKeys::Two) Branches = ETGOR_NumberKey::Hotkey2; else
				if (Key == EKeys::Three) Branches = ETGOR_NumberKey::Hotkey3; else
					if (Key == EKeys::Four) Branches = ETGOR_NumberKey::Hotkey4; else
						if (Key == EKeys::Five) Branches = ETGOR_NumberKey::Hotkey5; else
							if (Key == EKeys::Six) Branches = ETGOR_NumberKey::Hotkey6; else
								if (Key == EKeys::Seven) Branches = ETGOR_NumberKey::Hotkey7; else
									if (Key == EKeys::Eight) Branches = ETGOR_NumberKey::Hotkey8; else
										if (Key == EKeys::Nine) Branches = ETGOR_NumberKey::Hotkey9; else
											Branches = ETGOR_NumberKey::None;
}


bool ATGOR_InputController::InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad)
{
	int32 Index;
	if (EventType == EInputEvent::IE_Pressed)
		if (!_pressedKeys.Find(Key, Index)) _pressedKeys.AddUnique(Key);
	return(Super::InputKey(Key, EventType, AmountDepressed, bGamepad));
}

void ATGOR_InputController::Tick(float DeltaSeconds)
{
	for (int i = _pressedKeys.Num() - 1; i >= 0; i--)
	{
		FKey Key = _pressedKeys[i];
		float Time = GetInputKeyTimeDown(Key);
		if (Time > KeyTimeout)
		{
			_pressedKeys.RemoveAt(i);
			LongKey(Key);
		}
		else if (!IsInputKeyDown(Key))
		{
			_pressedKeys.RemoveAt(i);
			ShortKey(Key);
		}
	}

	Super::Tick(DeltaSeconds);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ATGOR_InputController::IsInControl(ETGOR_BoolEnumeration& Branches)
{
	Branches = ETGOR_BoolEnumeration::IsNot;
	if (HasControl)
		Branches = ETGOR_BoolEnumeration::Is;
}
