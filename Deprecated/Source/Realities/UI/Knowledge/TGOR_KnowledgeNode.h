// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/Mod/Knowledges/TGOR_Knowledge.h"

#include "Realities/UI/TGOR_Widget.h"
#include "TGOR_KnowledgeNode.generated.h"

class UTGOR_KnowledgeTree;

/**
*
*/
USTRUCT(BlueprintType)
struct FTGOR_TreeCoords
{
	GENERATED_USTRUCT_BODY()

public:
	FTGOR_TreeCoords();
	FTGOR_TreeCoords(int32 Row, int32 Col, int32 Direction);
	bool operator==(const FTGOR_TreeCoords& Other) const;
	bool operator!=(const FTGOR_TreeCoords& Other) const;
	FTGOR_TreeCoords Next(int32 rotation) const;
	int32 GetDirectionInverse() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Knowledge")
		int32 Row;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Knowledge")
		int32 Col;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Knowledge")
		int32 Direction;
};

/**
 * Node of a directed Graph that doubles as its UMG widget to allow
 * building the knowledge"Tree" in the Unreal UMG editor
 */
UCLASS()
class REALITIES_API UTGOR_KnowledgeNode : public UTGOR_Widget
{
	GENERATED_BODY()
	
public:
	UTGOR_KnowledgeNode();

	/** Reset with new knowledge */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		void Reset(UTGOR_Knowledge* Content, bool IsTrail);

	/** Adds a direction to this node */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		void AddDirection(int32 Direction, UTGOR_Knowledge* Other);

	/** Resets directions to current setup */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		void UpdateDirections();

	/** Report flipmap content  */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		void PrintFlipmap();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Set all directions to given mask */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR System", Meta = (Keywords = "C++"))
		void ResetDirections(int32 Index, bool FlipX, bool FlipY);

	/** Set trailmode of this node */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR System", Meta = (Keywords = "C++"))
		void SetTrailMode(bool IsTrail);

	/** Whether a trail should be drawn to a given node */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR System", Meta = (Keywords = "C++"))
		bool HasConnectionTo(UTGOR_Knowledge* Content) const;

	/** Associated knowledge */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Knowledge")
		UTGOR_Knowledge* Knowledge;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Pairs direction and connected knowledges */
	UPROPERTY()
		TMap<int32, UTGOR_Knowledge*> DirectionMask;

};
