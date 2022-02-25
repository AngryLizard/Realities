#pragma once


#include "Cell.h"
#include "ParabolaTree.h"


namespace Voronoi
{

	Cell::Cell()
		:	Cell(Vector(), 0)
	{
	}

	Cell::Cell(Vector v, int id)
		:	Event(),
			center(v),
			_id(id),
			_queryID(-1)
	{
	}

	Cell::~Cell()
	{
		center.forEachJoint([](Triangle* triangle)->bool
		{
			// No need for unlink as no cells share triangles
			delete(triangle);
			return(true);
		});

		center.forEachEdge([](Edge* edge)->bool
		{
			delete(edge); 
			return(true);
		});
	}

	int Cell::getID()
	{
		return(_id);
	}

	void Cell::trigger(ParabolaTreeRoot* tree, CmpHeap<Event*>*& events)
	{
		// Add Cell to priority queue
		tree->insert(trigger(), events);
	}

	ParabolaTree* Cell::trigger()
	{
		return(new ParabolaTree(this));
	}

	Vector Cell::getCenter()
	{
		return(center.p);
	}

	Triangle* Cell::pierce(Vector vector, int queryID)
	{
		if (_queryID == queryID)
			return(nullptr);

		_queryID = queryID;
		Triangle* triangle = nullptr;
		center.forEachJoint([&triangle, &vector](Triangle* t)->bool
		{ 
			if (t->inside(vector))
			{ 
				triangle = t;
				return(false);
			}
			return(true);
		});

		return(triangle);
	}

	Triangle* Cell::pierceNeighbours(Vector vector, int queryID)
	{
		Triangle* triangle = nullptr;

		forEachNeighbour([&vector, &queryID, &triangle](Cell* cell)->bool
		{
			if (cell->_queryID != queryID)
			{
				Triangle* t = cell->pierce(vector, queryID);
				if (t != nullptr) 
				{ 
					triangle = t; 
					return(false); 
				}
			}
			return(true);
		});

		return(triangle);
	}

	Triangle* Cell::pierceAll(Vector vector, int queryID)
	{
		Triangle* triangle = pierce(vector, queryID);
		if (triangle != nullptr) 
			return(triangle);

		forEachNeighbour([&vector, &queryID, &triangle](Cell* cell)->bool 
		{
			if (cell->_queryID != queryID)
			{
				Triangle* t = cell->pierceAll(vector, queryID);
				if (t != nullptr) 
				{
					triangle = t;
					return(false); 
				}
			}
			return(true);
		});

		return(triangle);
	}

	float Cell::parabola(Vector sweep)
	{
		// Get intersection of sweep with parabola
		float dx = center.p.x - sweep.x;
		float dy = center.p.y - sweep.y;

		if (dy == 0)
			return(NAN);

		return((dx * dx - sweep.y * sweep.y + center.p.y * center.p.y) / (2 * dy));
	}
}