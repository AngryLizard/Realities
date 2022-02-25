// Fill out your copyright notice in the Description page of Project Settings.


#include "TGOR_VelocityAnimInstance.h"
#include "Realities/Utility/Error/TGOR_Error.h"
#include "DrawDebugHelpers.h"
#include "Engine.h"


UTGOR_VelocityAnimInstance::UTGOR_VelocityAnimInstance()
	: Super(), Velocity(FVector::ZeroVector)
{
}

UTGOR_VelocityAnimInstance::UTGOR_VelocityAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), Velocity(FVector::ZeroVector)
{
}
