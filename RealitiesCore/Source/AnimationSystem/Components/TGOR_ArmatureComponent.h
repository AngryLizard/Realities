// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "PhysicsSystem/TGOR_PhysicsInstance.h"

#include "SocketSystem/Components/TGOR_SocketComponent.h"
#include "TGOR_ArmatureComponent.generated.h"

class UTGOR_NamedSocket;

/**
 * 
 */
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class ANIMATIONSYSTEM_API UTGOR_ArmatureComponent : public UTGOR_SocketComponent
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_ArmatureComponent();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual FTGOR_MovementDynamic GetIndexTransform(int32 Index) const override;
	virtual int32 GetIndexFromName(const FName& Name) const override;
	virtual FName GetNameFromIndex(int32 Index) const override;
	virtual bool IsValidIndex(int32 Index) const override;

	virtual void AttachToMovement(USceneComponent* Component, int32 Index) override;

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get skeletal mesh active with this component */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Movement", Meta = (Keywords = "C++"))
		USkeletalMeshComponent* GetSkeletalMesh() const;


};