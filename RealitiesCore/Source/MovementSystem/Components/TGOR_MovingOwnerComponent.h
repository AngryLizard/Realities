// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "DimensionSystem/Components/TGOR_DimensionOwnerComponent.h"
#include "TGOR_MovingOwnerComponent.generated.h"

class UTGOR_NamedSocket;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MOVEMENTSYSTEM_API UTGOR_MovingOwnerComponent : public UTGOR_DimensionOwnerComponent
{
	GENERATED_BODY()

public:	
	UTGOR_MovingOwnerComponent();
	virtual bool IsReady() const override;

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Returns the connected identity */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		UTGOR_SocketComponent* GetOwnerMovement() const;

	/* Target socket */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		TSubclassOf<UTGOR_NamedSocket> TargetSocket;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Teleports  */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		virtual void EnterOther(UTGOR_PilotComponent* Component, ETGOR_ComputeEnumeration& Branches);

	/** Returns whether any mobility can teleport from this portal */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		virtual bool CanTeleport(UTGOR_PilotComponent* Component) const;
};
