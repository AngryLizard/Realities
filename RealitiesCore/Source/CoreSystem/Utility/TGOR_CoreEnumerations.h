// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#include "TGOR_CoreEnumerations.generated.h"

/**
 * 
 */


UENUM(BlueprintType)
enum class ETGOR_NetvarEnumeration : uint8
{
	/** Not replicated */
	Local,
	/** Only server replicates */
	Server,
	/** Everyone can replicate */
	Client
};

UENUM(BlueprintType)
enum class ETGOR_FloatEnumeration : uint8
{
	Num,
	NaN,
	Inf
};

UENUM(BlueprintType)
enum class ETGOR_ValidEnumeration : uint8
{
	Valid,
	Invalid
};

UENUM(BlueprintType)
enum class ETGOR_BoolEnumeration : uint8
{
	Is,
	IsNot
};

UENUM(BlueprintType)
enum class ETGOR_CanEnumeration : uint8
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
enum class ETGOR_ReplicationType : uint8
{
	Unreliable,
	Reliable
};

UENUM(BlueprintType)
enum class ETGOR_PermissionType : uint8
{
	Local,
	Authority,
	Autonomous
};

UENUM(BlueprintType)
enum class ETGOR_PushEnumeration : uint8
{
	Failed,
	Created,
	Found
};
