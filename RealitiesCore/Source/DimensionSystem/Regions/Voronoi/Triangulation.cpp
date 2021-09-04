#pragma once


#include "Triangulation.h"
#include "Cell.h"


namespace Voronoi
{
	Triangulation::Triangulation(Cell* cell)
		:	_cell(cell),
			_counter(0)
	{
		reset();
	}


	Triangulation::~Triangulation()
	{
	}
	

	void Triangulation::reset()
	{
		_counter = 0;
	}
	

	void Triangulation::progress(Edge* edge)
	{
		EdgePoint* next = edge->getEnd();
		if (next == nullptr)
		{
			next = edge->getStart();
		}

		EdgePoint* prev = edge->back->getEnd();
		if (prev == nullptr)
		{
			prev = edge->getStart();
		}

		if (prev != next)
		{
			Triangle* triangle = new Triangle(_counter++);
			triangle->link(_cell, prev, next);
		}
	}

	void Triangulation::terminate()
	{
	}
}