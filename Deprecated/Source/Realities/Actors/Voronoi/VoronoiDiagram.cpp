#pragma once


#include "VoronoiDiagram.h"

namespace Voronoi
{

	VoronoiDiagram::VoronoiDiagram(Array<Cell*>* cells)
		:	_cells(cells)
	{
	}

	VoronoiDiagram::~VoronoiDiagram()
	{
	}

	void VoronoiDiagram::create()
	{

		// -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
		// -- -- Add all cells as create event -- -- //
		// -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

		if (_cells->getLength() == 0)
		{
			return;
		}

		CmpHeap<Event*>* events = nullptr;
		(*_cells)[0]->attach(&events);

		for (int i = 1; i < _cells->getLength(); i++)
		[&] {
			Cell* cell = (*_cells)[i];

			// Check if cell position already exists
			for (int j = i - 1; j >= 0; j--)
			{
				Cell* other = (*_cells)[j];
				Vector diff = cell->center.p - other->center.p;
				if (diff.dot(diff) < FLT_EPSILON)
				{
					return;
				}
			}

			// Add cell to events
			events->insert(cell, &Event::slt);
		} ();

		// -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
		// -- -- -- Step through all events -- -- -- //
		// -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

		ParabolaTreeRoot* parabolas = new ParabolaTreeRoot();

		while (events != nullptr)
		{
			Event* event = events->extract(&Event::slt);
			event->trigger(parabolas, events);
		}

		// -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
		// -- -- -- -- --  Cleanup -- -- -- -- -- -- //
		// -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

		delete(parabolas);
	}



	void VoronoiDiagram::triangulate()
	{
		
		// -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
		// -- -- -- -- Create triangulation -- -- -- //
		// -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

		for (int i = 0; i < _cells->getLength(); i++)
		{

			// -- -- -- -- -- -- -- -- -- -- -- -- -- -- //
			// -- -- Create triangles around region-- -- //
			// -- -- -- -- -- -- -- -- -- -- -- -- -- -- //

			Cell* cell = (*_cells)[i];

			Triangulation triangulation = Triangulation(cell);
			cell->center.forEachEdge([&triangulation](Edge* edge) -> bool
			{
				triangulation.progress(edge);
				return(true);
			});

			triangulation.terminate();
		}
	}
}