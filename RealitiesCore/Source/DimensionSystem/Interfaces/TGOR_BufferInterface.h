// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TGOR_BufferInterface.generated.h"

class UTGOR_Content;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UTGOR_BufferInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class DIMENSIONSYSTEM_API ITGOR_BufferInterface
{
	GENERATED_BODY()

public:

	/** Set current external state from buffer at index, uses only information that isn't changed through the movement mode including input and base information.
	  * Note that the current base can change to a different parent than stored in the parentBuffer, but parent buffer can still be applied and the base is probable to be nearby and influence movement anyway. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Network|Internal", Meta = (Keywords = "C++"))
		virtual void LoadFromBufferExternal(int32 Index);

	/** Set current state from buffer at Index, uses all available information that isn't already set from external. This is always called after external. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Network|Internal", Meta = (Keywords = "C++"))
		virtual void LoadFromBufferInternal(int32 Index);

	/** Set buffer at Index to a linear lerp between Index and Index+1. Sets Index to new BufferBottom. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Network|Internal", Meta = (Keywords = "C++"))
		virtual void LerpToBuffer(int32 Index, int32 NextIndex, float Alpha);

	/** Set buffer at Index from current state. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Network|Internal", Meta = (Keywords = "C++"))
		virtual void StoreToBuffer(int32 Index);

	/** Set buffer size, buffers content may or may not be wiped. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Network|Internal", Meta = (Keywords = "C++"))
		virtual void SetBufferSize(int32 Size);

};


UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UTGOR_StashInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class DIMENSIONSYSTEM_API ITGOR_StashInterface
{
	GENERATED_BODY()

public:

	/** Sometimes we want to stash an object's current state during buffer operations so it can be reverted later. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Network|Internal", Meta = (Keywords = "C++"))
		virtual void Stash();

	/** Revert stash if available. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Network|Internal", Meta = (Keywords = "C++"))
		virtual void Revert();

	/** Whether we're currently stashing a state. */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Network|Internal", Meta = (Keywords = "C++"))
		virtual bool HasStash() const;

private:
	bool StashState = false;
};
