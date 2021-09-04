// TGOR (C) // CHECKED //
#pragma once

#include "Engine/SceneCapture.h"
#include "Engine/Classes/GameFramework/SpringArmComponent.h"

#include "DimensionSystem/Actors/TGOR_DimensionActor.h"
#include "TGOR_PreviewActor.generated.h"

///////////////////////////////////////////////// DECL ///////////////////////////////////////////////////

class UTGOR_CustomisationComponent;
class UTGOR_RetargetComponent;
class USceneCaptureComponent2D;
class UTGOR_SceneComponent;

/**
* TGOR_PreviewActor 
*/
UCLASS()
class CUSTOMISATIONSYSTEM_API ATGOR_PreviewActor : public ATGOR_DimensionActor
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_PreviewActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	//////////////////////////////////////////////// IMPLEMENTABLES /////////////////////////////////////////

	/** Update material and return capture texture*/
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR System", Meta = (Keywords = "C++"))
		void UpdateTexture(UMaterialInstanceDynamic* Material, UTexture* Texture);

	/** Update material and return capture texture*/
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR System", Meta = (Keywords = "C++"))
		void SetPointerTransform(const FTransform& Transform);

	/** Update material and return capture texture*/
	UFUNCTION(BlueprintImplementableEvent, Category = "!TGOR System", Meta = (Keywords = "C++"))
		bool ProbePointerTransform(const FVector& InLocation, const FVector& InDirection, FVector& OutLocation, FVector& OutDirection, FVector& OutCenter, FVector& OutNormal);

	////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////
private:

	/** Customisation mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Actor, meta = (AllowPrivateAccess = "true"))
		UTGOR_CustomisationComponent* BodypartComponent;

	/** Scene spring component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Actor, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* SpringArmComponent;

	/** Scene capture component. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Actor, meta = (AllowPrivateAccess = "true"))
		USceneCaptureComponent2D* CaptureComponent2D;

public:

	FORCEINLINE USpringArmComponent* GetSpringArmComponent() const { return SpringArmComponent; }
	FORCEINLINE USceneCaptureComponent2D* GetCaptureComponent() const { return CaptureComponent2D; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	UPROPERTY(Transient)
		UTextureRenderTarget2D* CaptureTexture;

	UPROPERTY(Transient)
		UMaterialInstanceDynamic* CaptureMaterial;

	UPROPERTY(Transient)
		FVector TargetLocation;

	UPROPERTY(Transient)
		float TargetArmLength;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Creates rendertarget, automatically called from BeginPlay */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (Keywords = "C++"))
		void InitialiseRenderTarget();

	/** Moves according to settings  */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (Keywords = "C++"))
		void TickRenderTarget(float DeltaTime);

	/** Offsets camera by given delta */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (Keywords = "C++"))
		void OffsetCamera(const FVector2D& Delta);

	/** Focuses camera onto a given node */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (Keywords = "C++"))
		void Focus(int32 NodeIndex);

	/** Linetrace mesh for hit, returns true if a valid mesh face has been hit */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (Keywords = "C++"))
		bool ProbeMesh(USkinnedMeshComponent* ProbeTarget, const FVector& InLocation, const FVector& InDirection, FVector& OutLocation, FVector& OutDirection, FVector& OutCenter, FVector& OutNormal);

	/** Project position from the capture to world coordinates */
	UFUNCTION(BlueprintPure, Category = "!TGOR System", Meta = (Keywords = "C++"))
		bool DeprojectFromCaptureSpace(const FVector2D& CaptureCoords, FVector& WorldLocation, FVector& WorldDirection) const;

	/** Project position to the capture from world coordinates */
	UFUNCTION(BlueprintPure, Category = "!TGOR System", Meta = (Keywords = "C++"))
		bool ProjectToCaptureSpace(const FVector& WorldLocation, FVector2D& CaptureCoords) const;

	/** Capture scene and store into CaptureTexture, returns current material */
	UFUNCTION(BlueprintCallable, Category = "!TGOR System", Meta = (Keywords = "C++"))
		UMaterialInstanceDynamic* Capture();

	/** Material to be used */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR System")
		UMaterialInterface* UIMaterial;

	/** Speed at which camera blends */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR System")
		float BlendSpeed;

	/** Camera distance ratio */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR System")
		float DistanceRatio;

	/** Camera offset target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "!TGOR System")
		float TargetOffset;


	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////
public:
protected:
private:

};
