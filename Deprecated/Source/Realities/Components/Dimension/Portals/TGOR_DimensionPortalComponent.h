// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/TGOR_CustomEnumerations.h"
#include "Realities/Base/Instances/Dimension/TGOR_DimensionDataInstance.h"

#include "Realities/Components/Dimension/Portals/TGOR_DimensionLoaderComponent.h"
#include "TGOR_DimensionPortalComponent.generated.h"

class UTGOR_MobilityComponent;
class UTGOR_DimensionData;
class UTGOR_Dimension;
class ATGOR_Pawn;


USTRUCT(BlueprintType)
struct FTGOR_PortalIdentifier
{
	GENERATED_BODY()
	
	/* Target loader name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		FName PortalName;

	/* Target dimension name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		FName DimensionName;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FComponentTeleportedDelegate, UTGOR_PilotComponent*, Component);

UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_DimensionPortalComponent : public UTGOR_DimensionLoaderComponent
{
	GENERATED_BODY()

public:
	UTGOR_DimensionPortalComponent();
	virtual void BeginPlay() override;

	virtual bool PreAssemble(UTGOR_DimensionData* Dimension) override;

	void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;

	virtual bool IsReady() const override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/* Portal name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		FName PortalName;

	/* Target portal identifier */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		FName OtherPortalName;

	/* Portal local offset on arrival */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		FTransform PortalTransform;

	/** Signifies whether this component is in the middle of sending or receiving a teleport. Mostly useful for overlap events. */
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Dimension")
		bool IsMidTeleport;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Registers a given dimension and portal with given identifier */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		virtual void DialPortal(const FTGOR_PortalSelection& Selection);

	/** Assigns portal without loading dimension */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		virtual void DialPortalName(const FName& Portal);

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/* Whether this portal is meant to be teleported to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		bool IsPublic;

	/** Returns portal name */
	UFUNCTION(BlueprintPure, Category = "TGOR Region", Meta = (Keywords = "C++"))
		FName GetPortalName() const;

	/** Set portal transform in world space */
	UFUNCTION(BlueprintCallable, Category = "TGOR Region", Meta = (Keywords = "C++"))
		void SetPortalTransform(const FTransform& Transform);

	/** Connects this portal with the other (setting OtherIdentifier of other portal to this portal) */
	UFUNCTION(BlueprintCallable, Category = "TGOR Region", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		void Connect(ETGOR_ComputeEnumeration& Branches);

	/** Returns the connected portal */
	UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		UTGOR_DimensionPortalComponent* GetPortalComponent() const;

	/** Moves a controllable to linked dimension if loaded, KeepRelative keeps offset of actor to this portal */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		virtual void EnterOther(UTGOR_PilotComponent* Component, bool KeepRelative, const FTransform& Offset, ETGOR_ComputeEnumeration& Branches);

	/** Moves a controllable to this portal */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		virtual void EnterSelf(UTGOR_PilotComponent* Component, const FTransform& Offset, ETGOR_ComputeEnumeration& Branches);

	/** Returns whether a portal can be entered by a given mobility */
	UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		virtual bool IsOccupied(UTGOR_PilotComponent* Component) const;

	/** Returns whether any mobility can teleport from this portal */
	UFUNCTION(BlueprintPure, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		virtual bool CanTeleport(UTGOR_PilotComponent* Component) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	UPROPERTY(BlueprintAssignable, Category = "TGOR Dimension")
		FComponentTeleportedDelegate OnTeleportedComponent;

	/* Whether all portal activity is reported */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		bool Verbose;
};
