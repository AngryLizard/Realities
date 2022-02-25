// The Gateway of Realities: Planes of Existence. By Salireths.

#pragma once

#include "Realities/UI/TGOR_Widget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "TGOR_FractalRender.generated.h"

////////////////////////////////////////////// DECL //////////////////////////////////////////////////////

/** */

////////////////////////////////////////////// MACROS //////////////////////////////////////////////////////

#define ABS(A) ((A)<0.0f?(-(A)):(A))
#define NODE_COUNT 1024

/**
 * TGOR_FractalRender renders a moving fractal
 */
UCLASS()
class REALITIES_API UTGOR_FractalRender : public UTGOR_Widget
{
	GENERATED_BODY()
	
	struct Node
	{
		FVector2D P;
		FVector2D N;
		FVector2D C;
		FVector2D L;
		FVector2D R;
		float T;
		bool B;
	};

public:
	UTGOR_FractalRender(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	bool Initialize() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	UFUNCTION()
		virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	//UFUNCTION()
	//	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	UPROPERTY(EditAnywhere, Category = "TGOR Interface", meta = (ClampMin = "32", ClampMax = "1024"))
		int32 NodeCount;

	UPROPERTY(EditAnywhere, Category = "TGOR Interface", meta = (ClampMin = "16.0", ClampMax = "128.0"))
		float NodeLength;

	UPROPERTY(EditAnywhere, Category = "TGOR Interface", meta = (ClampMin = "0.001", ClampMax = "0.1"))
		float NodeFade;

	UPROPERTY(EditAnywhere, Category = "TGOR Interface")
		FLinearColor FractalColor;

	/////////////////////////////////////////////// INTERNAL ///////////////////////////////////////////
public:

	void setNormal(Node* N);
	void disconnect(Node* N);
	void connect(Node* N, Node* M);
	void setRandom(Node* N);

	bool isFree(const Node* N) const;
	bool isOutOfBounds(const Node* N) const;
	bool isFriend(const Node* N, const Node* M) const;

	Node* getRandomLeaf();
	Node* getClosest(Node* N);

	void generateNodes();
	void entertainNodes();

private:
	FVector2D WidgetSize;
	Node Nodes[NODE_COUNT];
	Node* Empty;
};
