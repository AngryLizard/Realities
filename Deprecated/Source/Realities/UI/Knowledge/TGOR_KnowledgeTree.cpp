// The Gateway of Realities: Planes of Existence.
#include "TGOR_KnowledgeTree.h"
#include "Realities/Mod/Knowledges/TGOR_Knowledge.h"

FTGOR_TreeCoords::FTGOR_TreeCoords()
	: Row(0), Col(0), Direction(0)
{
}

FTGOR_TreeCoords::FTGOR_TreeCoords(int32 Row, int32 Col, int32 Direction)
	: Row(Row), Col(Col), Direction(Direction)
{
}

bool FTGOR_TreeCoords::operator==(const FTGOR_TreeCoords& Other) const
{
	return Col == Other.Col && Row == Other.Row;
}

bool FTGOR_TreeCoords::operator!=(const FTGOR_TreeCoords& Other) const
{
	return Col != Other.Col || Row != Other.Row;
}

FTGOR_TreeCoords FTGOR_TreeCoords::Next(int32 rotation) const
{
	FTGOR_TreeCoords Out;
	Out.Direction = UTGOR_Math::PosMod(Direction + rotation, 6);
	switch (Out.Direction)
	{
	case 0:  Out.Row = Row - 1; Out.Col = Col - 0; return Out;
	case 1:  Out.Row = Row - 1; Out.Col = Col + 1; return Out;
	case 2:  Out.Row = Row - 0; Out.Col = Col + 1; return Out;
	case 3:  Out.Row = Row + 1; Out.Col = Col + 0; return Out;
	case 4:  Out.Row = Row + 1; Out.Col = Col - 1; return Out;
	case 5:  Out.Row = Row + 0; Out.Col = Col - 1; return Out;
	}
	return Out;
}


int32 FTGOR_TreeCoords::GetDirectionInverse() const
{
	return (Direction + 3) % 6;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FTGOR_TreeNode::FTGOR_TreeNode()
	: VariantIndex(INDEX_NONE), Parent(INDEX_NONE), Knowledge(nullptr)
{
}


int32 FTGOR_TreeNode::ComputeDepth(int32 MaxLength) const
{
	if (IsValid(Knowledge) && Knowledge->Parents.Num() > 0)
	{
		// Compute tree depth depending on neighbours (currently log2)
		const int32 Cousins = Knowledge->Parents[0]->Children.Num() + 1;
		return FMath::FloorLog2(Cousins) + MaxLength;
	}
	return 0;
}

void FTGOR_TreeNode::SampleVariants(FRandomStream& Stream, int32 MaxLength)
{
	if (!IsValid(Knowledge)) return;
	if (Knowledge->Parents.Num() == 0)
	{
		Variants.Reset();
		return;
	}

	// Number of possible variants depending on depth
	const int32 Depth = ComputeDepth(MaxLength);
	const int32 Count = Depth * Depth - 1;
	Variants.SetNum(Count);
	for (int32 i = 0; i < Count; i++)
	{
		Variants[i] = i;
	}

	if (Count > 2)
	{
		// Permutate order, not perfectly uniform but it gud nuff
		for (int32 i = 0; i < Count * 4; i++)
		{
			const int32 Index = Stream.RandRange(0, Count - 2);
			const int32 Temp = Variants[Index];
			Variants[Index] = Variants[Count - 1];
			Variants[Count - 1] = Temp;
		}
	}
}

bool FTGOR_TreeNode::NextVariantIndex()
{
	return ++VariantIndex < Variants.Num();
}

void FTGOR_TreeNode::ResetVariantIndex(const FTGOR_TreeCoords& Coords)
{
	VariantIndex = 0;
	TreeTrail.SetNum(1);
	TreeTrail[0] = Coords;
}

int32 FTGOR_TreeNode::GetCurrentVariant() const
{
	if (Variants.IsValidIndex(VariantIndex))
	{
		return Variants[VariantIndex];
	}
	return INDEX_NONE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////


UTGOR_KnowledgeTree::UTGOR_KnowledgeTree()
	: Super(), NeighbourCheck(2), SampleDepth(1), GridRows(0), GridCols(0), BuildIndex(INDEX_NONE)
{
}

void UTGOR_KnowledgeTree::BuildGrid(UTGOR_Knowledge* Root, int32 Seed, int32 MinRows, int32 MinCols)
{
	if (!IsValid(Root))
	{
		ERROR("Knowledge tree root is not defined!", Error);
	}

	Stream = FRandomStream(Seed);
	LoadingSequence.Reset();
	const int32 Depth = PopulateWith(Root, -1);

	// Initialise grid to max depth size.
	// Grid is cyclic. If we wanted no overlap Depth * 2 would be safe but that's boring
	GridRows = FMath::Max(MinRows, Depth);
	GridCols = FMath::Max(MinCols, Depth);
	Grid.SetNum(GridRows * GridCols);
	for (int i = 0; i < Grid.Num(); i++)
	{
		Grid[i] = -1;
	}

	// Place root in the middle with random direction
	FTGOR_TreeCoords Center(GridRows / 2, GridCols / 2, Stream.RandRange(0, 5));
	GridSet(Center, 0);

	// Start branching from root
	LoadingSequence[0].Knowledge = Root;
	LoadingSequence[0].ResetVariantIndex(Center);
	BuildIndex = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int32 UTGOR_KnowledgeTree::MoveNode(int32 Index, int32& Variant)
{
	// Extract rotation
	const int32 Div = Variant / 3;
	const int32 Rotation = Variant - Div * 3 - 1;
	Variant = Div;

	// Rotate and move in variant direction
	FTGOR_TreeNode& Node = LoadingSequence[Index];
	const FTGOR_TreeCoords Coords = Node.TreeTrail.Last().Next(Rotation);
	Node.TreeTrail.Emplace(Coords);
	return GridGet(Coords);
}

bool UTGOR_KnowledgeTree::CollidesWith(int32 Index, int32 Cell, bool Last) const
{
	if (Cell == INDEX_NONE) return false; // Empty slot
	if (Last) return true; // Non-empty slot when setting the last node of the trail is always a collision

	const FTGOR_TreeNode& Node = LoadingSequence[Index];
	const FTGOR_TreeNode& Other = LoadingSequence[Cell];

	// Check whether cell is empty, or our track landed on a track of a sibling (but not on the sibling itself)
	const bool IsInside = Other.TreeTrail.Last() == Node.TreeTrail.Last();
	const bool IsSibling = Other.Knowledge->Parents.Num() > 0 && Other.Knowledge->Parents[0] == Node.Knowledge->Parents[0];
	return !IsSibling || IsInside;
}

bool UTGOR_KnowledgeTree::HasCollision(int32 Index, int32 Cell, bool Last) const
{
	// Check current cell first
	if (CollidesWith(Index, Cell, Last)) return true;

	// Check neighbourhood
	const FTGOR_TreeNode& Node = LoadingSequence[Index];
	for (int i = -NeighbourCheck; i <= NeighbourCheck; i++)
	{
		// Don't differentiate last node
		Cell = GridGet(Node.TreeTrail.Last().Next(i));
		if (CollidesWith(Index, Cell, false)) return true;
	}
	return false;
}


int32 UTGOR_KnowledgeTree::Build(int32 Iterations, ETGOR_BuildEnumeration& Branches)
{
	// Abort if end of sequence
	if (BuildIndex < 0 || LoadingSequence.Num() <= BuildIndex)
	{
		Branches = ETGOR_BuildEnumeration::Unsolvable;
		return Iterations;
	}

	// Abort if out of iterations
	if (Iterations < 0)
	{
		Branches = ETGOR_BuildEnumeration::Progress;
		return Iterations;
	}

	// Branch out or reverse if necessary
	if (Branch(BuildIndex))
	{
		BuildIndex++;
		if (BuildIndex >= LoadingSequence.Num())
		{
			Branches = ETGOR_BuildEnumeration::Finished;
			return Iterations;
		}
		Reset(BuildIndex);
	}
	else if (!Reverse(BuildIndex))
	{
		BuildIndex--;
	}

	// Next iteration
	return Build(Iterations - 1, Branches);
}


void UTGOR_KnowledgeTree::Reset(int32 Index)
{
	// Copy parent state
	FTGOR_TreeNode& Node = LoadingSequence[Index];
	if (LoadingSequence.IsValidIndex(Node.Parent))
	{
		const FTGOR_TreeNode& Parent = LoadingSequence[Node.Parent];
		Node.ResetVariantIndex(Parent.TreeTrail.Last());
	}
}

bool UTGOR_KnowledgeTree::Branch(int32 Index)
{
	// Get index state
	FTGOR_TreeNode& Node = LoadingSequence[Index];
	int32 Variant = Node.GetCurrentVariant();
	if (Variant == INDEX_NONE)
	{
		return true;
	}

	// Initial movement to establish whether we're following another trail.
	// This is allowed for tracks from siblings as long as we don't intersect any trail.
	int32 Cell = MoveNode(Index, Variant);
	int32 CurrentTrack = Cell;

	// Break on collision or if we enter another trail
	while (!HasCollision(Index, CurrentTrack, Variant == 0) && Cell == CurrentTrack)
	{

		// Populate grid only if empty
		if (Cell == INDEX_NONE)
		{
			GridSet(Node.TreeTrail.Last(), Index);
		}

		// Abort if we're done
		if (Variant == 0)
		{
			return true;
		}
		Variant = Variant - 1;

		// Move further along trail, switch off current trail if we left it
		Cell = MoveNode(Index, Variant);
		if (Cell == INDEX_NONE)
		{
			CurrentTrack = Cell;
		}
	}

	// Go back a step to not override cell we collided against
	Node.TreeTrail.Pop();
	return false;
}

bool UTGOR_KnowledgeTree::Reverse(int32 Index)
{
	// Reverse trail
	FTGOR_TreeNode& Node = LoadingSequence[Index];
	// First entry holds parent location
	for (int i = 1; i < Node.TreeTrail.Num(); i++)
	{
		// Only remove own path
		const FTGOR_TreeCoords& Coord = Node.TreeTrail[i];
		if (GridGet(Coord) == Index)
		{
			GridSet(Coord, -1);
		}
	}
	Node.TreeTrail.SetNum(1);

	return Node.NextVariantIndex();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int32 UTGOR_KnowledgeTree::PopulateWith(UTGOR_Knowledge* Knowledge, int32 Parent)
{
	int32 MaxDepth = 0;
	if (IsValid(Knowledge))
	{
		// Initialise and add
		FTGOR_TreeNode Node;
		Node.Parent = Parent;
		Node.Knowledge = Knowledge;
		Node.SampleVariants(Stream, SampleDepth);
		const int32 PreDepth = Node.ComputeDepth(SampleDepth);
		Parent = LoadingSequence.Emplace(Node);

		// Populate children
		for (UTGOR_Knowledge* Child : Knowledge->Children)
		{
			const int32 Depth = PopulateWith(Child, Parent);
			MaxDepth = FMath::Max(MaxDepth, Depth);
		}

		return MaxDepth + PreDepth;
	}
	return MaxDepth;
}

void UTGOR_KnowledgeTree::GridSet(const FTGOR_TreeCoords& Coords, int32 Index)
{
	const int32 pRow = UTGOR_Math::PosMod(Coords.Row, GridRows);
	const int32 pCol = UTGOR_Math::PosMod(Coords.Col, GridCols);
	Grid[pCol * GridRows + pRow] = Index;
}

int32 UTGOR_KnowledgeTree::GridGet(const FTGOR_TreeCoords& Coords) const
{
	const int32 pRow = UTGOR_Math::PosMod(Coords.Row, GridRows);
	const int32 pCol = UTGOR_Math::PosMod(Coords.Col, GridCols);
	return Grid[pCol * GridRows + pRow];
}


UTGOR_Knowledge* UTGOR_KnowledgeTree::GetKnowledgeAt(int32 Row, int32 Col) const
{
	const int32 Cell = GridGet(FTGOR_TreeCoords(Row, Col, 0));
	if (LoadingSequence.IsValidIndex(Cell))
	{
		return LoadingSequence[Cell].Knowledge;
	}
	return nullptr;
}

bool UTGOR_KnowledgeTree::GetKnowledgeCoords(UTGOR_Knowledge* Knowledge, int32& Row, int32& Col) const
{
	for (const FTGOR_TreeNode& TreeNode : LoadingSequence)
	{
		if (TreeNode.Knowledge == Knowledge && TreeNode.TreeTrail.Num() > 0)
		{
			// Last entry of trail is the actual node
			const FTGOR_TreeCoords& Coords = TreeNode.TreeTrail.Last();
			Row = Coords.Row;
			Col = Coords.Col;
			return true;
		}
	}
	Row = GridRows / 2;
	Col = GridCols / 2;
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void UTGOR_KnowledgeTree::BuildNodes()
{
	//IsNodeVisible(int32 Row, int32 Col);
	int32 NodeIndex = 0;
	for (int i = 0; i < LoadingSequence.Num(); i++)
	{
		// Go along trail to register nodes
		UTGOR_KnowledgeNode* Node = nullptr;
		FTGOR_TreeNode& TreeNode = LoadingSequence[i];
		TreeNode.Nodes.SetNum(TreeNode.TreeTrail.Num());
		for (int j = 0; j < TreeNode.TreeTrail.Num(); j++)
		{
			// Check whether coord is in range
			const FTGOR_TreeCoords& Coord = TreeNode.TreeTrail[j];
			if (IsNodeVisible(Coord.Row, Coord.Col))
			{
				const int32 Cell = GridGet(Coord);
				if (Cell == i)
				{
					// Add direction to parent node
					if (IsValid(Node))
					{
						Node->AddDirection(Coord.Direction, TreeNode.Knowledge);
					}

					// Add new node to pool if necessary
					while (NodeIndex >= Nodes.Num())
					{
						Nodes.Emplace(GenerateNode());
					}

					// Register node
					Node = Nodes[NodeIndex++];
					if (IsValid(Node))
					{
						const bool IsTrail = j < TreeNode.TreeTrail.Num() - 1;
						EnableNode(Node, Coord.Row, Coord.Col, IsTrail);
						Node->Reset(TreeNode.Knowledge, IsTrail);

						// Only draw from-directions if there is a parent
						if (TreeNode.Parent != INDEX_NONE)
						{
							Node->AddDirection(Coord.GetDirectionInverse(), TreeNode.Knowledge);
						}
					}
				}
				else if(LoadingSequence.IsValidIndex(Cell))
				{
					Node = nullptr;

					// Check whether parent nodes have been initialised
					const FTGOR_TreeNode& TreeParent = LoadingSequence[Cell];
					const int32 Count = TreeParent.TreeTrail.Num();
					if (Count > TreeParent.Nodes.Num())
					{
						ERROR("Trail not yet populated with nodes.", Warning);
					}
					else
					{
						// Find parent node that matches cell
						for (int k = 0; k < Count; k++)
						{
							if (TreeParent.TreeTrail[k] == Coord)
							{
								Node = TreeParent.Nodes[k];
								break;
							}
						}
					}
				}
			}

			// Register on trail
			TreeNode.Nodes[j] = Node;
		}
	}

	for (; NodeIndex < Nodes.Num(); NodeIndex++)
	{
		DisableNode(Nodes[NodeIndex]);
	}
}
