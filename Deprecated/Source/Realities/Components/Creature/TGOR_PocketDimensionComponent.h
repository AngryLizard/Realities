// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Utility/TGOR_CustomEnumerations.h"

#include "Realities/Components/Dimension/Portals/TGOR_DimensionLoaderComponent.h"
#include "TGOR_PocketDimensionComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REALITIES_API UTGOR_PocketDimensionComponent : public UTGOR_DimensionLoaderComponent
{
	GENERATED_BODY()

public:	
	UTGOR_PocketDimensionComponent();
	
	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/* Identifier to be used for this pocket dimension */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		FString PocketIdentifier;

	/** Sets pocket dimension level volume to given force */
	UFUNCTION(BlueprintCallable, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void SetExternal(const FVector& Force);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Load dimension from content class */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "TGOR Dimension", Meta = (Keywords = "C++"))
		void BuildDimension(UTGOR_Dimension* Dimension);

};
