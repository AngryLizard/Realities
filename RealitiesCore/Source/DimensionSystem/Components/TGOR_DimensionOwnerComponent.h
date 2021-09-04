// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "TGOR_DimensionLoaderComponent.h"
#include "TGOR_DimensionOwnerComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DIMENSIONSYSTEM_API UTGOR_DimensionOwnerComponent : public UTGOR_DimensionLoaderComponent
{
	GENERATED_BODY()

public:	
	UTGOR_DimensionOwnerComponent();

	virtual bool Assemble(UTGOR_DimensionData* Dimension) override;
	virtual bool IsReady() const override;

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Registers a given dimension and portal with dimension owner */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		void DialOwner();

	/* Target identity */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		int32 ActorIdentifier;

	/** Signifies whether this component is in the middle of sending or receiving a teleport. Mostly useful for overlap events. */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Dimension")
		bool IsMidTeleport;

};
