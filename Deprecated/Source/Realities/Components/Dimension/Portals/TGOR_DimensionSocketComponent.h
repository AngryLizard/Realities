// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/TGOR_CustomEnumerations.h"

#include "Realities/Components/Dimension/Portals/TGOR_DimensionPortalComponent.h"
#include "TGOR_DimensionSocketComponent.generated.h"

class UTGOR_Socket;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REALITIES_API UTGOR_DimensionSocketComponent : public UTGOR_DimensionPortalComponent
{
	GENERATED_BODY()

public:	
	UTGOR_DimensionSocketComponent();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual void EnterSelf(UTGOR_PilotComponent* Component, const FTransform& Offset, ETGOR_ComputeEnumeration& Branches) override;
	virtual bool IsOccupied(UTGOR_PilotComponent* Component) const override;


	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Socket type to teleport to */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Dimension")
		TSubclassOf<UTGOR_Socket> TargetSocket;


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	
	/** Teleports any mobility linked to a given socket type */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		virtual void TeleportSocket(TSubclassOf<UTGOR_Socket> SourceSocket, ETGOR_ComputeEnumeration& Branches);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Socket parent */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Movement")
		UTGOR_MobilityComponent* Parent;


};
