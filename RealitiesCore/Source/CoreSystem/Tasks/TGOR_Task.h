// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"

#include "CoreSystem/TGOR_Object.h"
#include "TGOR_Task.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////
class UTGOR_Content;

/**
* TGOR_Action handles any kind of replicated actions
*/
UCLASS()
class CORESYSTEM_API UTGOR_Task : public UTGOR_Object
{
	GENERATED_BODY()

public:
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_Task();

	//virtual bool IsNameStableForNetworking() const { return true; };
	virtual bool IsSupportedForNetworking() const { return true; };

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	bool CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack) override;
	int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;

	template<typename S, typename T, typename F>
	static T* CreateTask(F* Factory, TSubclassOf<T> Type, S* Component = nullptr, int32 SlotIdentifier = -1)
	{
		if (!IsValid(Factory))
		{
			ERRET("No factory given.", Error, nullptr);
		}

		if (!*Type)
		{
			ERRET("No class type given.", Error, nullptr);
		}

		T* Task = nullptr;

		FString TaskName = Factory->GetName() + FString::FromInt(SlotIdentifier);
		if (IsValid(Component))
		{
			Task = FindObject<T>(Component, *TaskName);
			if (!IsValid(Task))
			{
				// Keep in mind that a custom outer hierarchy is only kept on the server. Replicated subobjects are flat.
				Task = NewObject<T>(Component, Type, FName(*TaskName));
			}

			Task->Identifier.Component = Component;
			Task->Identifier.Content = Factory;
			Task->Identifier.Slot = SlotIdentifier;
			Task->Initialise(); // Initialise
		}
		else
		{
			Task = NewObject<T>(Factory, Type, FName(*TaskName));
			Task->Identifier.Content = Factory;
		}
		return Task;
	}
};
