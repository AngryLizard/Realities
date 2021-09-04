// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "CoreSystem/Components/TGOR_SceneComponent.h"
#include "DimensionSystem/Interfaces/TGOR_DimensionInterface.h"
#include "TGOR_ConnectionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DIMENSIONSYSTEM_API UTGOR_ConnectionComponent : public UTGOR_SceneComponent, public ITGOR_DimensionInterface
{
	GENERATED_BODY()

public:
	UTGOR_ConnectionComponent();

	virtual bool PreAssemble(UTGOR_DimensionData* Dimension) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/* Identifiable name of this connection */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		FName ConnectionName;


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/* Whether this portal is meant to be teleported to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR Dimension", Meta = (Keywords = "C++"))
		bool IsPublic;

	/** Returns portal name */
	UFUNCTION(BlueprintPure, Category = "!TGOR Region", Meta = (Keywords = "C++"))
		FName GetConnectionName() const;

};
