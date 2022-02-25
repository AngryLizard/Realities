// The Gateway of Realities: Planes of Existence.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Realities/Utility/Error/TGOR_Error.h"


/**
 * 
 */
class CTGOR_SpatialTree
{
protected:		
	// Cell bounds
	FVector Location;
	FVector Size;

	// Biggest available space
	FVector Space;

public:

	CTGOR_SpatialTree(const FVector& Location, const FVector& Size);

	virtual ~CTGOR_SpatialTree();

	// Check whether bounds have room in this tree
	bool IsInside(const FVector& Point) const;

	// Check whether bounds have room in this tree
	bool HasSpace(const FVector& Bounds) const;

	// Get max bounds between Reference and Space
	FVector GetMax(const FVector& Reference) const;


	// Insert a box into this cell or one of its children
	virtual bool Insert(const FVector& Bounds, FVector& Result) = 0;

	// Remove a leaf at a location
	virtual bool Remove(const FVector& Point) = 0;

	// Calls for each child returning center, extend and whether it's a leaf
	virtual void ForEach(std::function<void(const FVector&, const FVector&, bool)> Func) = 0;
};



class CTGOR_SpatialLeaf : public CTGOR_SpatialTree
{
protected:
public:

	CTGOR_SpatialLeaf(const FVector& Location, const FVector& Size);
	virtual ~CTGOR_SpatialLeaf();

	
	virtual bool Insert(const FVector& Bounds, FVector& Result) override;
	virtual bool Remove(const FVector& Point) override;
	virtual void ForEach(std::function<void(const FVector&, const FVector&, bool)> Func) override;
};



class CTGOR_SpatialBranch : public CTGOR_SpatialTree
{
protected:
	// Children
	TArray<CTGOR_SpatialTree*> Children;

	// Number of children
	int32 Num;

	// Max number of children
	int32 Slices;

	// Split axis
	EAxis::Type Axis;

public:

	CTGOR_SpatialBranch(const FVector& Location, const FVector& Size, EAxis::Type Axis, int32 Slices);

	virtual ~CTGOR_SpatialBranch();
	
	// Get next axis
	EAxis::Type GetNext() const;

	// Slice size
	FVector SliceSize();

	// Set max available space from children
	void UpdateSpace();

	// Inserts and updates 
	bool Insert(CTGOR_SpatialTree* Child, const FVector& Bounds, FVector& Result);


	virtual bool Insert(const FVector& Bounds, FVector& Result) override;
	virtual bool Remove(const FVector& Point) override;
	virtual void ForEach(std::function<void(const FVector&, const FVector&, bool)> Func) override;
};



class CTGOR_SpatialRoot : public CTGOR_SpatialBranch
{
protected:
public:

	CTGOR_SpatialRoot(const FVector& Location, const FVector& Size, int32 Slices);

	virtual bool Insert(const FVector& Bounds, FVector& Result) override;
};