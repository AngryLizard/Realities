#pragma once

#include "EdgePoint.h"

namespace Voronoi
{	
	/* Represents one triangulation step */
	class Triangulation
	{
	public:
		Triangulation(Cell* cell);
		virtual ~Triangulation();

		/* Reset internal pointers */
		void reset();

		/* Go further clockwise with triangulation */
		void progress(Edge* edge);

		/* Last step of triangulation */
		void terminate();

	private:	
		Cell* _cell;
		int _counter;
	};

}