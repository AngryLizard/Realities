// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Components/BillboardComponent.h"
#include "TGOR_WorldPainterLayer.h"

#include "GameFramework/Actor.h"
#include "TGOR_WorldPainterBrush.generated.h"

//////////////////////////////////////////// DECL /////////////////////////////////////////////////


/**
* ATGOR_WorldPainterBrush marks a region inside a texture region volume
*/
UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class REALITIESPROCEDURAL_API ATGOR_WorldPainterBrush : public AActor
{
	GENERATED_BODY()

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////// ENGINE ////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////

	ATGOR_WorldPainterBrush(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	void OnConstruction(const FTransform& Transform) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	/** Size of this brush (distance variance) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Painter Brush", Meta = (ClampMin = 0, ClampMax = 1))
		float Size;

	/** Brush color to be applied */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Painter Texture", meta = (ShowOnlyInnerProperties))
		FTGOR_BrushData Brush;

#if WITH_EDITOR
	/** Bake owning painter canvas */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "World Painter Brush", Meta = (Keywords = "C++"))
		void Bake();
#endif

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Data of this brush */
	UPROPERTY()
		UMaterialInstanceDynamic* DynamicMaterial;

	/** Update billboard material */
	UFUNCTION()
		void UpdateBillboardMaterial();

	/** Get layer this brush paints to (attach parent) */
	UFUNCTION()
		ATGOR_WorldPainterLayer* GetPainterLayer() const;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual void PostEditMove(bool bFinished) override;
#endif // WITH_EDITOR

private:

#if WITH_EDITORONLY_DATA
private:
	/** Billboard Component displayed in editor */
	UPROPERTY()
		class UStaticMeshComponent* PreviewComponent;

public:
	/** Returns PreviewComponent subobject **/
	class UStaticMeshComponent* GetPreviewComponent() const;
#endif
};
