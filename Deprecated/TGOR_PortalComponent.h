// The Gateway of Realities: Planes of Existence.

#pragma once

#include "Realities/Components/TGOR_Component.h"
#include "TGOR_PortalComponent.generated.h"

class ATGOR_Dimension;

/**
* TGOR_PortalComponent gives an actor portal functionality
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIES_API UTGOR_PortalComponent : public UTGOR_Component
{
	GENERATED_BODY()

public:
	UTGOR_PortalComponent();

	virtual void BeginPlay() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Portal name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Region", Meta = (Keywords = "C++"))
		FName Name;

	/** Target portal name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Region", Meta = (Keywords = "C++"))
		FName TargetName;

	/** Target portal dimension name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Region", Meta = (Keywords = "C++"))
		FName TargetDimensionName;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Returns portal location */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TGOR Region", Meta = (Keywords = "C++"))
		FVector GetLocation();
		virtual FVector GetLocation_Implementation();
	
};
