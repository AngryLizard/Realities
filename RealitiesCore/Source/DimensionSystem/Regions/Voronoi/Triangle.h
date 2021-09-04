#pragma once

#include "Vector.h"

namespace Voronoi
{
	class EdgePoint;
	class Cell;

	/* Triangle between 3 cells */
	class Triangle
	{
		public:
			Triangle(int id);
			virtual ~Triangle();

			/* Default comparison function (compare angle) */
			static bool cmp(Triangle* a, Triangle* b, EdgePoint* c);


			/* Check if point is inside this triangle */
			bool inside(Vector vector);
			
			/* Get closest cell */
			Cell* getCell();

			/* Get ID of this triangle */
			int getID();

			/* Get triangle lerps */
			float getR();
			float getS();
			float getT();

			/* Remove from all cells */
			void unlink();

			/* Initialise with variables */
			void link(Cell* cell, EdgePoint* prev, EdgePoint* next);
			
			/* Call <func> on each edge */
			template<typename L>
			void forEachEdge(L func)
			{
				func(_cell->center.p, _prev->p);
				func(_prev->p, _next->p);
				func(_next->p, _cell->center.p);
			}

			/* Call <func> on each cell around prev */
			template<typename L>
			void forEachPrev(L func)
			{
				_prev->forEachEdge([func](Edge* edge) -> bool 
				{
					return(func(edge->getOut()) && func(edge->getIn()));
				});
			}

			/* Call <func> on each cell around next */
			template<typename L>
			void forEachNext(L func)
			{
				_next->forEachEdge([func](Edge* edge) -> bool
				{
					return(func(edge->getOut()) && func(edge->getIn()));
				});
			}

		private:
			int _id;
			Cell* _cell;
			EdgePoint* _prev;
			EdgePoint* _next;

			Vector _m;

			float _fact;
			float _sO, _tO;
			float _r, _s, _t;
			float _sX, _tX, _sY, _tY;

	};
}