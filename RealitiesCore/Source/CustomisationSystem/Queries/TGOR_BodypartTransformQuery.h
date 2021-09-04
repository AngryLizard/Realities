// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"

#include "UISystem/Queries/TGOR_Query.h"
#include "TGOR_BodypartTransformQuery.generated.h"

class UTGOR_BodypartCustomisationWidget;
class ATGOR_PreviewActor;

/**
 * 
 */
UCLASS()
class CUSTOMISATIONSYSTEM_API UTGOR_BodypartTransformQuery : public UTGOR_Query
{
	GENERATED_BODY()
	
public:
	UTGOR_BodypartTransformQuery();


	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Customisation")
		UTGOR_BodypartCustomisationWidget* CallbackWidget;

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Customisation")
		ATGOR_PreviewActor* CurrentPreview;

	UPROPERTY(BlueprintReadOnly, Category = "!TGOR Customisation")
		FTransform CurrentTransform;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	/** Build current transform from mesh probe */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void TransformFromProbe(const FVector& WorldLocation, const FVector& WorldDirection);

	/** Build current transform from parameters */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void TransformFromParams(const FVector& Location, const FVector& Direction, float Twist, float Scale);

	/** Build current transform into parameters */
	UFUNCTION(BlueprintPure, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void ParamsFromTransform(FVector& Location, FVector& Direction, float& Twist, float& Scale) const;

	/** Applies given transform via callback */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void CommitTransform(const FTransform& Transform);

	/** Applies current transform to preview */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void DisplayTransform();

	/** Sets display to possible skins */
	UFUNCTION(BlueprintCallable, Category = "!TGOR Customisation", Meta = (Keywords = "C++"))
		void SetupFromPreview(UTGOR_BodypartCustomisationWidget* CustomisationWidget, ATGOR_PreviewActor* Preview, const FTransform& Transform);
};
