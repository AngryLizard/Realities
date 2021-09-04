#pragma once


#include "Intersection.h"
#include "ParabolaTree.h"

namespace Voronoi
{
	Intersection::Intersection()
		: left(nullptr),
		right(nullptr)
	{
	}

	Intersection::~Intersection()
	{
	}

	void Intersection::trigger(ParabolaTreeRoot*, CmpHeap<Event*>*& events)
	{
		// Make sure intersection is still valid
		if (intersect()) left->detach(right, events);
	}

	ParabolaTree* Intersection::trigger()
	{
		return(left);
	}

	bool Intersection::intersect()
	{
		// Make sure edges are defined
		if (left == nullptr) return(false);
		if (right == nullptr) return(false);

		Edge* a = left->getEdge();
		Edge* b = right->getEdge();
		// Ignore edges that were already terminated
		if (a == nullptr || a->getEnd() != nullptr) return(false);
		if (b == nullptr || b->getEnd() != nullptr) return(false);
		
		// Intersect edges
		_intersection = a->intersect(b);
		if (_intersection.isNan()) return(false);

		// Get sweep line position of intersection
		Vector c = left->getEdge()->getIn()->getCenter();
		_sweep = _intersection.y + (c - _intersection).size();
		return(true);
	}

	Edge* Intersection::connect()
	{
		// Compute edge from intersection
		Cell* a = right->getEdge()->getIn();
		Cell* b = left->getEdge()->getOut();
		//Cell* c = left->getEdge()->getIn();
		// Make sure there are always two edges
		Edge* edge = new Edge(_intersection, a, b);
		Edge* back = new Edge(edge);

		// Create intersection
		EdgePoint* intersection = new EdgePoint(_intersection);

		// Cut open edges off
		back->terminate(intersection, true);
		left->getEdge()->terminate(intersection, false);
		right->getEdge()->terminate(intersection, false);
		return(edge);
	}

	Vector Intersection::getCenter()
	{
		return(Vector(_intersection.x, _sweep));
	}
}