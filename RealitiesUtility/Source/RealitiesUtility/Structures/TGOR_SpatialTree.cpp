// The Gateway of Realities: Planes of Existence.


#include "TGOR_SpatialTree.h"


CTGOR_SpatialTree::CTGOR_SpatialTree(const FVector& Location, const FVector& Size)
	: Location(Location), Size(Size), Space(FVector::ZeroVector)
{
}

CTGOR_SpatialTree::~CTGOR_SpatialTree()
{
}

bool CTGOR_SpatialTree::IsInside(const FVector& Point) const
{
	return(Location.X <= Point.X && Point.X < Location.X + Size.X &&
		Location.Y <= Point.Y && Point.Y < Location.Y + Size.Y &&
		Location.Z <= Point.Z && Point.Z < Location.Z + Size.Z);
}

bool CTGOR_SpatialTree::HasSpace(const FVector& Bounds) const
{
	return(Bounds.X < Space.X && Bounds.Y < Space.Y && Bounds.Z < Space.Z);
}

FVector CTGOR_SpatialTree::GetMax(const FVector& Reference) const
{
	// If one component is bigger, all of them are
	return(Space.ComponentMax(Reference));
}


CTGOR_SpatialLeaf::CTGOR_SpatialLeaf(const FVector& Location, const FVector& Size)
	: CTGOR_SpatialTree(Location, Size)
{
	// Occupied cells have no available space
	Space = FVector::ZeroVector;
}

CTGOR_SpatialLeaf::~CTGOR_SpatialLeaf()
{
}

bool CTGOR_SpatialLeaf::Insert(const FVector& Bounds, FVector& Result)
{
	// Return min corner as cell location
	Result = Location + Size / 2;
	return(true);
}

bool CTGOR_SpatialLeaf::Remove(const FVector& Point)
{
	return(true);
}

void CTGOR_SpatialLeaf::ForEach(std::function<void(const FVector&, const FVector&, bool)> Func)
{
	const FVector Extend = Size / 2;
	const FVector Center = Location + Extend;
	Func(Center, Extend, true);
}



CTGOR_SpatialBranch::CTGOR_SpatialBranch(const FVector& Location, const FVector& Size, EAxis::Type Axis, int32 Slices)
	: CTGOR_SpatialTree(Location, Size), Num(0), Slices(Slices), Axis(Axis)
{
	Children.SetNumZeroed(Slices);
	Space = SliceSize();
}

CTGOR_SpatialBranch::~CTGOR_SpatialBranch()
{
	// Free all children
	for (CTGOR_SpatialTree*& Child : Children)
	{
		if (Child)
		{
			delete(Child);
			Child = nullptr;
		}
	}
}


EAxis::Type CTGOR_SpatialBranch::GetNext() const
{
	switch (Axis)
	{
	case EAxis::Z: return(EAxis::Y);
	case EAxis::Y: return(EAxis::X);
	case EAxis::X: return(EAxis::Z);
	default: return(EAxis::None);
	}
}

FVector CTGOR_SpatialBranch::SliceSize()
{
	// Slice length
	const float Length = Size.GetComponentForAxis(Axis);
	const float Section = Length / Slices;

	// Compute cell size
	FVector NewSize = Size;
	NewSize.SetComponentForAxis(Axis, Section);
	return(NewSize);
}

void CTGOR_SpatialBranch::UpdateSpace()
{
	// Only update space if full
	if (Num == Slices)
	{
		// Reset space
		Space = FVector::ZeroVector;

		// Get max of all children
		for (CTGOR_SpatialTree* Child : Children)
		{
			Space = Child->GetMax(Space);
		}
	}
	else
	{
		// Space is size of new slice
		Space = SliceSize();
	}
}


bool CTGOR_SpatialBranch::Insert(CTGOR_SpatialTree* Child, const FVector& Bounds, FVector& Result)
{
	bool Success = Child->Insert(Bounds, Result);
	UpdateSpace();
	return(Success);
}

bool CTGOR_SpatialBranch::Insert(const FVector& Bounds, FVector& Result)
{
	// Allocate to already existing child
	for (CTGOR_SpatialTree* Child : Children)
	{
		// Check if there is space and insert
		if (Child && Child->HasSpace(Bounds))
		{
			return(Insert(Child, Bounds, Result));
		}
	}

	// Compute next cell Dimensions
	const float Bound = Bounds.GetComponentForAxis(Axis);
	const float Length = Size.GetComponentForAxis(Axis);
	const float Section = Length / Slices;

	// Compute cell size
	FVector NewSize = Size;

	// Compute cell location
	FVector NewLocation = Location;
	const float Offset = Location.GetComponentForAxis(Axis);

	// Find empty slot
	for (int i = 0; i < Slices; i++)
	{
		if (!Children[i])
		{
			// Transform new dimensions
			NewSize.SetComponentForAxis(Axis, Section);
			NewLocation.SetComponentForAxis(Axis, Offset + Section * i);

			// Create branch
			EAxis::Type Next = GetNext();
			CTGOR_SpatialTree* Child = new CTGOR_SpatialBranch(NewLocation, NewSize, Next, Slices);

			// Determine whether cell can further be split
			if (!Child->HasSpace(Bounds))
			{
				// Create leaf instead
				delete(Child);
				Child = new CTGOR_SpatialLeaf(NewLocation, NewSize);
			}

			Num++;
			Children[i] = Child;
			return(Insert(Child, Bounds, Result));
		}
	}

	return(false);
}

bool CTGOR_SpatialBranch::Remove(const FVector& Point)
{
	// Allocate to already existing child
	for (CTGOR_SpatialTree*& Child : Children)
	{
		// Check if point is inside child
		if (Child && Child->IsInside(Point) && Child->Remove(Point))
		{
			// Remove child
			delete(Child);
			Child = nullptr;
			Num--;
		}
	}

	// Delete if empty
	return(Num == 0);
}

void CTGOR_SpatialBranch::ForEach(std::function<void(const FVector&, const FVector&, bool)> Func)
{
	// Call locally
	const FVector Extend = Size / 2;
	const FVector Center = Location + Extend;
	Func(Center, Extend, false);

	// Call for every child
	for (CTGOR_SpatialTree* Child : Children)
	{
		if (Child)
		{
			Child->ForEach(Func);
		}
	}
}



CTGOR_SpatialRoot::CTGOR_SpatialRoot(const FVector& Location, const FVector& Size, int32 Slices)
	: CTGOR_SpatialBranch(Location, Size, EAxis::Z, Slices)
{
}


bool CTGOR_SpatialRoot::Insert(const FVector& Bounds, FVector& Result)
{
	if (HasSpace(Bounds))
	{
		return(CTGOR_SpatialBranch::Insert(Bounds, Result));
	}
	return(false);
}