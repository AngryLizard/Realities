// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TargetSystem/TGOR_AimInstance.h"

#include "ActionSystem/Components/TGOR_TerminalComponent.h"
#include "CoreSystem/Storage/TGOR_SaveInterface.h"
#include "TGOR_RegisterComponent.generated.h"

/**
* 
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class PLAYERSYSTEM_API UTGOR_RegisterComponent : public UTGOR_TerminalComponent, public ITGOR_SaveInterface
{
	GENERATED_BODY()

public:	
	UTGOR_RegisterComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;

	virtual void RegisterInteract(UTGOR_ActionTask* ActionSlot, int32 Index) override;
	virtual bool IsSupported(UTGOR_ActionComponent* Component) const override;

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Set of players that have registered with this interactable */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR System")
		TSet<int32> RegisteredPlayerKeys;

private:
};
