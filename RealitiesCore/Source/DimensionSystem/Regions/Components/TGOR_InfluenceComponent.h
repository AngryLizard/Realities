// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "../TGOR_ElementInstance.h"

#include "TGOR_RegionComponent.h"
#include "TGOR_InfluenceComponent.generated.h"

/**
 * 
 */
UCLASS()
class DIMENSIONSYSTEM_API UTGOR_InfluenceComponent : public UTGOR_RegionComponent
{
	GENERATED_BODY()
	
public:
	UTGOR_InfluenceComponent();

	virtual void RegionTick(float DeltaTime) override;


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** State influence on others */
	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Influence", Meta = (Keywords = "C++"))
		FTGOR_ElementInstance InfluenceState;

	/** Draws influence lines */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Influence", Meta = (Keywords = "C++"))
		bool bDebug;

	/** Radius of influence */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Influence", Meta = (Keywords = "C++"))
		float InfluenceRadius;
	
};
