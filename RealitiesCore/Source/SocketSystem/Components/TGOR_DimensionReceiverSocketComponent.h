// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "DimensionSystem/Components/TGOR_DimensionReceiverComponent.h"
#include "TGOR_DimensionReceiverSocketComponent.generated.h"

class UTGOR_NamedSocket;
class UTGOR_SocketComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SOCKETSYSTEM_API UTGOR_DimensionReceiverSocketComponent : public UTGOR_DimensionReceiverComponent
{
	GENERATED_BODY()

public:	
	UTGOR_DimensionReceiverSocketComponent();
	virtual void BeginPlay() override;

	virtual void EnterSelf(UTGOR_PilotComponent* Component, const FTransform& Offset, ETGOR_ComputeEnumeration& Branches) override;
	virtual bool IsOccupied(UTGOR_PilotComponent* Component) const override;


	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Socket type to teleport to */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Dimension")
		TSubclassOf<UTGOR_NamedSocket> TargetSocket;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Socket parent */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Movement")
		UTGOR_SocketComponent* Parent;


};
