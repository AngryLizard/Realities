// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Engine/Classes/Components/SplineComponent.h"
#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Utility/Datastructures/TGOR_Time.h"

#include "Realities/Actors/TGOR_Actor.h"
#include "TGOR_PlatformActor.generated.h"

///////////////////////////////////////////////// DECL ///////////////////////////////////////////////////

class UTGOR_MobilityComponent;

///////////////////////////////////////////////// ENUM ///////////////////////////////////////////////////

UENUM(BlueprintType)
enum class ETGOR_PlatformLocationMode : uint8
{
	MoveNone,
	MoveLinearly,
	MoveSinus
};

UENUM(BlueprintType)
enum class ETGOR_PlatformRotationMode : uint8
{
	MoveNone,
	MoveLinearly,
	MoveSpline
};

/**
* TGOR_PlatformActor provides basic functionality for moving platforms
*/
UCLASS()
class REALITIES_API ATGOR_PlatformActor : public ATGOR_Actor
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_PlatformActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** */

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TGOR Platform", meta = (AllowPrivateAccess = "true"))
		USplineComponent* SplineComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UTGOR_MobilityComponent* MobilityComponent;

public:

	FORCEINLINE UTGOR_MobilityComponent* GetMobile() const { return MobilityComponent; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Relative speed of this platform */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Platform", Meta = (Keywords = "C++"))
		float Speed;

	/** Threshold to increase time between the platform going */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Platform", Meta = (Keywords = "C++"))
		float Threshold;

	/** Offset used for movement functions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Platform", Meta = (Keywords = "C++"))
		float Offset;

	/** Mode platform changes location with */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Platform", Meta = (Keywords = "C++"))
		ETGOR_PlatformLocationMode LocationMode;

	/** Mode platform changes rotation with */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Platform", Meta = (Keywords = "C++"))
		ETGOR_PlatformRotationMode RotationMode;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

	/** Set position of scene component depending on time */
	void SetSceneComponent(FTGOR_Time Time);

private:
};
