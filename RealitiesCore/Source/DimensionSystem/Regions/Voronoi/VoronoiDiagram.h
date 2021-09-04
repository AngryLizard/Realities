#pragma once
#include "SearchActor.h"

#include "Array.h"

namespace Voronoi
{
	class VoronoiDiagram
	{
	public:
		VoronoiDiagram(Array<Cell*>* cells);
		virtual ~VoronoiDiagram();

		// Creates Diagram (cells and edges)
		void create();

		// Triangulates cells
		void triangulate();

	private:
		Array<Cell*>* _cells;
	};
}