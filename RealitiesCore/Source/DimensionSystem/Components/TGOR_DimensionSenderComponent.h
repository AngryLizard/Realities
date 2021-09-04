// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "../TGOR_DimensionInstance.h"

#include "TGOR_DimensionLoaderComponent.h"
#include "TGOR_DimensionSenderComponent.generated.h"

class UTGOR_MobilityComponent;
class UTGOR_DimensionData;
class UTGOR_Dimension;
class ATGOR_Pawn;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FComponentSentDelegate, UTGOR_PilotComponent*, Component);

UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class DIMENSIONSYSTEM_API UTGOR_DimensionSenderComponent : public UTGOR_DimensionLoaderComponent
{
	GENERATED_BODY()

public:
	UTGOR_DimensionSenderComponent();

	void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;

	virtual bool IsReady() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/* Target portal identifier */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		FName OtherPortalName;

	/** Signifies whether this component is in the middle of sending a teleport. Mostly useful for overlap events. */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Dimension")
		bool IsMidTeleport;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Connects this sender with a receiver */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Region", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void Connect(UTGOR_DimensionReceiverComponent* Receiver, ETGOR_ComputeEnumeration& Branches);

	/** Registers a given dimension and portal with given identifier */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		virtual void DialPortal(const FTGOR_ConnectionSelection& Selection);

	/** Assigns portal without loading dimension */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		virtual void DialPortalName(const FName& Portal);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Returns the connected portal */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		UTGOR_DimensionReceiverComponent* GetReceiverComponent() const;

	/** Moves a controllable to linked dimension if loaded, KeepRelative keeps offset of actor to this portal */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		virtual void EnterOther(UTGOR_PilotComponent* Component, bool KeepRelative, const FTransform& Offset, ETGOR_ComputeEnumeration& Branches);

	/** Returns whether any mobility can teleport from this portal */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		virtual bool CanTeleport(UTGOR_PilotComponent* Component) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	UPROPERTY(BlueprintAssignable, Category = "!TGOR Dimension")
		FComponentSentDelegate OnSentComponent;

	/* Whether all portal activity is reported */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		bool Verbose;
};
