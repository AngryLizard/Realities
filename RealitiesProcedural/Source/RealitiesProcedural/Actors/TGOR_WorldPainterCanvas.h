// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Engine/VolumeTexture.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "RealitiesProcedural/Utility/TGOR_Triangulation.h"

#include "GameFramework/Actor.h"
#include "TGOR_WorldPainterCanvas.generated.h"


/**
* ATGOR_WorldPainterCanvas Provides a volume to place texture region brushes.
*/
UCLASS(ClassGroup = (Custom), Blueprintable)
class REALITIESPROCEDURAL_API ATGOR_WorldPainterCanvas : public AActor
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_WorldPainterCanvas(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Painter Bounds")
		AActor* BoundsActor;

	/** Copy bounds of Bounds Actors */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "World Painter Bounds", Meta = (Keywords = "C++"))
		void CopyBounds();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

#if WITH_EDITORONLY_DATA
	/** Box for visualizing virtual texture extents. */
	UPROPERTY(Transient)
		class UBoxComponent* Box = nullptr;
#endif // WITH_EDITORONLY_DATA

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditMove(bool bFinished) override;
#endif // WITH_EDITOR

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	//void GenerateMesh();

};
