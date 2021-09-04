// The Gateway of Realities: Planes of Existence.

#pragma once

#include "TGOR_IdentityComponent.h"
#include "TGOR_TrackedComponent.generated.h"

/**
* TGOR_TrackedComponent gives an actor a permanent identity within the whole world
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class DIMENSIONSYSTEM_API UTGOR_TrackedComponent : public UTGOR_IdentityComponent
{
	GENERATED_BODY()

public:	
	UTGOR_TrackedComponent();
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual bool PreAssemble(UTGOR_DimensionData* Dimension) override;

	virtual void Write_Implementation(FTGOR_GroupWritePackage& Package, UTGOR_Singleton* Context) const override;
	virtual bool Read_Implementation(FTGOR_GroupReadPackage& Package, UTGOR_Singleton* Context) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Get current actor identifier */
	UFUNCTION(BlueprintPure, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		int32 GetTrackedIdentifier() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////	
protected:
	
	/** Tracker identification in current world */
	UPROPERTY(Replicated)
		int32 TrackedIdentity;
};
