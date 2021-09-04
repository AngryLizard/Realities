// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "CoreSystem/TGOR_Object.h"
#include "TGOR_DelegateBase.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FFloatDelegate, float, Number);
DECLARE_DYNAMIC_DELEGATE_OneParam(FIntegerDelegate, int32, Index);
DECLARE_DYNAMIC_DELEGATE_OneParam(FObjectDelegate, UObject*, Object);

/**
* TGOR_DelegateBase allows to link events to this object via name
*/
UCLASS()
class UISYSTEM_API UTGOR_DelegateBase : public UTGOR_Object
{
	GENERATED_BODY()

public:
	UTGOR_DelegateBase();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Float delegate */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interface")
		FFloatDelegate FloatDelegate;

	/** Float event */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interface")
		FName FloatEvent;


	/** Integer delegate */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interface")
		FIntegerDelegate IntegerDelegate;

	/** Integer event */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interface")
		FName IntegerEvent;


	/** Object delegate */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interface")
		FObjectDelegate ObjectDelegate;

	/** Object event */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Interface")
		FName ObjectEvent;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Call float event */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Interface", Meta = (Keywords = "C++"))
		virtual void CallFloatEvent(float Number);

	/** Call integer event */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Interface", Meta = (Keywords = "C++"))
		virtual void CallIntegerEvent(int32 Index);

	/** Call Object event */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Interface", Meta = (Keywords = "C++"))
		virtual void CallObjectEvent(UObject* Object);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Assigns a target to call events on */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Interface", Meta = (Keywords = "C++"))
		void AssignTarget(UObject* Object);

	/** Returns true if target has been assigned */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Interface", Meta = (Keywords = "C++"))
		bool hasTarget();

	/** Assign integer event */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Interface", Meta = (Keywords = "C++"))
		void AssignFloatEvent(FFloatDelegate Call);

	/** Assign float event */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Interface", Meta = (Keywords = "C++"))
		void AssignIntegerEvent(FIntegerDelegate Call);

	/** Assign Object event */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Interface", Meta = (Keywords = "C++"))
		void AssignObjectEvent(FObjectDelegate Call);

	template<typename E, typename ... T>
	void EnsureDelegate(E& Delegate, FName Name, T... t)
	{
		if ((!Delegate.IsBound() || Delegate.GetUObject() != Target || Delegate.GetFunctionName() != Name) && !Name.IsNone())
			Delegate.BindUFunction(Target, Name);

		Delegate.ExecuteIfBound(t...);
	}

	UObject* Target;
};
