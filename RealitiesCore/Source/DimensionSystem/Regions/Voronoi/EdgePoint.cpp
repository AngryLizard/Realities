#pragma once


#include "EdgePoint.h"
#include "Triangle.h"
#include "Edge.h"

namespace Voronoi
{
	EdgePoint::EdgePoint(Vector vector)
		:	CmpHeap<EdgePoint*>(this, nullptr),
			p(vector),
			_joints(nullptr),
			_region(nullptr),
			_rank(0)
	{
	}

	EdgePoint::~EdgePoint()
	{
		// Prevent deletion of children
		truncate();

		if (_region != nullptr)
			delete(_region);
	}


	
	int EdgePoint::getRank()
	{
		return(_rank);
	}

	void EdgePoint::append(Edge* edge)
	{
		auto slt = [this](Edge* a, Edge* b)->bool
		{
			return((a->getStart()->p - p).cheapAngle() < (b->getStart()->p - p).cheapAngle());
		};

		_rank++;
		if (_region == nullptr)
			_region = new BinarySearchTree<Edge*>(edge, &_region);
		else 
			_region->insert(edge, slt);
	}

	void EdgePoint::append(Triangle* joint)
	{
		auto slt = [this](Triangle* a, Triangle* b)->bool
		{
			return(Triangle::cmp(a, b, this));
		};

		if (_joints == nullptr)
			_joints = new BinarySearchTree<Triangle*>(joint, &_joints);
		else
			_joints->insert(joint, slt);
	}

	void EdgePoint::trunc(Triangle* joint)
	{
		if (_joints == nullptr) return;

		auto slt = [this](Triangle* a, Triangle* b)->bool
		{
			return(Triangle::cmp(a, b, this));
		};

		_joints->remove(joint, slt);
	}
}