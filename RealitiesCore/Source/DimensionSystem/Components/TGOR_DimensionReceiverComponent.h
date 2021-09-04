// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "../TGOR_DimensionInstance.h"

#include "DimensionSystem/Components/TGOR_ConnectionComponent.h"
#include "TGOR_DimensionReceiverComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FComponentReceivedDelegate, UTGOR_PilotComponent*, Component);

UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class DIMENSIONSYSTEM_API UTGOR_DimensionReceiverComponent : public UTGOR_ConnectionComponent
{
	GENERATED_BODY()

public:
	UTGOR_DimensionReceiverComponent();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/* Portal local offset on arrival */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		FTransform PortalTransform;

	/** Signifies whether this component is in the middle of receiving a teleport. Mostly useful for overlap events. */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Dimension")
		bool IsMidTeleport;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Connects this receiver with a sender */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Region", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void Connect(UTGOR_DimensionSenderComponent* Sender, ETGOR_ComputeEnumeration& Branches);


	/** Set portal transform in world space */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void SetPortalTransform(const FTransform& Transform);

	/** Moves a controllable to this receiver */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		virtual void EnterSelf(UTGOR_PilotComponent* Component, const FTransform& Offset, ETGOR_ComputeEnumeration& Branches);

	/** Returns whether a portal can be entered by a given mobility */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		virtual bool IsOccupied(UTGOR_PilotComponent* Component) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Dimension")
		FComponentReceivedDelegate OnReceivedComponent;

	/* Whether all portal activity is reported */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		bool Verbose;
};
