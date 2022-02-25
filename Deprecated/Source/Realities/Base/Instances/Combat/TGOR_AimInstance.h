// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Storage/TGOR_Serialisation.h"
#include "Realities/Utility/Storage/TGOR_PackageNetwork.h"
#include "Realities/Utility/Error/TGOR_Error.h"

#include "Engine/UserDefinedStruct.h"
#include "TGOR_AimInstance.generated.h"

class UTGOR_InteractableComponent;
class UTGOR_ActionComponent;
class UTGOR_AimComponent;
class UTGOR_Target;
class UTGOR_Force;

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_ForceInstance
{
	GENERATED_USTRUCT_BODY()
	SERIALISE_INIT_HEADER;

	FTGOR_ForceInstance();
	FTGOR_ForceInstance operator*(float Intensity) const;

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Forces with intensities */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<UTGOR_Force*, float> Forces;
};

template<>
struct TStructOpsTypeTraits<FTGOR_ForceInstance> : public TStructOpsTypeTraitsBase2<FTGOR_ForceInstance>
{
	enum
	{
		WithNetSerializer = true
	};
};

/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_InfluenceInstance
{
	GENERATED_USTRUCT_BODY()
	SERIALISE_INIT_HEADER;

	FTGOR_InfluenceInstance();

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Influences target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Target* Target;
	
	/** Influence type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTGOR_ForceInstance Forces;

	/** Instigator actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TWeakObjectPtr<AActor> Instigator;

	/** Hit location in world space */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Location;
};

template<>
struct TStructOpsTypeTraits<FTGOR_InfluenceInstance> : public TStructOpsTypeTraitsBase2<FTGOR_InfluenceInstance>
{
	enum
	{
		WithNetSerializer = true
	};
};

/**
* 
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_AimInstance
{
	GENERATED_USTRUCT_BODY()
	SERIALISE_INIT_HEADER;

	FTGOR_AimInstance();

	void Send(FTGOR_NetworkWritePackage& Package, UTGOR_Context* Context) const;
	void Recv(FTGOR_NetworkReadPackage& Package, UTGOR_Context* Context);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Attached target class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Target* Target;

	/** Customisable component this AimInstance is attached to (defined by target content) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TWeakObjectPtr<USceneComponent> Component;

	/** Customisable index payload (defined by target content) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Index;

	/** Customisable offset vector (defined by target content) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Offset;
};

template<>
struct TStructOpsTypeTraits<FTGOR_AimInstance> : public TStructOpsTypeTraitsBase2<FTGOR_AimInstance>
{
	enum
	{
		WithNetSerializer = true
	};
};


/**
*
*/
USTRUCT(BlueprintType)
struct REALITIES_API FTGOR_AimPoint
{
	GENERATED_USTRUCT_BODY()

		FTGOR_AimPoint();

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Target class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTGOR_Target* Target;

	/** Target Component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TWeakObjectPtr<USceneComponent> Component;

	/** Target distance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Distance;

	/** Target center */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Center;
};