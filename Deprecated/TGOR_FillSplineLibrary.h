// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"

#include "TGOR_ProceduralComponent.h"
#include "TGOR_FillSplineComponent.generated.h"

//////////////////////////////////////////// DECL /////////////////////////////////////////////////

class UProceduralMeshComponent;

/**
* UTGOR_FillSplineComponent creates mesh between two splines
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIESPROCEDURAL_API UTGOR_FillSplineComponent : public UTGOR_ProceduralComponent
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	UTGOR_FillSplineComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//////////////////////////////////////////// IMPLEMENTABLES ////////////////////////////////////////

	/**  */

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(BlueprintReadOnly, Category = "Procedural Mesh")
		TWeakObjectPtr<USplineComponent> LeftSpline;

	UPROPERTY(BlueprintReadOnly, Category = "Procedural Mesh")
		TWeakObjectPtr<USplineComponent> RightSpline;

	/** Whether UV projection is enabled */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh|UV")
		bool EnableProjectUV;

	/** Which Spline (0 = Left, 1 = Right) to use for unwrap */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh|UV", meta = (EditCondition = "!EnableProjectUV", ClampMin = 0, ClampMax = 1))
		float UnwrapLane;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Get average distance between spline points */
	UFUNCTION(BlueprintPure, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
		float GetAveragePointDistance() const;

	/** Get maximum distance between spline points */
	UFUNCTION(BlueprintPure, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
		float GetMaxPointDistance() const;

	/** Get minimum distance between spline points */
	UFUNCTION(BlueprintPure, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
		float GetMinPointDistance() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Generate spline mesh */
	UFUNCTION(BlueprintCallable, Category = "Procedural Mesh", Meta = (Keywords = "C++"))
		void Generate(USplineComponent* Left, USplineComponent* Right);
};
