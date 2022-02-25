// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/Error/TGOR_Error.h"

#include "Realities/Components/Movement/TGOR_SymplecticComponent.h"
#include "TGOR_CustomMovementComponent.generated.h"

/**
* TGOR_CustomMovementComponent is used for server-bound movement, for example pets or platforms
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_CustomMovementComponent : public UTGOR_SymplecticComponent
{
	GENERATED_BODY()

public:
	UTGOR_CustomMovementComponent();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Called before physics is computed */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void OnPreComputePhysics(const FTGOR_MovementTick& Tick, bool Replay);

	/** Called when forces and torque are computed */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void OnComputePhysics(const FTGOR_MovementTick& Tick, const FTGOR_MovementOutput& Output, const FTGOR_MovementSpace& Space, bool Replay, FTGOR_MovementOutput& Out);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	virtual void PreComputePhysics(const FTGOR_MovementTick& Tick, bool Replay) override;
	virtual void ComputePhysics(const FTGOR_MovementTick& Tick, const FTGOR_MovementSpace& Space, bool Replay, FTGOR_MovementOutput& Output) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Load external properties from the buffer at an index. External properties are buffer properties that are set even during replay. */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void OnLoadFromBufferExternal(int32 Index);

	/** Load internal properties from the buffer at an index. Internal properties are buffer properties that are set before replay. Does not need to include external properties. */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void OnLoadFromBufferWhole(int32 Index);

	/** Linearly interpolate between PrevIndex and NextIndex and set the result to the buffer at PrevIndex. */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void OnLerpToBuffer(int32 PrevIndex, int32 NextIndex, float Alpha);

	/** Set properties of buffer at index to the current state. */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void OnStoreToBuffer(int32 Index);

	/** Reset buffers to a given size. */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR Movement", Meta = (Keywords = "C++"))
		void OnSetBufferSize(int32 Size);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	virtual void LoadFromBufferExternal(int32 Index) override;
	virtual void LoadFromBufferWhole(int32 Index) override;
	virtual void LerpToBuffer(int32 PrevIndex, int32 NextIndex, float Alpha) override;
	virtual void StoreToBuffer(int32 Index) override;
	virtual void SetBufferSize(int32 Size) override;

};
