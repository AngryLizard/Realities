// The Gateway of Realities: Planes of Existence.

#pragma once

#include "CoreMinimal.h"
#include "Realities/UI/Knowledge/TGOR_KnowledgeNode.h"

#include "Realities/UI/TGOR_Menu.h"
#include "TGOR_KnowledgeTree.generated.h"

///////////////////////////////////////////// ENUMS ///////////////////////////////////////////////////

UENUM(BlueprintType)
enum class ETGOR_BuildEnumeration : uint8
{
	Progress,
	Finished,
	Unsolvable
};

///////////////////////////////////////////// STRUCTS ///////////////////////////////////////////////////


/**
*
*/
USTRUCT(BlueprintType)
struct FTGOR_TreeNode
{
	GENERATED_USTRUCT_BODY()

	FTGOR_TreeNode();

	int32 ComputeDepth(int32 MaxLength) const;
	void SampleVariants(FRandomStream& Stream, int32 MaxLength);
	bool NextVariantIndex();
	void ResetVariantIndex(const FTGOR_TreeCoords& Coords);
	int32 GetCurrentVariant() const;

	/** Sampled variation order */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Knowledge")
		TArray<int32> Variants;

	/** Currently active variation index */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Knowledge")
		int32 VariantIndex;

	/** Current trail in the knowledge tree */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Knowledge")
		TArray<FTGOR_TreeCoords> TreeTrail;

	/** Associated parent node */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Knowledge")
		int32 Parent;

	/** Associated content */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Knowledge")
		UTGOR_Knowledge* Knowledge;

	/** Associated Node Widgets (same index as trail) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TGOR Knowledge")
		TArray<UTGOR_KnowledgeNode*> Nodes;
};

/**
 * 
 */
UCLASS()
class REALITIES_API UTGOR_KnowledgeTree : public UTGOR_Widget
{
	GENERATED_BODY()

		////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	UTGOR_KnowledgeTree();

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Initialise grid for a given root node */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		void BuildGrid(UTGOR_Knowledge* Root, int32 Seed, int32 MinRows, int32 MinCols);

	/** Moves node forward according to variant, movec forward variant and returns current cell index. */
	UFUNCTION()
		int32 MoveNode(int32 Index, int32& Variant);

	/** Checks whether there is a collision between a knowledge and a cell index */
	UFUNCTION()
		bool CollidesWith(int32 Index, int32 Cell, bool Last) const;

	/** Checks whether there is a collision between a knowledge and a cell index as well as neighbourhood */
	UFUNCTION()
		bool HasCollision(int32 Index, int32 Cell, bool Last) const;

	/** Builds tree from a sequence index for given number of max iterations, returns how many iterations are left */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (ExpandEnumAsExecs = "Branches", Keywords = "C++"))
		int32 Build(int32 Iterations, ETGOR_BuildEnumeration& Branches);

	/** Resets a knowledge in the sequence of a given index. */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		void Reset(int32 Index);

	/** Branches current node index out in a given direction, returns whether branching was successful. */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		bool Branch(int32 Index);

	/** Reverses current node index and increases variant, returns false if no variant is available. */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		bool Reverse(int32 Index);

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Populate Nodes recursively from a root knowledge, returns max tree depth */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		int32 PopulateWith(UTGOR_Knowledge* Knowledge, int32 Parent);

	/** Set grid entry (cyclic grid) */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		void GridSet(const FTGOR_TreeCoords& Coords, int32 Index);

	/** Get grid entry (cyclic grid) */
	UFUNCTION(BlueprintPure, Category = "TGOR System", Meta = (Keywords = "C++"))
		int32 GridGet(const FTGOR_TreeCoords& Coords) const;

	/** Get node at given position */
	UFUNCTION(BlueprintPure, Category = "TGOR System", Meta = (Keywords = "C++"))
		UTGOR_Knowledge* GetKnowledgeAt(int32 Row, int32 Col) const;

	/** Get position of a node. Returns whether knowledge was found. */
	UFUNCTION(BlueprintPure, Category = "TGOR System", Meta = (Keywords = "C++"))
		bool GetKnowledgeCoords(UTGOR_Knowledge* Knowledge, int32& Row, int32& Col) const;

	////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	/** Build node widgets */
	UFUNCTION(BlueprintCallable, Category = "TGOR System", Meta = (Keywords = "C++"))
		void BuildNodes();

	/** Check whether a node is supposed to be visible (Can be out of bounds for wrapping behaviour) */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR System", Meta = (Keywords = "C++"))
		bool IsNodeVisible(int32 Row, int32 Col) const;

	/** Allocate node widget */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR System", Meta = (Keywords = "C++"))
		UTGOR_KnowledgeNode* GenerateNode() const;


	/** Move specified note to a given cell */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR System", Meta = (Keywords = "C++"))
		void EnableNode(UTGOR_KnowledgeNode* Node, int32 Row, int32 Col, bool IsTrail);

	/** Set specified note invisible */
	UFUNCTION(BlueprintImplementableEvent, Category = "TGOR System", Meta = (Keywords = "C++"))
		void DisableNode(UTGOR_KnowledgeNode* Node);


	// Rotation sector check around each node to ensure distance between subtrees (-1 for no check, 0 for front only, 1/2 for front)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Knowledge")
		int32 NeighbourCheck;

	// Additional depth number of variants is sampled from
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TGOR Knowledge")
		int32 SampleDepth;

	// Number of rows of grid
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Knowledge")
		int32 GridRows;

	// Number of columns of grid
	UPROPERTY(BlueprintReadOnly, Category = "TGOR Knowledge")
		int32 GridCols;

	////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	/** Current build index */
	UPROPERTY()
		int32 BuildIndex;

	/** Grid for collision detection, contains index of node */
	UPROPERTY()
		TArray<int32> Grid;

	/** List of knowledges in sequence they should be placed in */
	UPROPERTY()
		TArray<FTGOR_TreeNode> LoadingSequence;

	/** List of nodes in this menu */
	UPROPERTY()
		TArray<UTGOR_KnowledgeNode*> Nodes;

	/** Random generator */
	UPROPERTY()
		FRandomStream Stream;
	
};
