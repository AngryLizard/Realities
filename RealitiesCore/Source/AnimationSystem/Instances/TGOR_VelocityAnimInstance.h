// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "TGOR_SubAnimInstance.h"
#include "TGOR_VelocityAnimInstance.generated.h"


UCLASS(Transient, Blueprintable)
class ANIMATIONSYSTEM_API UTGOR_VelocityAnimInstance : public UTGOR_SubAnimInstance
{
	GENERATED_UCLASS_BODY()

public:
	UTGOR_VelocityAnimInstance();

	/** Current velocity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Velocity;

protected:
private:
};
