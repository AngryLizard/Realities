// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#include "Engine/UserDefinedEnum.h"
#include "TGOR_CustomEnumerations.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class ETGOR_Ownership : uint8
{
	Guest,
	Rental,
	Owner
};



UENUM(BlueprintType)
enum class ETGOR_ColorHue : uint8
{
	Any,
	Red,
	Orange,
	Yellow,
	Brown,
	Green,
	Cyan,
	Blue,
	Purple,
	Grey,
	Multicolor,
	Uncategorized
};

UENUM(BlueprintType)
enum class ETGOR_ColorType : uint8
{
	Any,
	White,
	Light,
	Ashen,
	Exotic,
	Vivid,
	Normal,
	Dark,
	Black,
	Phantom,
	Freeze,
	Burn,
	Uncategorized
};


UENUM(BlueprintType)
enum class ETGOR_WeaponCallType : uint8
{
	Main,
	Special,
	Skill,
	Item
};



UENUM(BlueprintType)
enum class ETGOR_LoaderType : uint8
{
	Level,
	Actor,
	RemoveByTag,
	RemoveByTransform
};

UENUM(BlueprintType)
enum class ETGOR_Gender : uint8
{
	NoGender,
	Male,
	Female,
	Herm,
	Custom1,
	Custom2,
	Custom3,
	Custom4
};

UENUM(BlueprintType)
enum class ETGOR_WeaponMode : uint8
{
	Ground,
	Swimming,
	Flying
};


UENUM(BlueprintType)
enum class ETGOR_ForwardAuthority : uint8
{
	/** Only server can forward */
	Authority,
	/** Client forwards state */
	Autonomous
};



UENUM(BlueprintType)
enum class ETGOR_ActionStage : uint8
{
	Active,
	Pending,
	Passive
};

UENUM(BlueprintType)
enum class ETGOR_VariableType : uint8
{
	None,
	Byte,
	Float,
	Integer,
	Boolean,
	Vector,
	Normal,
	Actor,
	Component
};

UENUM(BlueprintType)
enum ETGOR_CustomMovementMode
{
	Skiing,
	Montage,
	Climb,
	Waypoint
};

UENUM(BlueprintType)
enum class ETGOR_TripleEnumeration : uint8
{
	Primary,
	Secondary,
	Tertiary
};

UENUM(BlueprintType)
enum class ETGOR_EquipableInputEnumeration : uint8
{
	None,
	Primary,
	Secondary,
	Special
};

UENUM(BlueprintType)
enum class ETGOR_TerminationEnumeration : uint8
{
	Terminate,
	Continue
};

UENUM(BlueprintType)
enum class ETGOR_WaitEnumeration : uint8
{
	Wait,
	Continue
};

UENUM(BlueprintType)
enum class ETGOR_InvarianceEnumeration : uint8
{
	// Can be called and is valid
	Valid,
	// Cannot be called but is valid
	Uncallable,
	// Cannot be called and is invalid
	Invalid
};

UENUM(BlueprintType)
enum class ETGOR_LocationEnumeration : uint8
{
	Top,
	Right,
	Bottom,
	Left,
	Center
};


UENUM(BlueprintType)
enum class ETGOR_InputEnumeration : uint8
{
	Input_4_3_1 UMETA(DisplayName = "16/8/1"),
	Input_4_4_0 UMETA(DisplayName = "16/16/0"),
	Input_6_1_1 UMETA(DisplayName = "64/1/1"),
	Input_7_1_0 UMETA(DisplayName = "128/1/0"),
	Input_8_0_0 UMETA(DisplayName = "256/0/0"),
	Input_0_0_0 UMETA(DisplayName = "0/0/0")
};