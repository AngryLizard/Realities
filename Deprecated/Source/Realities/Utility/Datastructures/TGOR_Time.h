// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Utility/Storage/TGOR_PackageGroup.h"
#include "Realities/Utility/Storage/TGOR_PackageNetwork.h"
#include "Realities/Utility/Storage/TGOR_Serialisation.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_Time.generated.h"

#define SECONDS_PER_EPOCH 86'400.0f

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_Time
{
	GENERATED_USTRUCT_BODY()
	SERIALISE_INIT_HEADER;
	FTGOR_Time();

	void Write(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const;
	void Read(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context);
	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Singleton* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Singleton* Context);

	void Normalize();
	void Clear(); // Set to 0
	bool IsZero() const; // Whether it is 0

	float Modf(float Period = (2*PI), float Speed = 1.0f, float Phase = 0.0f) const;
	float Value() const; // Incremental value for visuals, jumps every epoch
	FString Print() const;

	bool operator==(const FTGOR_Time& Other) const;
	bool operator!=(const FTGOR_Time& Other) const;
	bool operator>(const FTGOR_Time& Other) const;
	bool operator<(const FTGOR_Time& Other) const;
	float operator-(const FTGOR_Time& Other) const;
	FTGOR_Time operator+=(float DeltaSeconds);
	FTGOR_Time operator+(float DeltaSeconds) const;

	UPROPERTY()
		float Seconds;

	UPROPERTY()
		uint16 Epochs;
};

template<> struct TStructOpsTypeTraits<FTGOR_Time> : public TStructOpsTypeTraitsBase2<FTGOR_Time>
{
	enum { WithNetSerializer = true };
};