#pragma once


#include "SearchActor.h"


namespace Voronoi
{
	int SearchActor::QUERY_ID = 0;

	SearchActor::SearchActor()
		:	_triangle(nullptr),
			_sub(0),
			_id(0)
	{
	}

	SearchActor::~SearchActor()
	{
	}

	void SearchActor::reset()
	{
		_triangle = nullptr;
	}

	void SearchActor::reset(Cell* cell)
	{
		Triangle*& target = _triangle;

		cell->center.forEachJoint([&target](Triangle* triangle) -> bool
		{
			target = triangle;
			return(false);
		});
	}

	bool SearchActor::isValid()
	{
		return(_triangle != nullptr);
	}

	int SearchActor::getID()
	{
		return(_id);
	}

	int SearchActor::getSubID()
	{
		return(_sub);
	}

	void SearchActor::getSurroundings(int& opposite, int& prev, int& next)
	{
		if (!isValid())
		{
			opposite = prev = next = getSubID();
			return;
		}

		opposite = prev = next = -1;
		_triangle->forEachPrev([&](Cell* cell) -> bool
		{
			int id = cell->getID();
			if (id != _id && opposite != id && prev != id)
			{
				if (opposite == -1)
				{
					opposite = id;
				}
				else if (prev == -1)
				{
					prev = id;
				}
			}
			return(true);
		});

		_triangle->forEachNext([&](Cell* cell) -> bool
		{
			int id = cell->getID();
			if (id != _id && opposite != id && next != id)
			{
				if (prev == id)
				{
					prev = opposite;
					opposite = id;
				}
				else if (next == -1)
				{
					next = id;
				}
			}
			return(true);
		});
	}

	void SearchActor::getLerps(float& prev, float& next, float& meet)
	{
		if (!isValid())
		{
			prev = 1.0f;
			next = meet = 0.0f;
			return;
		}

		prev = _triangle->getS();
		next = _triangle->getT();
		meet = _triangle->getR();
	}

	void SearchActor::update(Vector vector)
	{
		if (!isValid())
		{
			_id = _sub = 0;
			return;
		}

		// Pierce current triangle
		if (_triangle->inside(vector))
		{
			Cell* cell = _triangle->getCell();
			if (cell != nullptr)
			{
				_id = cell->getID();
				_sub = _triangle->getID();
			}
			return;
		}

		// Pierce current region
		int queryID = (QUERY_ID = (QUERY_ID + 1) & 0b1111'1111'1111'1111);
		Cell* cell = _triangle->getCell();
		Triangle* triangle = cell->pierce(vector, queryID);

		// Pierce neighbouring regions
		if (triangle == nullptr)
		{
			triangle = cell->pierceNeighbours(vector, queryID);
		}

		// Pierce the whole rest
		if (triangle == nullptr)
		{
			cell->forEachNeighbour([&vector, &queryID, &triangle](Cell* cell)
			{
				Triangle* t = cell->pierceAll(vector, queryID);
				if (t != nullptr)
				{
					triangle = t;
					return(false);
				}
				return(true);
			});
		}

		// Update variables
		if (triangle != nullptr)
		{
			_triangle = triangle;
			cell = _triangle->getCell();
			if (cell != nullptr)
			{
				_id = cell->getID();
				_sub = _triangle->getID();
			}
		}
	}
}