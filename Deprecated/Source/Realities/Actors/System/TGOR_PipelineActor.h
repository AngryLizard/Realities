// TGOR (C) // CHECKED //
#pragma once

#include "CoreMinimal.h"
#include "Engine/Classes/Components/SplineComponent.h"

#include "Realities/Utility/Error/TGOR_Error.h"
#include "Realities/Utility/Datastructures/TGOR_Time.h"
#include "Realities/Components/Movement/TGOR_PipelineComponent.h"

#include "Realities/Actors/TGOR_DimensionActor.h"
#include "Realities/Interfaces/TGOR_RegisterInterface.h"
#include "TGOR_PipelineActor.generated.h"


/**
* 
*/
UCLASS()
class REALITIES_API ATGOR_PipelineActor : public ATGOR_DimensionActor
{
	GENERATED_BODY()
	
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_PipelineActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** */

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TGOR Pipeline", meta = (AllowPrivateAccess = "true"))
		USplineComponent* SplineComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		UTGOR_PipelineComponent* PipelineComponent;

public:

	FORCEINLINE UTGOR_PipelineComponent* GetPipeline() const { return PipelineComponent; }
	   
	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////

protected:

	/** 
	UPROPERTY(ReplicatedUsing = OnReplicateAttachments, BlueprintReadOnly, Category = "TGOR Pipeline")
		TArray<FTGOR_PipelineAttachment> Attachments;

	UFUNCTION()
		void OnReplicateAttachments();
	*/

private:
};
