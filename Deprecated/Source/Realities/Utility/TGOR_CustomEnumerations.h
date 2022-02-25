// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#include "Engine/UserDefinedEnum.h"
#include "TGOR_CustomEnumerations.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class ETGOR_EventEnumeration : uint8
{
	Running,
	Dead
};

UENUM(BlueprintType)
enum class ETGOR_FloatEnumeration : uint8
{
	Num,
	NaN,
	Inf
};

UENUM(BlueprintType)
enum class ETGOR_BoolEnumeration : uint8
{
	Is,
	IsNot
};

UENUM(BlueprintType)
enum class ETGOR_AbilityEnumeration : uint8
{
	Can,
	CanNot
};

UENUM(BlueprintType)
enum class ETGOR_FetchEnumeration : uint8
{
	Found,
	Empty
};

UENUM(BlueprintType)
enum class ETGOR_UpdateEnumeration : uint8
{
	Success,
	Update,
	Failed
};

UENUM(BlueprintType)
enum class ETGOR_PushEnumeration : uint8
{
	Failed,
	Created,
	Found
};

UENUM(BlueprintType)
enum class ETGOR_Ownership : uint8
{
	Guest,
	Rental,
	Owner
};

UENUM(BlueprintType)
enum class ETGOR_ComputeEnumeration : uint8
{
	Success,
	Failed
};

UENUM(BlueprintType)
enum class ETGOR_NetOccupation : uint8
{
	Host,
	Client,
	Remote,
	Invalid
};

UENUM(BlueprintType)
enum class ETGOR_DirectionEnumeration : uint8
{
	Left,
	Right,
	Middle
};


UENUM(BlueprintType)
enum class ETGOR_RecipeEnumeration : uint8
{
	NoMatch,
	SomeMatch,
	FullMatch
};

UENUM(BlueprintType)
enum class ETGOR_MovementEnumeration : uint8
{
	Queued, // Queued movement, all movements above the queue override even with true invariant
	Sticky, // Queued movement, but doesn't get overridden by movements above the queue
	Manual, // Needs to be executed by preemption, only disappears once invariant is false
	Override // Execute *only* when overriding (terminates even with true invariant)
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
enum class ETGOR_ModAction : uint8
{
	Add,
	Remove
};

UENUM(BlueprintType)
enum class ETGOR_ModloadEnumeration : uint8
{
	Success,
	NotFound,
	WrongMod,
	WrongVersion
};

UENUM(BlueprintType)
enum class ETGOR_Privileges : uint8
{
	Invalid,
	User,
	Operator
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
enum class ETGOR_ReplicationType : uint8
{
	Unreliable,
	Reliable
};

UENUM(BlueprintType)
enum class ETGOR_VariableReplication : uint8
{
	Local,
	Authority,
	Autonomous
};

UENUM(BlueprintType)
enum class ETGOR_NotifyReplication : uint8
{
	NoForward, /** Clients don't forward action state until server update (recommended for long-term running actions) */
	NoValidation, /** Clients don't terminate action on invalid and keep polling until server update (recommended for default) */
	Independent /** Clients forward and validate actions like the server (recommended for client heavy actions with quick response times) */
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
	Valid, // Can be called and is valid
	Uncallable, // Cannot be called but is valid
	Invalid // Cannot be called and is invalid
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