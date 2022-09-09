// Copyright Epic Games, Inc. All Rights Reserved.

#include "TGOR_ModifierCommands.h"

#define LOCTEXT_NAMESPACE "TGOR_ModifierCommands"

void FTGOR_ModifierCommands::RegisterCommands()
{
	UI_COMMAND(MirrorX, "Mirror Channel X", "Mirror selected ControlRig channels along X axis", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(Loop, "Loop Channel", "Loop selected channels", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(TimeOffsetHalf, "Time Offset Half", "Time offset selected channels by half the playback length", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(MirrorLeftToRight, "Mirror Left To Right", "Mirror selected ControlRig channels (_L suffix) to their right-hand equivalent (same name, _R suffix)", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(FlipLeftToRight, "Flip Left To Right", "Flip (mirror with offset) selected ControlRig channels (_L suffix) to their right-hand equivalent (same name, _R suffix)", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(MirrorRightToLeft, "Mirror Right To Left", "Mirror selected ControlRig channels (_R suffix) to their left-hand equivalent (same name, _L suffix)", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(FlipRightToLeft, "Flip Right To Left", "Flip (mirror with offset) selected ControlRig channels (_R suffix) to their left-hand equivalent (same name, _L suffix)", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
