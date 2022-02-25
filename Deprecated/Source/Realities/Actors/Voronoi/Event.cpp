#pragma once


#include "Event.h"

#include "Cell.h"
#include "ParabolaTree.h"


namespace Voronoi
{
	Event::Event()
		: CmpHeap<Event*>(this, nullptr)
	{
	}

	Event::~Event()
	{
		// Prevent deletion of children
		truncate();
	}

	bool Event::slt(Event* s, Event* t)
	{
		Vector a = s->getCenter();
		Vector b = t->getCenter();
		if (a.y == b.y)
			return(a.x < b.x);
		return(a.y < b.y);
	}
}