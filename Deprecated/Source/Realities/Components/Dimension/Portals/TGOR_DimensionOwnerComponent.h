// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/TGOR_CustomEnumerations.h"

#include "Realities/Components/Dimension/Portals/TGOR_DimensionLoaderComponent.h"
#include "TGOR_DimensionOwnerComponent.generated.h"

class UTGOR_NamedSocket;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REALITIES_API UTGOR_DimensionOwnerComponent : public UTGOR_DimensionLoaderComponent
{
	GENERATED_BODY()

public:	
	UTGOR_DimensionOwnerComponent();
	virtual void BeginPlay() override;

	virtual bool Assemble(UTGOR_DimensionData* Dimension) override;
	virtual bool IsReady() const override;

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Registers a given dimension and portal with dimension owner */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void DialOwner();

	/** Returns the connected identity */
	UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		UTGOR_MovementComponent* GetOwnerMovement() const;

	/* Target identity */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		int32 ActorIdentifier;

	/* Target socket */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		TSubclassOf<UTGOR_NamedSocket> TargetSocket;

	/** Signifies whether this component is in the middle of sending or receiving a teleport. Mostly useful for overlap events. */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Dimension")
		bool IsMidTeleport;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Teleports  */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void EnterOther(UTGOR_PilotComponent* Component, ETGOR_ComputeEnumeration& Branches);

	/** Returns whether any mobility can teleport from this portal */
	UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		bool CanTeleport(UTGOR_PilotComponent* Component) const;
};
